#include <Wire.h>
#include <SPI.h>
#include "LowPower.h"
#include <lmic.h>
#include <hal/hal.h>
#include <EEPROM.h>

#include <CayenneLPP.h>
CayenneLPP lpp(22);//4 bateria + 3 hall + 3 entradaDigitalLED + 3 salidaDigitalLED


boolean envioEnCurso = false;
boolean puertaAbierta;
//interruppido se usa para saber si se ha iniciado el envío de un mensaje
//y es necesario abandonar los bucles de sueño
//para entrar en el bucle de espera a que se termine el envío iniciado
//pues si entrase el Arduino en sueño durante el envío del mensaje podría fallar el envío
boolean interrumpido = false;
//El periodo de heartBeat va a estar almacenado en la posición 37
//Mapa memoria EEPROM
//Posicion 0: 0sFF indica que se usen las credenciales originales / 0x00 indica que se utilicen las credenciales recibidas por downlink
//Posiciones 1 hasta máximo 36: credenciales
//Posición 37: Periodo heartbeat en minutos
byte minutosHeartbeat;

byte DEVEUI[8] ;
byte APPEUI[8] ;
byte APPKEY[16];
boolean nuevasCredenciales[3] = {false, false, false};
static const PROGMEM u1_t DEVEUI_INICIAL[8] = {  }; //LSB
static const PROGMEM u1_t APPEUI_INICIAL[8] = {  }; //LSB
static const PROGMEM u1_t APPKEY_INICIAL[16] = {  }; //MSB
boolean resetearTrasSiguienteUplink = false;

//Si el byte 0 de la eeprom contiene 0xFF querrá decir que el cliente aún no ha tomado posesión del nodo
//Por el contrario, si contiene un 0x00, habrá que usar los datos leídos de la EEPROM

byte poseido;

//Primero el nombre del dispositivo y segundo el secreto
//el nombre usa 8 dígitos para el nombre del proveedor y 8 dígitos para el nombre del dispositivo: ej. 0000000100000001
//el secreto siempre deberá empezar por una letra para que el cliente pueda diferenciarlo fácilmente del nombre del dispositivo
byte datoMorse[2][16] = {{0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10110000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b10110000},
  {0b01010000, 0b10001110, 0b10001010, 0b01101100, 0b00110000, 0b10011010, 0b01100100, 0b10011110, 0b01011000, 0b10010000, 0b01101000, 0b10010110, 0b01000000, 0b01001000, 0b01100000, 0b10010010}
};
/*
  Estructura para codificar datoMorse
  //Los 3 bits más significativos indican la longitud de la letra
  //A continuación un 1 indica ., y un 0 indica -
  static const struct {
  const char letter;
  byte code;
  } MorseMap[] =
  {
  { 'A', 0b01010000 },
  { 'B', 0b10001110 },
  { 'C', 0b10001010 },
  { 'D', 0b01101100 },
  { 'E', 0b00110000 },
  { 'F', 0b10011010 },
  { 'G', 0b01100100 },
  { 'H', 0b10011110 },
  { 'I', 0b01011000 },
  { 'J', 0b10010000 },
  { 'K', 0b01101000 },
  { 'L', 0b10010110 },
  { 'M', 0b01000000 },
  { 'N', 0b01001000 },
  { 'O', 0b01100000 },
  { 'P', 0b10010010 },
  { 'Q', 0b10000100 },
  { 'R', 0b01110100 },
  { 'S', 0b01111100 },
  { 'T', 0b00101111 },
  { 'U', 0b01111000 },
  { 'V', 0b10011100 },
  { 'W', 0b01110000 },
  { 'X', 0b10001100 },
  { 'Y', 0b10001000 },
  { 'Z', 0b10000110 },

  { '1', 0b10110000 },
  { '2', 0b10111000 },
  { '3', 0b10111100 },
  { '4', 0b10111110 },
  { '5', 0b10111111 },
  { '6', 0b10101111 },
  { '7', 0b10100111 },
  { '8', 0b10100011 },
  { '9', 0b10100001 },
  { '0', 0b10100000 },

  { '&', 0b10110111 },
  };
*/


byte hex2byte(byte hex) {
  if (hex <= 57) {
    //es un número 0-9
    return hex - 48;
  } else {
    //es una letra A-F
    return hex - 55;
  }
}
//Para resetear el Arduino tras una toma de posesión
void(* resetFunc) (void) = 0; //declare reset function @ address 0

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.

void os_getArtEui (u1_t* buf) {
  if (poseido == 0x00) {
    EEPROM.get(9, APPEUI);
    memcpy(buf, APPEUI, 8);
  } else {

    memcpy_P(buf, APPEUI_INICIAL, 8);
  }

}

// This should also be in little endian format, see above.
void os_getDevEui (u1_t* buf) {
  if (poseido == 0x00) {
    EEPROM.get(1, DEVEUI);
    memcpy(buf, DEVEUI, 8);
  } else {
    memcpy_P(buf, DEVEUI_INICIAL, 8);
  }

}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.

void os_getDevKey (u1_t* buf) {
  if (poseido == 0x00) {
    EEPROM.get(17, APPKEY);
    memcpy(buf, APPKEY, 16);
  } else {

    memcpy_P(buf, APPKEY_INICIAL, 16);
  }

}


//static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
//const unsigned TX_INTERVAL = 60;

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,
  .dio = {2, 7, LMIC_UNUSED_PIN},
};

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
      //Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      //Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      //Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      //Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      //Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      //Serial.println(F("EV_JOINED"));
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
      //Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      //Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (resetearTrasSiguienteUplink) {
        resetFunc();  //call reset
      }
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        //Serial.println(F("Received "));
        //Serial.println(LMIC.dataLen);
        //Serial.println(F(" bytes of payload"));
        //El downlink tendrá el formato canal (06), tipo de valor (00=digital), valor (0 o 100=0x64), FF (ACK)
        //Sólo voy a atender los downlink que entren por el puerto 99
        //Serial.println(LMIC.dataLen);
        if (LMIC.frame[LMIC.dataBeg - 1] == 99) {
          if (LMIC.frame[LMIC.dataBeg + 0] == 0x06 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para el LED por el canal 6
            nuevasCredenciales[0] = false;
            nuevasCredenciales[1] = false;
            nuevasCredenciales[2] = false;
            if (LMIC.frame[LMIC.dataBeg + 2] == 100) {
              digitalWrite(4, HIGH);
            } else {
              digitalWrite(4, LOW);
            }
          } else if (LMIC.frame[LMIC.dataBeg + 0] == 0x07 && LMIC.frame[LMIC.dataBeg + 1] == 0x01) {
            //downlink para establecer el periodo del heartbeat
            nuevasCredenciales[0] = false;
            nuevasCredenciales[1] = false;
            nuevasCredenciales[2] = false;
            EEPROM.write(37,LMIC.frame[LMIC.dataBeg + 2]);
            minutosHeartbeat=LMIC.frame[LMIC.dataBeg + 2];
          } else if (LMIC.frame[LMIC.dataBeg + 0] == 0x46 && LMIC.frame[LMIC.dataBeg + 1] == 0x46) {
            //downlink de configuración de nuevas credenciales
            //Hay que tener en cuenta que los downlink pueden llegar desordenados
            if (LMIC.frame[LMIC.dataBeg + 4] == 0x4F) {
              //OTAA
              if (LMIC.frame[LMIC.dataBeg + 2] == 0x30 && LMIC.frame[LMIC.dataBeg + 3] == 0x31) {
                //DeviceEUI
                nuevasCredenciales[0] = true;
                for (byte i = 0; i < 8; i++) {
                  DEVEUI[i] = (hex2byte(LMIC.frame[LMIC.dataBeg + 5 + (2 * i)]) << 4) + hex2byte(LMIC.frame[LMIC.dataBeg + 6 + (2 * i)]);
                  EEPROM.write(8 - i, DEVEUI[i]);
                  Serial.print(hex2byte(LMIC.frame[LMIC.dataBeg + 5 + (2 * i)]) << 4);
                  Serial.print("  ");
                  Serial.print(hex2byte(LMIC.frame[LMIC.dataBeg + 6 + (2 * i)]));
                  Serial.print("  ");
                  Serial.println(DEVEUI[i]);
                  delay(100);
                }
              } else if (LMIC.frame[LMIC.dataBeg + 2] == 0x30 && LMIC.frame[LMIC.dataBeg + 3] == 0x32) {
                //APPEUI
                nuevasCredenciales[1] = true;
                for (byte i = 0; i < 8; i++) {
                  APPEUI[i] = (hex2byte(LMIC.frame[LMIC.dataBeg + 5 + (2 * i)]) << 4) + hex2byte(LMIC.frame[LMIC.dataBeg + 6 + (2 * i)]);
                  EEPROM.write(16 - i, APPEUI[i]);
                  delay(100);
                }
              } else if (LMIC.frame[LMIC.dataBeg + 2] == 0x30 && LMIC.frame[LMIC.dataBeg + 3] == 0x33) {
                //APPKEY
                nuevasCredenciales[2] = true;
                for (byte i = 0; i < 16; i++) {
                  APPKEY[i] = (hex2byte(LMIC.frame[LMIC.dataBeg + 5 + (2 * i)]) << 4) + hex2byte(LMIC.frame[LMIC.dataBeg + 6 + (2 * i)]);
                  EEPROM.write(17 + i, APPKEY[i]);
                  delay(100);
                }
              }

            } else {
              //ABP
              //Este tipo de nodo no admite ABP
            }
            if (nuevasCredenciales[0] && nuevasCredenciales[1] && nuevasCredenciales[2]) {
              //Tenemos las nuevas credenciales completas
              EEPROM.write(0, 0x00);
              resetearTrasSiguienteUplink = true;
            }
          }
        }
      }
      // Schedule next transmission
      //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
      envioEnCurso = false;
      break;
    case EV_LOST_TSYNC:
      //Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      //Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      //Serial.println(F("EV_LINK_ALIVE"));
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
    lpp.reset();
    lpp.addAnalogInput(1, readVcc() / 1000.F);
    lpp.addDigitalInput(2, puertaAbierta);
    lpp.addDigitalInput(3, digitalRead(4));
    lpp.addDigitalOutput(6, digitalRead(4));
    LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  Serial.begin(9600);

  pinMode(3, INPUT_PULLUP);//SENSOR HALL
  pinMode(8, INPUT_PULLUP);//BOTON
  pinMode(4, OUTPUT);//LED

  Serial.println(F("Starting"));
  if ( digitalRead(8) == LOW) {
    delay(500);
    if ( digitalRead(8) == LOW) {
      //El botón está pulsado durante el arranque
      //Empiezo un ciclo de 5 segundos con el LED cambiando de estado cada 0.5 seg
      //Si se libera el botón antes de terminar paso al modo MORSE
      //Si no, paso al modo RESET
      bool pulsacionLarga = true;
      for (int i = 0; i < 5; i++) {
        digitalWrite(4, HIGH);
        delay(500);
        if (digitalRead(8) == HIGH) {
          pulsacionLarga = false;
          break;
        }
        digitalWrite(4, LOW);
        delay(500);
        if (digitalRead(8) == HIGH) {
          pulsacionLarga = false;
          break;
        }
      }
      if (pulsacionLarga) {
        //Reset
        EEPROM.write(0, 0xFF);
      } else {
        //Morse
        //Se sale con otra pulsación
        digitalWrite(4, LOW);
        delay(3000); //Para dar tiempo a que el cliente prepare el móvil y, de paso, debouncing
        byte dit = 80; //120ms es un dit para 10 palabras por minuto
        bool interrumpido = false;
        int i = 1;
        while (!interrumpido) {
          i = (i + 1) % 2;
          for (int j = 0; j < 16; j++) {//caracteres
            if (interrumpido) {
              break;
            }
            byte longitud = datoMorse[i][j] >> 5;
            for (int k = 0; k < longitud; k++) {//caracter
              if (digitalRead(8) == LOW) {
                interrumpido = true;
                break;
              }
              if (((datoMorse[i][j] & (0b00000001 << (4 - k))) >> (4 - k)) == 0b0000001) {
                //es un punto: un dit del punto y un dit del espacio intra-caracter
                digitalWrite(4, HIGH);
                delay(dit);
                digitalWrite(4, LOW);
              } else {
                //es una raya: tres dit del punto y un dit del espacio intra-caracter
                digitalWrite(4, HIGH);
                delay(3 * dit);
                digitalWrite(4, LOW);
              }
              delay(dit); //1 dit de pausa intra-caracter
            }
            delay(2 * dit); //3 dits de pausa inter-caracter
          }
          if (digitalRead(8) == LOW) {//Lo pongo aquí tambien porque es una pausa bastante larga
            interrumpido = true;
            break;
          }
          delay(4 * dit); //7 dits de pausa inter-palabras
        }
      }
    }
  }
  EEPROM.get(0, poseido);
  EEPROM.get(37, minutosHeartbeat);
  Serial.println(minutosHeartbeat);
  if(minutosHeartbeat>60){
    minutosHeartbeat=1;
  }

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100); //Para mejorar la recepción de los downlinks largos
  //Conecto en el pin 3 un interruptor a GND normalmente abierto
  pinMode(3, INPUT);
  delay(100);
  // Start job
  puertaAbierta = digitalRead(3);
  enviarMensaje();
  //do_send(&sendjob);
}

void loop() {
  //Esperamos a que concluya el envío
  while (envioEnCurso) {
    os_runloop_once();
  }
  delay(100);
  interrumpido = false;
  if (puertaAbierta == digitalRead(3)) {
    //La puerta no ha cambiado de estado
    attachInterrupt(digitalPinToInterrupt(3), enviarMensaje, (puertaAbierta == 1 ? FALLING : RISING));
    for (byte minutos = 0; minutos < minutosHeartbeat; minutos++) {
      if (interrumpido == true) {
        break;
      }
      //Aproximadamente 7 loops son un minuto
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
      puertaAbierta = !puertaAbierta;
    }
  } else {
    //La puerta ha cambiado de estado durante el envío del mensaje de estado anterior
    puertaAbierta = !puertaAbierta;
    enviarMensaje();
  }
}
