#include "CRC8.h"
#include "CRC.h"
CRC32 crc;

#include <SparkFunLIS3DH.h>
#include <Wire.h>
LIS3DH acelerometro(I2C_MODE, 0x19);
volatile bool interrupcion_boton1 = false;
volatile bool interrupcion_boton2 = false;
volatile bool interrupcion_acelerometro = false;

uint32_t periodo_heartbeat = 60;  //segundos
//Canal 1 Luminosidad
//Canal 2 Temperatura
//Canal 3 Acelerómetro
//Canal 4 Botón1
//Canal 5 Botón2
//Canal 6 Interrupción acelerómetro
//Canal 7 LED1
//Canal 8 LED2
/*************************************

   LoRaWAN band setting:
     RAK_REGION_EU433
     RAK_REGION_CN470
     RAK_REGION_RU864
     RAK_REGION_IN865
     RAK_REGION_EU868
     RAK_REGION_US915
     RAK_REGION_AU915
     RAK_REGION_KR920
     RAK_REGION_AS923

 *************************************/
#define OTAA_BAND (RAK_REGION_EU868)
#define OTAA_DEVEUI \
  { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x87, 0x0B }
#define OTAA_APPEUI \
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define OTAA_APPKEY \
  { 0xCA, 0x9B, 0x6C, 0x2E, 0x84, 0x4F, 0xF3, 0x73, 0x1A, 0x39, 0xFE, 0x3F, 0x7F, 0xD9, 0x78, 0xEA }

/** Packet buffer for sending */
uint8_t collected_data[64] = { 0 };

void recvCallback(SERVICE_LORA_RECEIVE_T *data) {
  if (data->BufferSize > 0) {
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    Serial.print("\r\n");
    //Heartbeat
    if (data->Port == 99) {
      if (data->Buffer[0] == 0x04) {
        if (data->Buffer[1] == 0x01) {
          if (data->BufferSize == 3) {
            //Heartbeat
            uint8_t flash_value[2] = { 0 };
            flash_value[0] = (uint8_t)data->Buffer[2];
            flash_value[1] = calcCRC8((uint8_t *)(data->Buffer + 2), 1);
            api.system.flash.set(0, flash_value, 2);
            periodo_heartbeat = 0;
            periodo_heartbeat = flash_value[0];
            Serial.println(periodo_heartbeat);
            Serial.println(flash_value[1]);
          }
        }
      }
    }
    //LED1
    if (data->Port == 1) {
      if (data->Buffer[0] == 0x07) {
        if (data->Buffer[1] == 0x01) {
          if (data->BufferSize == 3) {
            digitalWrite(PA0,data->Buffer[2]);
          }
        }
      }
    }
    //LED2
    if (data->Port == 1) {
      if (data->Buffer[0] == 0x08) {
        if (data->Buffer[1] == 0x01) {
          if (data->BufferSize == 3) {
            digitalWrite(PA1,data->Buffer[2]);
          }
        }
      }
    }
  }
}

void joinCallback(int32_t status) {
  Serial.printf("Join status: %d\r\n", status);
}

void sendCallback(int32_t status) {
  if (status == 0) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Sending failed");
  }
}

void int_boton1() {
  interrupcion_boton1 = true;
}

void int_boton2() {
  interrupcion_boton2 = true;
}

void int_acelerometro() {
  interrupcion_acelerometro = true;
}

void setup() {
  Serial.begin(115200, RAK_AT_MODE);

  Serial.println("RAKwireless LoRaWan OTAA Example");
  Serial.println("------------------------------------------------------");

  if (api.lorawan.nwm.get() != 1) {
    Serial.printf("Set Node device work mode %s\r\n",
                  api.lorawan.nwm.set(1) ? "Success" : "Fail");
    api.system.reboot();
  }

  // OTAA Device EUI MSB first
  uint8_t node_device_eui[8] = OTAA_DEVEUI;
  // OTAA Application EUI MSB first
  uint8_t node_app_eui[8] = OTAA_APPEUI;
  // OTAA Application Key MSB first
  uint8_t node_app_key[16] = OTAA_APPKEY;

  if (!api.lorawan.appeui.set(node_app_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set application EUI is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appkey.set(node_app_key, 16)) {
    Serial.printf("LoRaWan OTAA - set application key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.band.set(OTAA_BAND)) {
    Serial.printf("LoRaWan OTAA - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_C)) {
    Serial.printf("LoRaWan OTAA - set device class is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_OTAA))  // Set the network join mode to OTAA
  {
    Serial.printf("LoRaWan OTAA - set network join mode is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.join())  // Join to Gateway
  {
    Serial.printf("LoRaWan OTAA - join fail! \r\n");
    return;
  }

  /** Wait for Join success */
  while (api.lorawan.njs.get() == 0) {
    Serial.print("Wait for LoRaWAN join...");
    api.lorawan.join();
    delay(10000);
  }

  if (!api.lorawan.adr.set(true)) {
    Serial.printf("LoRaWan OTAA - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(1)) {
    Serial.printf("LoRaWan OTAA - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(0)) {
    Serial.printf("LoRaWan OTAA - set unconfirm mode is incorrect! \r\n");
    return;
  }

  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get() ? "ON" : "OFF");             // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get() ? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  Serial.printf("Uplink period is %us\r\n", periodo_heartbeat);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);

  //LEDs
  pinMode(PA0, OUTPUT);  //LED1
  pinMode(PA1, OUTPUT);  //LED2

  //Interrupciones
  pinMode(PA8, INPUT_PULLUP);
  pinMode(PA15, INPUT_PULLUP);
  attachInterrupt(PA8, int_boton1, FALLING);       //Boton 1
  attachInterrupt(PA15, int_boton2, FALLING);      //Boton 2
  attachInterrupt(PB5, int_acelerometro, RISING);  //Interrupción LIS3dh (Default: push-pull output forced to GND)

  //Acelerómetro
  acelerometro.settings.accelSampleRate = 200;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
  acelerometro.settings.accelRange = 2;         //Max G force readable.  Can be: 2, 4, 8, 16
  if (acelerometro.begin() != 0) {
    Serial.println("Error iniciando el acelerómetro en 0x19.");
  } else {
    Serial.println("Acelerómetro detectado en 0x19.");
  }

  //Configuración inicial
  uint8_t flash_read[2] = { 0 };

  if (api.system.flash.get(0, flash_read, 2)) {
    Serial.print("CRC calculado = ");
    Serial.println(calcCRC8((uint8_t *)flash_read, 1), HEX);
    Serial.print("CRC leido = ");
    Serial.println(flash_read[1], HEX);
    //Serial.println(crc.calc(), HEX);
    if (calcCRC8((uint8_t *)flash_read, 1) == flash_read[1]) {
      periodo_heartbeat = flash_read[0];
    }
    Serial.print("Periodo heartbeat [s]:");
    Serial.println(periodo_heartbeat);
  }

  configIntterupts();
}

void uplink_routine() {
  /** Payload of Uplink */
  uint8_t data_len = 0;
  /*
  collected_data[data_len++] = (uint8_t) 't';
  collected_data[data_len++] = (uint8_t) 'e';
  collected_data[data_len++] = (uint8_t) 's';
  collected_data[data_len++] = (uint8_t) 't';
  */

  collected_data[data_len++] = (uint8_t)0x01;
  collected_data[data_len++] = (uint8_t)0x65;
  int ldr = analogRead(PB3);
  Serial.print("Luminosidad= ");
  Serial.println(ldr);
  collected_data[data_len++] = (uint8_t)(ldr >> 8);
  collected_data[data_len++] = (uint8_t)(ldr & 0x00FF);
  collected_data[data_len++] = (uint8_t)0x02;
  collected_data[data_len++] = (uint8_t)0x67;
  // Coeficiente beta de la NTC 3950; su R25 es 10k
  float temperatura = (1. / ((1. / (25 + 273.15)) + ((1. / 3950.) * log(((1023. / analogRead(PB4)) - 1) * 8000. / 10000.)))) - 273.15;
  Serial.print("Temperatura= ");
  Serial.println(temperatura);
  int temperatura_en_decimas = (int)(temperatura * 10);
  collected_data[data_len++] = (uint8_t)(temperatura_en_decimas >> 8);
  collected_data[data_len++] = (uint8_t)(temperatura_en_decimas & 0x00FF);
  collected_data[data_len++] = (uint8_t)0x03;
  collected_data[data_len++] = (uint8_t)0x71;
  int aceleracion_en_milesimas = (int)(acelerometro.readFloatAccelX() * 1000);
  collected_data[data_len++] = (uint8_t)(aceleracion_en_milesimas >> 8);
  collected_data[data_len++] = (uint8_t)(aceleracion_en_milesimas & 0x00FF);
  aceleracion_en_milesimas = (int)(acelerometro.readFloatAccelY() * 1000);
  collected_data[data_len++] = (uint8_t)(aceleracion_en_milesimas >> 8);
  collected_data[data_len++] = (uint8_t)(aceleracion_en_milesimas & 0x00FF);
  aceleracion_en_milesimas = (int)(acelerometro.readFloatAccelZ() * 1000);
  collected_data[data_len++] = (uint8_t)(aceleracion_en_milesimas >> 8);
  collected_data[data_len++] = (uint8_t)(aceleracion_en_milesimas & 0x00FF);
  collected_data[data_len++] = (uint8_t)0x04;
  collected_data[data_len++] = (uint8_t)0x00;
  if (interrupcion_boton1) {
    interrupcion_boton1 = false;
    collected_data[data_len++] = 1;
  } else {
    collected_data[data_len++] = !digitalRead(PA8);
  }
  collected_data[data_len++] = (uint8_t)0x05;
  collected_data[data_len++] = (uint8_t)0x00;
  if (interrupcion_boton2) {
    interrupcion_boton2 = false;
    collected_data[data_len++] = 1;
  } else {
    collected_data[data_len++] = !digitalRead(PA15);
  }
  collected_data[data_len++] = (uint8_t)0x06;
  collected_data[data_len++] = (uint8_t)0x00;
  if (interrupcion_acelerometro) {
    interrupcion_acelerometro = false;
    collected_data[data_len++] = 1;
  } else {
    collected_data[data_len++] = digitalRead(PB5);
  }
  collected_data[data_len++] = (uint8_t)0x07;
  collected_data[data_len++] = (uint8_t)0x01;
  collected_data[data_len++] = digitalRead(PA0);
  collected_data[data_len++] = (uint8_t)0x08;
  collected_data[data_len++] = (uint8_t)0x01;
  collected_data[data_len++] = digitalRead(PA1);
  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("0x%02X ", (uint8_t)collected_data[i]);
  }
  Serial.println("");

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *)&collected_data, 2, false, 1)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
  }
}

void loop() {
  static uint64_t last = 0;
  static uint64_t elapsed;

  if (((elapsed = millis() - last) > (periodo_heartbeat * 1000)) || interrupcion_boton1 || interrupcion_boton2 || interrupcion_acelerometro) {
    uplink_routine();
    last = millis();
  }
  Serial.printf("Try sleep %ums..", (periodo_heartbeat * 1000));
  api.system.sleep.all(periodo_heartbeat * 1000);
  Serial.println("Wakeup..");
}
void configIntterupts(){
  //The LIS3DH interrupts signals can behave as free-fall, 
  //wake-up, 6D and 4D orientation detection, and click detection.
  uint8_t dataToWrite = 0;

  //LIS3DH_INT1_CFG   
  //dataToWrite |= 0x80;//AOI, 0 = OR 1 = AND
  //dataToWrite |= 0x40;//6D, 0 = interrupt source, 1 = 6 direction source
  //Set these to enable individual axes of generation source (or direction)
  // -- high and low are used generically
  //dataToWrite |= 0x20;//Z high
  //dataToWrite |= 0x10;//Z low
  dataToWrite |= 0x08;//Y high
  //dataToWrite |= 0x04;//Y low
  //dataToWrite |= 0x02;//X high
  //dataToWrite |= 0x01;//X low
  acelerometro.writeRegister(LIS3DH_INT1_CFG, dataToWrite);
  
  //LIS3DH_INT1_THS   
  dataToWrite = 0;
  //Provide 7 bit value, 0x7F = 127 always equals max range by accelRange setting
  dataToWrite |= 0x10; // 1/8 range
  acelerometro.writeRegister(LIS3DH_INT1_THS, dataToWrite);
  
  //LIS3DH_INT1_DURATION  
  dataToWrite = 0;
  //minimum duration of the interrupt
  //LSB equals 1/(sample rate)
  dataToWrite |= 0x01; // 1 * 1/50 s = 20ms
  acelerometro.writeRegister(LIS3DH_INT1_DURATION, dataToWrite);
  
  //LIS3DH_CLICK_CFG   
  dataToWrite = 0;
  //Set these to enable individual axes of generation source (or direction)
  // -- set = 1 to enable
  //dataToWrite |= 0x20;//Z double-click
  dataToWrite |= 0x10;//Z click
  //dataToWrite |= 0x08;//Y double-click 
  dataToWrite |= 0x04;//Y click
  //dataToWrite |= 0x02;//X double-click
  dataToWrite |= 0x01;//X click
  acelerometro.writeRegister(LIS3DH_CLICK_CFG, dataToWrite);
  
  //LIS3DH_CLICK_SRC
  dataToWrite = 0;
  //Set these to enable click behaviors (also read to check status)
  // -- set = 1 to enable
  //dataToWrite |= 0x20;//Enable double clicks
  dataToWrite |= 0x04;//Enable single clicks
  //dataToWrite |= 0x08;//sine (0 is positive, 1 is negative)
  dataToWrite |= 0x04;//Z click detect enabled
  dataToWrite |= 0x02;//Y click detect enabled
  dataToWrite |= 0x01;//X click detect enabled
  acelerometro.writeRegister(LIS3DH_CLICK_SRC, dataToWrite);
  
  //LIS3DH_CLICK_THS   
  dataToWrite = 0;
  //This sets the threshold where the click detection process is activated.
  //Provide 7 bit value, 0x7F always equals max range by accelRange setting
  dataToWrite |= 0x0A; // ~1/16 range
  acelerometro.writeRegister(LIS3DH_CLICK_THS, dataToWrite);
  
  //LIS3DH_TIME_LIMIT  
  dataToWrite = 0;
  //Time acceleration has to fall below threshold for a valid click.
  //LSB equals 1/(sample rate)
  dataToWrite |= 0x08; // 8 * 1/50 s = 160ms
  acelerometro.writeRegister(LIS3DH_TIME_LIMIT, dataToWrite);
  
  //LIS3DH_TIME_LATENCY
  dataToWrite = 0;
  //hold-off time before allowing detection after click event
  //LSB equals 1/(sample rate)
  dataToWrite |= 0x08; // 4 * 1/50 s = 160ms
  acelerometro.writeRegister(LIS3DH_TIME_LATENCY, dataToWrite);
  
  //LIS3DH_TIME_WINDOW 
  dataToWrite = 0;
  //hold-off time before allowing detection after click event
  //LSB equals 1/(sample rate)
  dataToWrite |= 0x10; // 16 * 1/50 s = 320ms
  acelerometro.writeRegister(LIS3DH_TIME_WINDOW, dataToWrite);

  //LIS3DH_CTRL_REG5
  //Int1 latch interrupt and 4D on  int1 (preserve fifo en)
  acelerometro.readRegister(&dataToWrite, LIS3DH_CTRL_REG5);
  dataToWrite &= 0xF3; //Clear bits of interest
  //dataToWrite |= 0x08; //Latch interrupt (Cleared by reading int1_src)
  dataToWrite |= 0x00; //Do not latch interrupt 
  //dataToWrite |= 0x04; //Pipe 4D detection from 6D recognition to int1?
  acelerometro.writeRegister(LIS3DH_CTRL_REG5, dataToWrite);

  //LIS3DH_CTRL_REG3
  //Choose source for pin 1
  dataToWrite = 0;
  //dataToWrite |= 0x80; //Click detect on pin 1
  dataToWrite |= 0x40; //AOI1 event (Generator 1 interrupt on pin 1)
  dataToWrite |= 0x20; //AOI2 event ()
  //dataToWrite |= 0x10; //Data ready
  //dataToWrite |= 0x04; //FIFO watermark
  //dataToWrite |= 0x02; //FIFO overrun
  acelerometro.writeRegister(LIS3DH_CTRL_REG3, dataToWrite);
 
  //LIS3DH_CTRL_REG6
  //Choose source for pin 2 and both pin output inversion state
  dataToWrite = 0;
  dataToWrite |= 0x80; //Click int on pin 2
  //dataToWrite |= 0x40; //Generator 1 interrupt on pin 2
  //dataToWrite |= 0x10; //boot status on pin 2
  //dataToWrite |= 0x02; //invert both outputs
  acelerometro.writeRegister(LIS3DH_CTRL_REG6, dataToWrite);
  
}