//En el último byte de la EEPROM [1023] guardo los minutos de heartbeat
//En A0 está la NTC de exterior -20..50 => R= 17721 = 18K
//En A6 y A7 están las de interior -30..20 => R= 44275 = 47K



#define R_25 10000
#define TEMPERATURA_NOMINAL 25
#define COEFICIENTE_BETA 3950

byte NTCPin[] = {0, 6, 7};
unsigned int R_PULL_DOWN[] = {22000, 44800, 44800}; 


#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <EEPROM.h>
#include "LowPower.h"
#include <CayenneLPP.h>
CayenneLPP lpp(22);

boolean envioEnCurso = false;
boolean boton;
boolean interrumpido = false;
byte minutosHeartbeat;

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

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = { FILLMEIN };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = { FILLMEIN };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { FILLMEIN };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

static osjob_t sendjob;

// Schedule TX every this many minutes
const unsigned TX_INTERVAL = 30;

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_UNUSED_PIN,
  .dio = {4, 5, LMIC_UNUSED_PIN},
};

void printHex2(unsigned v) {
  v &= 0xff;
  if (v < 16)
    Serial.print('0');
  Serial.print(v, HEX);
}

void reset() { asm volatile ("jmp 0"); }

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
void enviarMensaje() {
  envioEnCurso = true;
  interrumpido = true;
  os_setCallback (&sendjob, do_send);
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
      {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
        Serial.print("netid: ");
        Serial.println(netid, DEC);
        Serial.print("devaddr: ");
        Serial.println(devaddr, HEX);
        Serial.print("AppSKey: ");
        for (size_t i = 0; i < sizeof(artKey); ++i) {
          if (i != 0)
            Serial.print("-");
          printHex2(artKey[i]);
        }
        Serial.println("");
        Serial.print("NwkSKey: ");
        for (size_t i = 0; i < sizeof(nwkKey); ++i) {
          if (i != 0)
            Serial.print("-");
          printHex2(nwkKey[i]);
        }
        Serial.println();
      }
      // Disable link check validation (automatically enabled
      // during join, but because slow data rates change max TX
      // size, we don't use it in this example.
      LMIC_setLinkCheckMode(0);
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
        //Serial.println(F("Received "));
        //Serial.println(LMIC.dataLen);
        //Serial.println(F(" bytes of payload"));
        //El downlink del periodo de heartbeat tendrá el formato canal (07), tipo de valor (01=digital output), valor (0 a 255), FF (ACK)
        //Sólo voy a atender los downlink que entren por el puerto 99
        if (LMIC.frame[LMIC.dataBeg - 1] == 99) {
          if (LMIC.frame[LMIC.dataBeg + 0] == 0x07 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {

            byte periodoHeartbeat = LMIC.frame[LMIC.dataBeg + 2];
            if (periodoHeartbeat > 0) {
              EEPROM.write(1023, periodoHeartbeat);
              minutosHeartbeat = periodoHeartbeat;
            }
          }else if (LMIC.frame[LMIC.dataBeg + 0] == 0x03 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para actuar el LED
            
            if (LMIC.frame[LMIC.dataBeg + 2]==0x64) {
              digitalWrite(3,HIGH);

            } else if (LMIC.frame[LMIC.dataBeg + 2]==0x00) {
              digitalWrite(3,LOW);
            }
          }else if (LMIC.frame[LMIC.dataBeg + 0] == 0x09 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para resetear
            
            if (LMIC.frame[LMIC.dataBeg + 2]==1) {
              reset();

            } 
          }
        }
      }
      // Schedule next transmission
      //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
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

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    digitalWrite(6, HIGH);
    lpp.reset();
    lpp.addAnalogInput(1, readVcc() / 1000.F);
    lpp.addDigitalInput(2, boton);
    lpp.addDigitalOutput(3, digitalRead(3));

    float ADCvalue;
    float R_NTC;
    //Repetir con las 3 NTC
    for (byte i = 0; i < 3; i++) {
      ADCvalue = analogRead(NTCPin[i]);
      Serial.println(analogRead(0));
      //Convertir el ADC en R_NTC
      R_NTC = R_PULL_DOWN[i] * ((1023.0 / ADCvalue) - 1.0);
      Serial.println(R_NTC);
      float beta;
      beta = R_NTC / R_25; // (R/Ro);
      beta = log(beta); // ln(R/Ro);
      beta /= COEFICIENTE_BETA; // 1/B * ln(R/Ro);
      beta += (1.0 / (TEMPERATURA_NOMINAL + 273.15)); // + (1/To);
      Serial.println(beta);
      beta = 1.0 / beta; // Inverso
      beta -= 273.15; // Convertir a ºC
      lpp.addTemperature(4 + i, beta);
    }
    digitalWrite(6, LOW);

    LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  pinMode(2, INPUT_PULLUP);//BOTON
  pinMode(3, OUTPUT);//LED
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  Serial.begin(9600);
  Serial.println(F("Starting"));
  minutosHeartbeat = EEPROM.read(1023);
  if (minutosHeartbeat == 0 || minutosHeartbeat == 255) {
    minutosHeartbeat = 30;
  }

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  //Conecto en el pin 2 un interruptor a GND normalmente abierto

  delay(100);
  // Start job
  boton = digitalRead(2);
  enviarMensaje();
}

void loop() {
  //Esperamos a que concluya el envío
  while (envioEnCurso) {
    os_runloop_once();
  }
  delay(100);
  interrumpido = false;
  if (boton == digitalRead(2)) {
    //El botón no ha cambiado de estado
    attachInterrupt(digitalPinToInterrupt(2), enviarMensaje, (boton == 1 ? FALLING : RISING));

    //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
    for (byte minutos = 0; minutos < minutosHeartbeat; minutos++) {
      if (interrumpido == true) {
        break;
        boton = !boton;
      }
      for (byte contador = 0; contador < 7; contador++) {
        if (interrumpido == true) {
          break;
        }
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      }
    }
    if (!interrumpido) {
      enviarMensaje();
    } else {
      boton = !boton;
    }
  } else {
    //El boton ha cambiado de estado durante el envío del mensaje de estado anterior
    boton = !boton;
    enviarMensaje();
  }
}
