#include "CRC8.h"
#include "CRC.h"
CRC32 crc;

uint32_t periodo_heartbeat = 60;                  //segundos
volatile int ultima_interrupcion_solicitada = 0;  //0-No solicitada; 1-Puerta abierta; 2-Puerta cerrada
volatile bool interrupcion_puerta_abierta = false;
volatile bool interrupcion_puerta_cerrada = false;
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
  { 0x70, 0xB3, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define OTAA_APPEUI \
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define OTAA_APPKEY \
  { 0xC2, 0x00, 0x0A, 0x11, 0x00, 0x00, 0xB2, 0xA2, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00 }

/** Packet buffer for sending */
uint8_t collected_data[64] = { 0 };

void recvCallback(SERVICE_LORA_RECEIVE_T *data) {
  if (data->BufferSize > 0) {
    /*
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    Serial.print("\r\n");
    */
    //Heartbeat
    if (data->Port == 99) {
      if (data->Buffer[0] == 0x04) {
        if (data->Buffer[1] == 0x01) {
          if (data->BufferSize == 4) {  //Mete un FF al final
            //Heartbeat
            uint8_t flash_value[2] = { 0 };
            flash_value[0] = (uint8_t)data->Buffer[2];
            flash_value[1] = calcCRC8((uint8_t *)(data->Buffer + 2), 1);
            api.system.flash.set(0, flash_value, 2);
            periodo_heartbeat = flash_value[0];
          }
        }
      }
    }
    //Confirmación
    if (data->Port == 99) {
      if (data->Buffer[0] == 0x05) {
        if (data->Buffer[1] == 0x01) {
          if (data->BufferSize == 4) {  //Mete un FF al final
            uint8_t flash_value[2] = { 0 };
            flash_value[0] = (uint8_t)data->Buffer[2];
            flash_value[1] = calcCRC8((uint8_t *)(data->Buffer + 2), 1);
            api.system.flash.set(2, flash_value, 2);
            if (flash_value[0] == 0xFE) {
              //Confirmados
              api.lorawan.cfm.set(1);
            } else {
              //No confirmados
              api.lorawan.cfm.set(0);
            }
          }
        }
      }
    }
    //Reset
    if (data->Port == 99) {
      if (data->Buffer[0] == 0x06) {
        if (data->Buffer[1] == 0x01) {
          if (data->BufferSize == 4) {  //Mete un FF al final
            if ((uint8_t)data->Buffer[2] == 0x01) {
              //Confirmados
              api.system.reboot();
            }
          }
        }
      }
    }
  }
}

void joinCallback(int32_t status) {
  //Serial.printf("Join status: %d\r\n", status);
}

/*************************************
 * enum type for LoRa Event
    RAK_LORAMAC_STATUS_OK = 0,
    RAK_LORAMAC_STATUS_ERROR,
    RAK_LORAMAC_STATUS_TX_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_TIMEOUT,
    RAK_LORAMAC_STATUS_RX2_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_ERROR,
    RAK_LORAMAC_STATUS_RX2_ERROR,
    RAK_LORAMAC_STATUS_JOIN_FAIL,
    RAK_LORAMAC_STATUS_DOWNLINK_REPEATED,
    RAK_LORAMAC_STATUS_TX_DR_PAYLOAD_SIZE_ERROR,
    RAK_LORAMAC_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS,
    RAK_LORAMAC_STATUS_ADDRESS_FAIL,
    RAK_LORAMAC_STATUS_MIC_FAIL,
    RAK_LORAMAC_STATUS_MULTICAST_FAIL,
    RAK_LORAMAC_STATUS_BEACON_LOCKED,
    RAK_LORAMAC_STATUS_BEACON_LOST,
    RAK_LORAMAC_STATUS_BEACON_NOT_FOUND,
 *************************************/

void sendCallback(int32_t status) {
  if (status == RAK_LORAMAC_STATUS_OK) {
    //Serial.println("Successfully sent");
    if (ultima_interrupcion_solicitada == 1) {
      ultima_interrupcion_solicitada = 0;
      if (digitalRead(PA10)) {
        //Si la puerta sigue abierta
        attachInterrupt(PA10, puerta_cerrada, FALLING);  //Puerta cerrada
      } else {
        //La puerta se ha cerrado
        interrupcion_puerta_cerrada = true;
        ultima_interrupcion_solicitada = 2;
      }
    } else if (ultima_interrupcion_solicitada == 2) {
      ultima_interrupcion_solicitada = 0;
      if (!digitalRead(PA10)) {
        //Si la puerta sigue cerrada
        attachInterrupt(PA10, puerta_abierta, RISING);  //Puerta abierta
      } else {
        //La puerta se ha cerrado
        interrupcion_puerta_abierta = true;
        ultima_interrupcion_solicitada = 1;
      }
    }
  } else {
    //Serial.println("Sending failed");
    if (ultima_interrupcion_solicitada == 1) {
      interrupcion_puerta_abierta = true;
    } else if (ultima_interrupcion_solicitada == 2) {
      interrupcion_puerta_cerrada = true;
    }
  }
}
void puerta_abierta() {
  detachInterrupt(digitalPinToInterrupt(PA10));
  interrupcion_puerta_abierta = true;
  ultima_interrupcion_solicitada = 1;
}
void puerta_cerrada() {
  detachInterrupt(digitalPinToInterrupt(PA10));
  interrupcion_puerta_cerrada = true;
  ultima_interrupcion_solicitada = 2;
}
void setup() {
  //Serial.begin(115200, RAK_AT_MODE);
  delay(2000);
  //LED
  pinMode(PA8, OUTPUT);
  //Habilitador de la NTC
  pinMode(PA0, OUTPUT);
  digitalWrite(PA0, LOW);
  //Interrupciones
  pinMode(PA10, INPUT);  //HALL
  delay(300);
  if (digitalRead(PA10)) {
    attachInterrupt(PA10, puerta_cerrada, FALLING);  //Puerta cerrada
  } else {
    attachInterrupt(PA10, puerta_abierta, RISING);  //Puerta cerrada
  }


  //Serial.println("RAKwireless LoRaWan OTAA Example");
  //Serial.println("------------------------------------------------------");

  if (api.lorawan.nwm.get() != 1) {
    //Serial.printf("Set Node device work mode %s\r\n", api.lorawan.nwm.set() ? "Success" : "Fail");
    api.system.reboot();
  }

  // OTAA Device EUI MSB first
  uint8_t node_device_eui[8] = OTAA_DEVEUI;
  // OTAA Application EUI MSB first
  uint8_t node_app_eui[8] = OTAA_APPEUI;
  // OTAA Application Key MSB first
  uint8_t node_app_key[16] = OTAA_APPKEY;

  if (!api.lorawan.appeui.set(node_app_eui, 8)) {
    //Serial.printf("LoRaWan OTAA - set application EUI is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appkey.set(node_app_key, 16)) {
    //Serial.printf("LoRaWan OTAA - set application key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    //Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.band.set(OTAA_BAND)) {
    //Serial.printf("LoRaWan OTAA - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    //Serial.printf("LoRaWan OTAA - set device class is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_OTAA))  // Set the network join mode to OTAA
  {
    //Serial.printf("LoRaWan OTAA - set network join mode is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.join())  // Join to Gateway
  {
    //Serial.printf("LoRaWan OTAA - join fail! \r\n");
    return;
  }

  /** Wait for Join success */
  while (api.lorawan.njs.get() == 0) {
    //Serial.print("Wait for LoRaWAN join...");
    api.lorawan.join();
    uint64_t last = millis();
    while(millis()-last<10000 && api.lorawan.njs.get() == 0){
      digitalWrite(PA8,HIGH);
      delay(250);
      digitalWrite(PA8,LOW);
      delay(250);
    }   
  }

  if (!api.lorawan.adr.set(true)) {
    //Serial.printf("LoRaWan OTAA - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(1)) {
    //Serial.printf("LoRaWan OTAA - set retry times is incorrect! \r\n");
    return;
  }
  /*
  if (!api.lorawan.cfm.set(0)) {
    //Serial.printf("LoRaWan OTAA - set unconfirm mode is incorrect! \r\n");
    return;
  }
  */
  /** Check LoRaWan Status*/
  /*
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get() ? "ON" : "OFF");             // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get() ? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  */
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  /*
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  Serial.printf("Uplink period is %ums\r\n", periodo_heartbeat);
  Serial.println("");
  */
  //Configuración inicial
  uint8_t flash_read[2] = { 0 };
  if (api.system.flash.get(0, flash_read, 2)) {
    if (calcCRC8((uint8_t *)flash_read, 1) == flash_read[1]) {
      periodo_heartbeat = flash_read[0];
    }
  }
  flash_read[2] = { 0 };
  if (api.system.flash.get(2, flash_read, 2)) {
    if (calcCRC8((uint8_t *)flash_read, 1) == flash_read[1]) {
      if (flash_read[0] == 0xFE) {
        //Confirmados
        api.lorawan.cfm.set(1);
      } else {
        //No confirmados
        api.lorawan.cfm.set(0);
      }
    }
  } else {
    api.lorawan.cfm.set(0);
  }
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);
}

bool uplink_routine() {
  digitalWrite(PA8, HIGH);  //LED
  digitalWrite(PA0, HIGH);  //Alimentación NTC
  /** Payload of Uplink */
  uint8_t data_len = 0;
  //Batería
  int16_t adc_value;
  UDRV_ADC_RESOLUTION resolution = udrv_adc_get_resolution();
  udrv_adc_set_resolution(UDRV_ADC_RESOLUTION_12BIT);
  udrv_adc_read(UDRV_ADC_CHANNEL_VREFINT, &adc_value);
  float bat_lvl = (float)adc_value;
  bat_lvl = bat_lvl / 4095;
  bat_lvl = 1.27 / bat_lvl;
  bat_lvl = bat_lvl * 0.974 - 0.065;  //calibrate
  int bateria_en_centesimas = (int)(bat_lvl * 100);
  collected_data[data_len++] = (uint8_t)0x01;
  collected_data[data_len++] = (uint8_t)0x02;
  collected_data[data_len++] = (uint8_t)(bateria_en_centesimas >> 8);
  collected_data[data_len++] = (uint8_t)(bateria_en_centesimas & 0x00FF);
  udrv_adc_set_resolution(resolution);

  //Hall
  collected_data[data_len++] = (uint8_t)0x02;
  collected_data[data_len++] = (uint8_t)0x00;
  //El DRV5032FB pone la salida en HIGH en ausencia del imán y en LOW en su presencia
  if (interrupcion_puerta_abierta) {
    collected_data[data_len++] = 1;
    interrupcion_puerta_abierta = false;
  } else if (interrupcion_puerta_cerrada) {
    collected_data[data_len++] = 0;
    interrupcion_puerta_cerrada = false;
  } else {
    collected_data[data_len++] = digitalRead(PA10);
  }

  //Temperatura
  collected_data[data_len++] = (uint8_t)0x03;
  collected_data[data_len++] = (uint8_t)0x67;
  // Coeficiente beta de la NTC 3950; su R25 es 10k
  float temperatura = (1. / ((1. / (25 + 273.15)) + ((1. / 3950.) * log(((1023. / analogRead(PB2)) - 1) * 8000. / 10000.)))) - 273.15;
  int temperatura_en_decimas = (int)(temperatura * 10);
  collected_data[data_len++] = (uint8_t)(temperatura_en_decimas >> 8);
  collected_data[data_len++] = (uint8_t)(temperatura_en_decimas & 0x00FF);
  digitalWrite(PA0, LOW);
  /*
  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("0x%02X ", collected_data[i]);
  }
  Serial.println("");
  */
  digitalWrite(PA8, LOW);  //LED

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *)&collected_data, 1)) {
    //Serial.println("Sending is requested");
    return true;
  } else {
    //Serial.println("Sending failed");
    return false;
  }
}

void loop() {
  static uint64_t last = 0;
  static uint64_t elapsed;

  if (((elapsed = millis() - last) > (periodo_heartbeat * 1000)) || interrupcion_puerta_abierta || interrupcion_puerta_cerrada) {
    uplink_routine();
    last = millis();
  }
  //Serial.printf("Try sleep %ums..", (periodo_heartbeat * 1000));
  api.system.sleep.all(periodo_heartbeat * 1000);
  //Serial.println("Wakeup..");
}
