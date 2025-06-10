#define LED PA8
#define POWER_ANALOG_SENSORS PA0
#define LDR PB2
#define SOIL_MOISTURE PA10
#define POWER_HTU21D PA1

#define VREFINT_CAL_ADDR ((uint16_t*)(0x1FFF75AAUL))


#include <EEPROM.h>
#define DATA_LENGTH E2END  //Es el tamaño de la emulated EEPROM

#include "SparkFunHTU21D.h"
HTU21D miHTU21D;
TwoWire Wire2(PB4, PA7);

#include <STM32LoRaWAN.h>
#include <STM32LowPower.h>
#include <STM32RTC.h>

STM32LoRaWAN modem;
/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

/* Change this value to set alarm match offset in millisecond */
static unsigned long TX_INTERVAL = 20000; /* mssegundos */
static bool uplinkACK = false;

// Declare it volatile since it's incremented inside an interrupt
volatile int alarmMatch_counter = 0;

/* callback function once the Alarm A matched */
void alarmMatch(void* data) {
  UNUSED(data);
  alarmMatch_counter++;
}

void background_work() {
  /* Toggle LED */
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  Serial.print("  Alarm Match: ");
  Serial.print(alarmMatch_counter);
  Serial.println(" times. deepSleeping now ! ");
  Serial.flush(); /* Flush before going to sleep */
}

void setup() {
  pinMode(POWER_ANALOG_SENSORS, OUTPUT);  //Habilitador de la alimentación de los sensores de humedad del suelo y LDR a través del MCP94092
  digitalWrite(POWER_ANALOG_SENSORS, LOW);
  pinMode(LED, OUTPUT);  //LED
  digitalWrite(LED, LOW);
  pinMode(POWER_HTU21D, OUTPUT);  //Alimentación de sensores I2C de Tª/H (BME280 o HTU21D)
  digitalWrite(POWER_HTU21D, LOW);

  //Leer el valor de heartbeat (minutos; entre 1 y 254) y uplinkACK (0 o 1) de las emulated EEPROM
  //Para que se considere que hay una configuración válida de la eeprom los 2 primeros bytes deben ser 4A y46 (JF)
  eeprom_buffer_fill();
  if ((eeprom_buffered_read_byte(0) != 0x4A) && (eeprom_buffered_read_byte(1) != 0x46)) {
    eeprom_buffered_write_byte(0, 0x4A);
    eeprom_buffered_write_byte(1, 0x46);
    eeprom_buffered_write_byte(2, 0x01);  //1 minuto
    eeprom_buffered_write_byte(3, 0x00);  //No ACK
    eeprom_buffer_flush();
  } else {
    TX_INTERVAL = (eeprom_buffered_read_byte(2) * 60 * 1000);
    uplinkACK = (bool)eeprom_buffered_read_byte(3);
  }



  Serial.begin(115200);
  Serial.println("Start");
  modem.begin(EU868);
  modem.setADR(false);
  modem.dataRate(DR_4);  //SF8

  // Configure join method by (un)commenting the right method
  // call, and fill in credentials in that method call.

  //bool connected = modem.joinOTAA(/* AppEui */ "0000000000000000", /* AppKey */ "00000000000000000000000000000000", /* DevEui */ "0000000000000000");
  bool connected = modem.joinABP(/* DevAddr */ "AAAAAAAA", /* NwkSKey */ "00000000000000000000000000000000", /* AppSKey */ "00000000000000000000000000000000");


  if (connected) {
    Serial.println("Joined");
  } else {
    Serial.println("Join failed");
    while (true)
      ; /* infinite loop */
  }

  /* set the calendar */
  rtc.setTime(15, 30, 58);
  rtc.setDate(4, 9, 23);

  // Configure low power
  LowPower.begin();
  LowPower.enableWakeupFrom(&rtc, alarmMatch);
}

void send_packet() {
  digitalWrite(LED, HIGH);
  //Batería
  analogReadResolution(12);

  //VREFINT=1.212
  //La información de calibración (VREFINT_CAL) está referida a 3.3V
  //3300/4095=1212/VREFINT_CAL (igualdad de la calibración)
  //VDD*analogRead(AVREF)=1212*4095
  //Combinando las 2 anteriores
  //3300*VREFINT_CAL=VDD*analogRead(AVREF)
  int bateria_en_milesimas = *(uint16_t*)VREFINT_CAL_ADDR * 3300 / analogRead(AVREF);

  analogReadResolution(10);

  digitalWrite(POWER_HTU21D, HIGH);
  delay(1000);
  miHTU21D.begin(Wire2);
  int temperaturaHTU21D;
  int humedadHTU21D;
  temperaturaHTU21D = (int)(miHTU21D.readTemperature() * 10);  //Décimas de grado
  humedadHTU21D = (int)(miHTU21D.readHumidity() * 2);          //0.5%
  digitalWrite(POWER_HTU21D, LOW);
  digitalWrite(LED, LOW);
  delay(200);
  digitalWrite(POWER_ANALOG_SENSORS, HIGH);
  int32_t humedadSuelo;
  int medidas[10], medidas_ordenadas[10], minimo, indice_minimo;
  delay(1000);
  //Tomo 10 medidas del sensor de humedad del suelo
  for (byte i = 0; i < 10; i++) {
    medidas[i] = analogRead(SOIL_MOISTURE);
    delay(300);
  }
  //Ordeno las 10 medias de menor a mayor
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
  //Calculo la mediana
  humedadSuelo = (medidas_ordenadas[4] + medidas_ordenadas[5]) / 2;

  int32_t luminosidadLDR;
  //Tomo 10 medidas del sensor LDR
  for (byte i = 0; i < 10; i++) {
    medidas[i] = analogRead(LDR);
    delay(300);
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
  luminosidadLDR = (medidas_ordenadas[4] + medidas_ordenadas[5]) / 2;

  digitalWrite(POWER_ANALOG_SENSORS, LOW);
  delay(200);



  char payload[29] = { 0 }; /* packet to be sent */
  /* prepare the Tx packet : get date and format string */
  //sprintf(payload, "%2X%c%c%c%c%c%2X", (int)(bateria_en_milesimas & 0xFFFF), (char)(temperaturaHTU21D >> 8), (char)(temperaturaHTU21D), (char)(humedadHTU21D),(char)(humedadSuelo >> 8), (char)(humedadSuelo),(int)(luminosidadLDR &0xFFFF));
  payload[0] = (byte)(bateria_en_milesimas >> 8);
  payload[1] = (byte)(bateria_en_milesimas);
  payload[2] = (byte)(temperaturaHTU21D >> 8);
  payload[3] = (byte)(temperaturaHTU21D);
  payload[4] = (byte)(humedadHTU21D);
  payload[5] = (byte)(humedadSuelo >> 8);
  payload[6] = (byte)(humedadSuelo);
  payload[7] = (byte)(luminosidadLDR >> 8);
  payload[8] = (byte)(luminosidadLDR);
  modem.setPort(10);
  modem.beginPacket();
  modem.write(payload, 9);
  Serial.println(payload);
  if (modem.endPacket(uplinkACK) == (int)strlen(payload)) {
    Serial.println("Sent packet");
  } else {
    Serial.println("Failed to send packet");
  }

  if (modem.available()) {
    //El downlink del periodo de heartbeat tendrá el formato canal (04), tipo de valor (01=digital output), valor (0 a 255)
    //El modo de ACK debe entrar por el canal 05 con tipo 01: FF (ACK) y cualquier otra cosa noACK
    //Sólo voy a atender los downlink que entren por el puerto 99
    if (modem.getDownlinkPort() == 99) {
      digitalWrite(LED, LOW);
      delay(250);
      digitalWrite(LED, HIGH);
      delay(250);
      digitalWrite(LED, LOW);
      delay(250);
      digitalWrite(LED, HIGH);
      delay(250);
      uint8_t primer_byte = modem.read();
      uint8_t segundo_byte = modem.read();
      if (primer_byte == 0x04 && segundo_byte == 0x01) {
        //Puede ser un herarbeat
        uint8_t potencial_heartbeat = modem.read();
        if (potencial_heartbeat > 0) {
          eeprom_buffer_fill();
          eeprom_buffered_write_byte(2, potencial_heartbeat);
          eeprom_buffer_flush();
          TX_INTERVAL = (potencial_heartbeat * 60 * 1000);
        }
      } else if (primer_byte == 0x05 && segundo_byte == 0x01) {
        uint8_t potencial_ACK = modem.read();
        eeprom_buffer_fill();
        eeprom_buffered_write_byte(3, (potencial_ACK == 0xFF ? 0x01 : 0x00));
        eeprom_buffer_flush();
        uplinkACK = (potencial_ACK == 0xFF);
      }
    }


    while (modem.available()) {
      uint8_t b = modem.read();
      //Vacío el buffer por si quedase algo
    }
  }
}

void loop() {
  //Cuidado con alterar el orden de estas instrucciones
  //podría provocar que el uC no entre en sueño profundo
  send_packet();

  background_work();

  LowPower.deepSleep(TX_INTERVAL);
}
