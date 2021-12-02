//Voy a usar la EEPROM para guardar los counters
//Al programar el nodo hay que borrar la EEPROM
//En los primeros 4 bytes escribiré el valor del counter up inicial
//y a partir de esa posición iré poniendo a 0 bit a bit con cada nuevo uplink
//Cuando la memoria está vacía el valor de cada byte es FF
//Hay otras soluciones como AVR101: High Endurance EEPROM Storage
//El ATMega328 tiene 1024 bytes de EEPROM
//Voy a usar los 804 [0..803] primeros para uplinks, los 219 siguientes [804..1021] para downlinks
//el 1022 para la confirmación de los uplinks (1 significa No, y 254 significa sí)
//y el último [1023] para los minutos de heartbeat

#define POWER_SENSORS A0
#define LDR A1
#define SOIL_MOISTURE A2
#define POWER_HTU21D A3

#include <lmic.h>
#include "LowPower.h"
#include <hal/hal.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"

HTU21D myHumidity;

boolean envioEnCurso = false;

byte minutosHeartbeat;

unsigned long contadorUPInicial;
unsigned long contadorDOWNInicial;
unsigned long contadorUP;
unsigned long contadorDOWN;

//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// LoRaWAN NwkSKey, network session key
// This should be in big-endian (aka msb).
static const PROGMEM u1_t NWKSKEY[16] = {FILLMEIN};

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = {FILLMEIN};

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this should be in big-endian (aka msb) too.
static const u4_t DEVADDR = FILLMEIN; // <-- Change this address for every node!

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

//static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
//const unsigned TX_INTERVAL = 60;

// Pin mapping
// Adapted for Feather M0 per p.10 of [feather]
// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_UNUSED_PIN,
  .dio = {2, 3, LMIC_UNUSED_PIN},
};

void reset() {
  asm volatile ("jmp 0");
}

long readVcc() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));

  long result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_RFU1:
      ||     Serial.println(F("EV_RFU1"));
      ||     break;
    */
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        //El downlink del periodo de heartbeat tendrá el formato canal (04), tipo de valor (01=digital output), valor (0 a 255), FF (ACK)
        //Sólo voy a atender los downlink que entren por el puerto 99
        //Serial.println(LMIC.dataLen);
        if (LMIC.frame[LMIC.dataBeg - 1] == 99) {
          if (LMIC.frame[LMIC.dataBeg + 0] == 0x04 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para el heartbeat por el canal 4
            byte periodoHeartbeat = LMIC.frame[LMIC.dataBeg + 2];
            if (periodoHeartbeat > 0) {
              EEPROM.write(1023, periodoHeartbeat);
              minutosHeartbeat = periodoHeartbeat;
            }
          }
        }
      }
      //Tengo que almacenar los counters
      actualizarContador(0, LMIC.seqnoUp);
      contadorUP = LMIC.seqnoUp;

      if (contadorDOWN != LMIC.seqnoDn) {
        actualizarContador(804, LMIC.seqnoDn);
        contadorDOWN = LMIC.seqnoDn;
      }
      //Tengo que ponerlo después de actualizar los contadores
      //Pues de otro modo resetea y vuelve a enviar el mismo uplink,
      //que recibe nuevamente el reset y entra en un bucle sin fin
      if (LMIC.frame[LMIC.dataBeg + 0] == 0x05 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
        //downlink para resetear
        if (LMIC.frame[LMIC.dataBeg + 2] == 1) {
          reset();
        }
      }
      envioEnCurso = false;
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
    */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}



void setup() {
  pinMode(POWER_SENSORS, OUTPUT);
  digitalWrite(POWER_SENSORS, LOW);
  pinMode(POWER_HTU21D, OUTPUT);
  digitalWrite(POWER_HTU21D, LOW);

  while (!Serial); // wait for Serial to be initialized
  Serial.begin(115200);
  delay(100);     // per sample code on RF_95 test
  Serial.println(F("Starting"));


  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100); //Para mejorar la recepción de los downlinks

  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x13, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_eu868)
  // Set up the channels used by the Things Network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set. The LMIC doesn't let you change
  // the three basic settings, but we show them here.
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
  // TTN defines an additional channel at 869.525Mhz using SF9 for class B
  // devices' ping slots. LMIC does not have an easy way to define set this
  // frequency and support for class B is spotty and untested, so this
  // frequency is not configured here.
#elif defined(CFG_us915) || defined(CFG_au915)
  // NA-US and AU channels 0-71 are configured automatically
  // but only one group of 8 should (a subband) should be active
  // TTN recommends the second sub band, 1 in a zero based count.
  // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
  LMIC_selectSubBand(1);
#elif defined(CFG_as923)
  // Set up the channels used in your country. Only two are defined by default,
  // and they cannot be changed.  Use BAND_CENTI to indicate 1% duty cycle.
  // LMIC_setupChannel(0, 923200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  // LMIC_setupChannel(1, 923400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);

  // ... extra definitions for channels 2..n here
#elif defined(CFG_kr920)
  // Set up the channels used in your country. Three are defined by default,
  // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
  // BAND_MILLI.
  // LMIC_setupChannel(0, 922100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(1, 922300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(2, 922500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

  // ... extra definitions for channels 3..n here.
#elif defined(CFG_in866)
  // Set up the channels used in your country. Three are defined by default,
  // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
  // BAND_MILLI.
  // LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

  // ... extra definitions for channels 3..n here.
#else
# error Region not supported
#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  LMIC_setAdrMode(0);
  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF8, 14);
  //Leo los contadores

  contadorUPInicial = EEPROM.get( 0, contadorUPInicial );
  contadorDOWNInicial = EEPROM.get( 804, contadorDOWNInicial );
  contadorUP = contadorUPInicial ;
  contadorDOWN = contadorDOWNInicial ;
  //Si alguno de ellos es FFFFFFFF quiere decir que es el primer arranque
  if (contadorUPInicial == 0xFFFFFFFF) {
    contadorUPInicial = 0;
    contadorUP = 0 ;
    EEPROM.put(0, contadorUPInicial);
    EEPROM.write(4, 0xFF);
  } else {
    //Tengo que leer cuántos ceros consecutivos hay para sumarselos al valor inicial
    for (int i = 4; i < 804; i++) {
      byte lectura = EEPROM.read(i);
      if (lectura == 0) {
        contadorUP = contadorUP + 8;
      } else {
        contadorUP = contadorUP + ultimoByte(lectura);
        break;
      }
    }
  }

  LMIC.seqnoUp = contadorUP;
  if (contadorDOWNInicial == 0xFFFFFFFF) {
    contadorDOWNInicial = 0;
    contadorDOWN = 0;
    EEPROM.put(804, contadorDOWNInicial);
    EEPROM.write(808, 0xFF);
  } else {
    //Tengo que leer cuántos ceros consecutivos hay para sumarselos al valor inicial
    for (int i = 808; i <= 1021; i++) {
      byte lectura = EEPROM.read(i);
      if (lectura == 0) {
        contadorDOWN = contadorDOWN + 8;
      } else {
        contadorDOWN = contadorDOWN + ultimoByte(lectura);
        break;
      }
    }
  }
  LMIC.seqnoDn = contadorDOWNInicial ;
  contadorDOWN = contadorDOWNInicial ;
  minutosHeartbeat = EEPROM.read(1023);
  if (minutosHeartbeat == 0 || minutosHeartbeat == 255) {
    minutosHeartbeat = 2;
  }
minutosHeartbeat = 2;

  enviarMensaje();
  // Start job
  //do_send(&sendjob);
}

void loop() {
  //Esperamos a que concluya el envío
  while (envioEnCurso) {
    os_runloop_once();
  }

  for (byte minutos = 0; minutos < minutosHeartbeat; minutos++) {

    for (byte contador = 0; contador < 7; contador++) {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
  }

  enviarMensaje();

}

void enviarMensaje() {
  envioEnCurso = true;
  os_setCallback (&sendjob, do_send);
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    static uint8_t payload[5];
    //digitalWrite(POWER_SENSORS, HIGH);
    int32_t comodin;
    comodin =  readVcc(); //Tensión en milivoltios

    payload[0] = comodin >> 8;
    payload[1] = comodin;
/*
    int medidas[10], medidas_ordenadas[10], minimo, indice_minimo;
    for (byte i = 0; i < 10; i++) {
      medidas[i] = analogRead(LDR);
      delay(100);
    }
    for (byte i = 0; i < 10; i++) {
      minimo = 1023;
      for (byte j = 0; j < 10; j++) {
        if (medidas[j] < minimo) {
          minimo = medidas[j];
          indice_minimo = j;
        }
      }
      medidas_ordenadas[i] = medidas[indice_minimo];
      medidas[indice_minimo] = 1023;
    }
    comodin = (medidas_ordenadas[4] + medidas_ordenadas[5]) / 2;
    payload[4] = comodin >> 8;
    payload[5] = comodin;
    //Tomo la mediana de 10 medidas del sensor de humedad del suelo
    for (byte i = 0; i < 10; i++) {
      medidas[i] = analogRead(SOIL_MOISTURE);
      delay(100);
    }
    for (byte i = 0; i < 10; i++) {
      minimo = 1023;
      for (byte j = 0; j < 10; j++) {
        if (medidas[j] < minimo) {
          minimo = medidas[j];
          indice_minimo = j;
        }
      }
      medidas_ordenadas[i] = medidas[indice_minimo];
      medidas[indice_minimo] = 1023;//Lo pongo al máximo
    }
    comodin = (medidas_ordenadas[4] + medidas_ordenadas[5]) / 2;
    payload[2] = comodin >> 8;
    payload[3] = comodin;
    digitalWrite(POWER_SENSORS, LOW);
    delay(500);
    */
    digitalWrite(POWER_HTU21D, HIGH);
    delay(1000);
    myHumidity.begin();
    float comodinHTU21D;

    comodinHTU21D = myHumidity.readTemperature();
    Serial.println(comodinHTU21D);
    payload[2] = ((int)(comodinHTU21D * 10)) >> 8;
    payload[3] = ((int)(comodinHTU21D * 10));
    comodinHTU21D = myHumidity.readHumidity(); //%
    Serial.println(comodinHTU21D);
    payload[4] = ((int)(comodinHTU21D * 2));
    digitalWrite(POWER_HTU21D, LOW);
    //Para evitar fuga de corriente por las pullup del i2c
    digitalWrite( SDA, LOW);
    digitalWrite( SCL, LOW);
    LMIC_setTxData2(1, payload, 5 , 0);
    Serial.println(F("Packet queued"));


  }
  // Next TX is scheduled after TX_COMPLETE event.
}
byte ultimoByte(byte lectura) {
  byte i = 8;
  while (lectura != 0) {
    lectura = lectura >> 1;
    i--;
  }
  return i;
  /*
    switch (lectura) {
    case 0xFF:
      //No sumo nada
      return 0;
      break;
    case 0x7F:
      return 1;
      break;
    case 0x3F:
      return 2;
      break;
    case 0x1F:
      return 3;
      break;
    case 0x0F:
      return 4;
      break;
    case 0x07:
      return 5;
      break;
    case 0x03:
      return 6;
      break;
    case 0x01:
      return 7;
      break;
    }
  */
}
void actualizarContador(int direccion, long contador) {
  int diferencia;
  if (direccion == 0) {
    //contadorUPInicial
    diferencia = contador - contadorUPInicial;
  } else {
    //contadorDOWNInicial
    diferencia = contador - contadorDOWNInicial;
  }
  int direccionDelta = (diferencia - 1) / 8;
  byte resto = diferencia % 8;
  if (direccion == 0 && direccionDelta == 799 && resto == 8) {
    //Hay que reiniciar
    EEPROM.put(0, contador);
    contadorUPInicial = contador;
    EEPROM.write(4, 0xFF);
    return;
  } else if (direccion == 804 && direccionDelta == 213 && resto == 8) {
    //Hay que reiniciar
    EEPROM.put(804, contador);
    contadorDOWNInicial = contador;
    EEPROM.write(808, 0xFF);
    return;
  }
  byte valor;
  switch (resto) {
    case 0:
      valor = 0x00;
      //Reseteo el byte siguiente
      EEPROM.write(direccion + 4 + direccionDelta + 1, 0xFF);
      break;
    case 1:
      valor = 0x7F;
      break;
    case 2:
      valor = 0x3F;
      break;
    case 3:
      valor = 0x1F;
      break;
    case 4:
      valor = 0x0F;
      break;
    case 5:
      valor = 0x07;
      break;
    case 6:
      valor = 0x03;
      break;
    case 7:
      valor = 0x01;
      break;
  }
  EEPROM.write(direccion + 4 + direccionDelta, valor);
  return;
}
