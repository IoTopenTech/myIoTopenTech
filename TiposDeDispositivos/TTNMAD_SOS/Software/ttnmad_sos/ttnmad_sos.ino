//Voy a usar la EEPROM para guardar los counters
//Al programar el nodo hay que borrar la EEPROM
//En los primeros 4 bytes escribiré el valor del counter up inicial
//y a partir de esa posición iré poniendo a 0 bit a bit con cada nuevo uplink
//Cuando la memoria está vacía el valor de cada byte es FF
//Hay otras soluciones como AVR101: High Endurance EEPROM Storage
//El ATMega328 tiene 1024 bytes de EEPROM
//Voy a usar los 804 [0..803] primeros para uplinks, los 219 siguientes [804..1020] para downlinks
//el 1021 para el modoLocalizacion (0 - Sólo GPS, 1 - Sólo WiFi, 2 - Ambos)
//el 1022 para los minutos de heartbeat en alarma
//y el último [1023] para los minutos de heartbeat fuera de alarma

#define EN_GPS 6
#define EN_ESP 7
#define LED 8
#define SOS 2
#define TIMER 3


#include <lmic.h>
#include "LowPower.h"
#include <hal/hal.h>
#include <SPI.h>
#include <EEPROM.h>



#include <NMEAGPS.h>
#define gpsPort Serial
NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values
boolean FIXconseguido = false;
boolean FIXconseguido_preciso = false;
byte precisionFIX = 0;
boolean GPSarrancado = false;
float latitud, longitud;//, altitud;

uint8_t payload[21];
byte MACconseguidas = 0;

volatile boolean envioEnCurso = false;
volatile boolean aplicarAntirrebote = false;

boolean modoAlarma = false;
byte modoLocalizacion; // 0 - Sólo GPS, 1 - Sólo WiFi, 2 - Ambos
byte minutosHeartbeatSinAlarma;
byte minutosHeartbeatConAlarma;

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
static const PROGMEM u1_t NWKSKEY[16] = {0x05, 0x84, 0x4D, 0x16, 0xC7, 0xF2, 0x28, 0x44, 0x2A, 0x87, 0x1E, 0x30, 0x46, 0x04, 0xF5, 0xF0};

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = { 0xB3, 0x4B, 0xAA, 0xE3, 0x5F, 0x3E, 0xE7, 0xF3, 0xB8, 0xB8, 0xC7, 0x0E, 0xD1, 0x89, 0x23, 0xD2};

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this should be in big-endian (aka msb) too.
static const u4_t DEVADDR = 0x260B4196; // <-- Change this address for every node!

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
  .dio = {4, 5, LMIC_UNUSED_PIN},
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
        //El downlink del periodo de heartbeat con alarma tendrá el formato canal (04), tipo de valor (01=digital output), valor (0 a 255), FF (ACK)
        //Sólo voy a atender los downlink que entren por el puerto 99
        //Serial.println(LMIC.dataLen);
        if (LMIC.frame[LMIC.dataBeg - 1] == 99) {
          if (LMIC.frame[LMIC.dataBeg + 0] == 0x04 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para el heartbeat por el canal 4
            byte periodoHeartbeat = LMIC.frame[LMIC.dataBeg + 2];
            if (periodoHeartbeat > 0) {
              EEPROM.write(1022, periodoHeartbeat);
              minutosHeartbeatConAlarma = periodoHeartbeat;
            }
          } else if (LMIC.frame[LMIC.dataBeg + 0] == 0x05 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para el heartbeat sin alarma por el canal 5
            byte periodoHeartbeat = LMIC.frame[LMIC.dataBeg + 2];
            if (periodoHeartbeat > 0) {
              EEPROM.write(1023, periodoHeartbeat);
              minutosHeartbeatSinAlarma = periodoHeartbeat;
            }
          } else if (LMIC.frame[LMIC.dataBeg + 0] == 0x06 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para entrar o salir del modo alarma
            modoAlarma = LMIC.frame[LMIC.dataBeg + 2];
            digitalWrite(LED, modoAlarma);
          } else if (LMIC.frame[LMIC.dataBeg + 0] == 0x03 && LMIC.frame[LMIC.dataBeg + 1] == 0x01 && LMIC.frame[LMIC.dataBeg + 2] <= 2) {
            //downlink para configurar el modo de localización
            byte comodin = LMIC.frame[LMIC.dataBeg + 2];
            if (modoLocalizacion != comodin && comodin <= 2) {
              modoLocalizacion = comodin;
              EEPROM.write(1021, comodin);
              delay(10);
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
      //Tengo que poner el reset después de actualizar los contadores
      //Pues de otro modo resetea y vuelve a enviar el mismo uplink,
      //que recibe nuevamente el reset y entra en un bucle sin fin
      if (LMIC.frame[LMIC.dataBeg + 0] == 0x07 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
        //downlink para resetear
        if (LMIC.frame[LMIC.dataBeg + 2] == 1) {
          //Reseteo los contadores y los periodos
          contadorUP = 0;
          EEPROM.put(0, contadorUP);
          EEPROM.write(4, 0xFF);
          contadorDOWN = 0;
          EEPROM.put(804, contadorDOWN);
          EEPROM.write(808, 0xFF);
          EEPROM.write(1022, 5);
          EEPROM.write(1023, 240);
          delay(100);
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
  pinMode(EN_GPS, OUTPUT);
  digitalWrite(EN_GPS, LOW);
  pinMode(EN_ESP, OUTPUT);
  digitalWrite(EN_ESP, LOW);
  pinMode(SOS, INPUT_PULLUP);
  digitalWrite(TIMER, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  while (!Serial); // wait for Serial to be initialized
  Serial.begin(9600);
  delay(100);     // per sample code on RF_95 test
  Serial.println(F("Starting"));


  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  //En la mayoría de los casos ya no es necesario configurar el ERROR por
  //un error que existía en los cálculos de tiempo
  //Esto es una maravilla porque nos ahorra problemas al usar distintos SF
  //https://github.com/mcci-catena/arduino-lmic#controlling-protocol-timing
  //LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100); //Para mejorar la recepción de los downlinks

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
  LMIC_setDrTxpow(DR_SF10, 14);
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
    delay(10);
  } else {
    //Tengo que leer cuántos ceros consecutivos hay para sumarselos al valor inicial
    for (int i = 4; i < 804; i++) {
      byte lectura = EEPROM.read(i);
      if (lectura == 0) {
        contadorUP = contadorUP + 8;
      } else if (lectura == 0xFF) {
        break;
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
    delay(10);
  } else {
    //Tengo que leer cuántos ceros consecutivos hay para sumarselos al valor inicial
    for (int i = 808; i <= 1020; i++) {
      byte lectura = EEPROM.read(i);
      if (lectura == 0) {
        contadorDOWN = contadorDOWN + 8;
      } else if (lectura == 0xFF) {
        break;
      } else {
        contadorDOWN = contadorDOWN + ultimoByte(lectura);
        break;
      }
    }
  }
  LMIC.seqnoDn = contadorDOWNInicial ;
  contadorDOWN = contadorDOWNInicial ;
  minutosHeartbeatSinAlarma = EEPROM.read(1023);
  if (minutosHeartbeatSinAlarma == 0 || minutosHeartbeatSinAlarma == 255) {
    minutosHeartbeatSinAlarma = 30;
    EEPROM.write(1023, 240);
    delay(10);
  }
  minutosHeartbeatConAlarma = EEPROM.read(1022);
  if (minutosHeartbeatConAlarma == 0 || minutosHeartbeatConAlarma == 255) {
    minutosHeartbeatConAlarma = 3;
    EEPROM.write(1022, 5);
    delay(10);
  }
  modoLocalizacion = EEPROM.read(1021);
  if (modoLocalizacion > 2) {
    modoLocalizacion = 2;
    EEPROM.write(1021, 2);
    delay(10);
  }
  Serial.print("Contador UP = ");
  Serial.println(contadorUP);
  Serial.print("Contador DOWN = ");
  Serial.println(contadorDOWN);
  Serial.print("Periodo sin alarma = ");
  Serial.println(minutosHeartbeatSinAlarma);
  Serial.print("Periodo con alarma = ");
  Serial.println(minutosHeartbeatConAlarma);
  // Start job
  enviarMensaje();
  attachInterrupt(digitalPinToInterrupt(SOS), botonPulsado, FALLING);
}

void loop() {
  extern volatile unsigned long timer0_overflow_count;
  extern volatile unsigned long timer0_millis;
  //Esperamos a que concluya el envío en curso si lo hay
  while (envioEnCurso == true || os_queryTimeCriticalJobs(ms2osticks(60000))) {
    os_runloop_once();
    if (aplicarAntirrebote == true) {
      funcionAplicarAntirrebote();
    }
    delay(10);
  }

  if (aplicarAntirrebote == true) {
    funcionAplicarAntirrebote();
  }
  byte minutosHeartbeat;
  unsigned long crono;
  boolean modoAlarmaLocal = modoAlarma;
  FIXconseguido = false;
  FIXconseguido_preciso = false;
  MACconseguidas = 0;

  if (modoAlarma == true) {
    minutosHeartbeat = minutosHeartbeatConAlarma;
  } else {
    //No estamos en modo alarma
    minutosHeartbeat = minutosHeartbeatSinAlarma;
    digitalWrite(EN_GPS, LOW);
    digitalWrite(EN_ESP, LOW);
  }
  if (!modoAlarma || modoLocalizacion == 1) {
    payload[3] = 0;
    payload[4] = 0;
    payload[5] = 0;
    payload[6] = 0;
    payload[7] = 0;
    payload[8] = 0;
    precisionFIX = 0;
  }
  for (byte minutos = 0; minutos < minutosHeartbeat; minutos++) {
    if (modoAlarmaLocal != modoAlarma) {
      //Se ha pulsado el botón
      break;
    }
    //Dos minutos antes de enviar el heartbeat activamos el GPS
    //y lo desactivaremos 2 ciclos antes de terminar el plazo para tomar las MACs con el ESP03
    if (modoAlarmaLocal == true && (minutosHeartbeat - minutos) <= 2 && FIXconseguido_preciso == false && (modoLocalizacion == 0 || modoLocalizacion == 2)) {
      digitalWrite(EN_GPS, HIGH);
      GPSarrancado = true;
    } else {
      digitalWrite(EN_GPS, LOW);
      GPSarrancado = false;
    }
    for (byte contador = 0; contador < 7; contador++) {
      if (modoAlarmaLocal != modoAlarma) {
        break;
      }
      if (aplicarAntirrebote == true) {
        funcionAplicarAntirrebote();
      }
      if (modoAlarmaLocal == true) {
        //Estamos en modo alarma
        if ( minutosHeartbeat - minutos == 1 && contador >= 5 && (modoLocalizacion == 1 || modoLocalizacion == 2)) {
          //Estamos en el plazo del ESP03
          digitalWrite(EN_GPS, LOW);
          GPSarrancado = false;
          //Intento lograr 2 MACs
          if ( MACconseguidas < 2) {
            delay(150);
            digitalWrite(EN_ESP, HIGH);
            delay(150);
            //Vacío el buffer
            while (Serial.available()) {
              Serial.read();
              delay(10);
            }
            byte byteSerial;
            byte i = 0;
            crono = millis();
            while ((millis() - crono < 8000) && MACconseguidas < 2 ) {
              if (aplicarAntirrebote == true) {
                funcionAplicarAntirrebote();
              }
              if (modoAlarmaLocal != modoAlarma) {
                break;
              }
              delay(100);
              if (Serial.available()) {
                byteSerial = Serial.read();
              }
              if (byteSerial != '#') {
                if (i < 12) {
                  payload[9 + i] = byteSerial;
                  i++;
                } else {
                  //Esto no debería ocurrir
                  i = 0;
                  MACconseguidas = 0;
                }
              } else {//Es un signo #
                if (i == 6) {
                  if (Serial.available()) {
                    //No hago nada para capturar también la segunda MAC
                  } else {
                    //No hay segunda MAC
                    MACconseguidas = 1;
                    break;
                  }
                } else if (i == 12) {
                  //Tengo 2 MACs
                  MACconseguidas = 2;
                  break;
                } else {
                  //He cogido una transmisión del esp03 a medias (ajusto el offset)
                  i = 0;
                }
              }
            }
          } else {
            digitalWrite(EN_ESP, LOW);
            
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            //Give the AVR back the slept time back (simple version)
            /*
              cli();
              timer0_millis += 8000;
              timer0_overflow_count+= 8 * 64 * clockCyclesPerMicrosecond();
              sei();
              os_getTime();   //VERY IMPORTANT after sleep to update os_time and not cause txbeg and os_time out of sync which causes send delays with the RFM95 on and eating power
              os_runloop_once();
            */
          }
        } else if (GPSarrancado && !FIXconseguido_preciso && (modoLocalizacion == 0 || modoLocalizacion == 2)) {
          //Estamos en el plazo del GPS
          crono = millis();
          while ((millis() - crono < 8000) && !FIXconseguido_preciso ) {
            if (aplicarAntirrebote == true) {
              funcionAplicarAntirrebote();
            }
            if (modoAlarmaLocal != modoAlarma) {
              break;
            }
            while (gps.available(Serial)) {
              fix = gps.read();
              if (fix.valid.location ) {
                FIXconseguido = true;
                //Sólo apago el GPS si el fix tiene cierta calidad (HDOP<3)
                if (fix.valid.hdop) {
                  //La librería entrega HDOP en 1/1000
                  precisionFIX = fix.hdop / 1000;
                  if (precisionFIX < 3) {
                    FIXconseguido_preciso = true;
                    digitalWrite(EN_GPS, LOW);
                    GPSarrancado = false;
                    
                  }
                }
                
                int32_t comodin;
                comodin = ((float)fix.latitude()) * 40000;
                if (comodin < 0) {
                  comodin = -comodin;
                  payload[3] = (comodin >> 16) | 0x80;
                } else {
                  payload[3] = (comodin >> 16);
                }
                payload[4] = comodin >> 8;
                payload[5] = comodin;
                comodin = ((float)fix.longitude()) * 40000;
                if (comodin < 0) {
                  comodin = -comodin;
                  payload[6] = (comodin >> 16) | 0x80;
                } else {
                  payload[6] = (comodin >> 16);
                }
                payload[7] = comodin >> 8;
                payload[8] = comodin;
                
              }

            }
          }

        } else {
          //No hemos llegado aún al plazo del GPS ni del ESP03 o estamos entre el uno y el otro
          LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
          //Give the AVR back the slept time back (simple version)
          // timer0 uses a /64 prescaler and overflows every 256 timer ticks
          /*
            cli();
            timer0_millis += 8000;
            timer0_overflow_count+= 8 * 64 * clockCyclesPerMicrosecond();
            sei();
            os_getTime();   //VERY IMPORTANT after sleep to update os_time and not cause txbeg and os_time out of sync which causes send delays with the RFM95 on and eating power
            os_runloop_once();
          */
        }
      } else {
        //No estamos en modo alarma
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
        //Give the AVR back the slept time back (simple version)
        /*
          cli();
          timer0_millis += 8000;
          timer0_overflow_count+= 8 * 64 * clockCyclesPerMicrosecond();
          sei();
          os_getTime();   //VERY IMPORTANT after sleep to update os_time and not cause txbeg and os_time out of sync which causes send delays with the RFM95 on and eating power
          os_runloop_once();
        */
      }
    }
  }
  //Enviamos un heartbeat
  digitalWrite(EN_GPS, LOW);
  GPSarrancado = false;
  digitalWrite(EN_ESP, LOW);
  Serial.println(F("Voy a mandar un mensaje"));
  enviarMensaje();
}


void funcionAplicarAntirrebote() {
  delay(100);//Antirrebote largo
  aplicarAntirrebote = false;
  if (digitalRead(SOS) == HIGH) {
    attachInterrupt(digitalPinToInterrupt(SOS), botonPulsado, FALLING);
  } else {
    attachInterrupt(digitalPinToInterrupt(SOS), botonDespulsado, RISING);
  }
}
void enviarMensaje() {
  envioEnCurso = true;
  os_setCallback (&sendjob, do_send);
}

void botonPulsado() {
  detachInterrupt(digitalPinToInterrupt(SOS));
  //attachInterrupt(digitalPinToInterrupt(SOS), botonDespulsado, RISING);
  modoAlarma = !modoAlarma;
  digitalWrite(LED, modoAlarma);
  aplicarAntirrebote = true;
}
void botonDespulsado() {
  detachInterrupt(digitalPinToInterrupt(SOS));
  aplicarAntirrebote = true;
}
void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    int32_t comodin;
    comodin =  readVcc(); //Tensión en milivoltios
    payload[0] = comodin >> 8;
    payload[1] = comodin;
    payload[2] = min(32, precisionFIX) | (modoAlarma << 7) | (modoLocalizacion << 5);
    //LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);

    LMIC_setTxData2(1, payload, (modoAlarma ? (9 + MACconseguidas * 6) : 3) , 0);
    Serial.println(F("Packet queued"));
  }
}
byte ultimoByte(byte lectura) {
  byte i = 8;
  while (lectura != 0) {
    lectura = lectura >> 1;
    i--;
  }
  return i;

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
    delay(10);
    return;
  } else if (direccion == 804 && direccionDelta == 212 && resto == 8) {
    //Hay que reiniciar
    EEPROM.put(804, contador);
    contadorDOWNInicial = contador;
    EEPROM.write(808, 0xFF);
    delay(10);
    return;
  }
  byte valor;
  valor=(2^(8-resto))-1;
  if(resto==0){
    //Reseteo el byte siguiente
      EEPROM.write(direccion + 4 + direccionDelta + 1, 0xFF);
      delay(10);
  }
  /*
  switch (resto) {
    case 0:
      valor = 0x00;
      //Reseteo el byte siguiente
      EEPROM.write(direccion + 4 + direccionDelta + 1, 0xFF);
      delay(10);
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
  */
  EEPROM.write(direccion + 4 + direccionDelta, valor);
  delay(10);
  return;
}
