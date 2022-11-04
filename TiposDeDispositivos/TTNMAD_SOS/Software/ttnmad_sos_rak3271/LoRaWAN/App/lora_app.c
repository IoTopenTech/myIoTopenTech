/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    lora_app.c
 * @author  MCD Application Team
 * @brief   Application of the LRWAN Middleware
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "stm32_lpm.h"
#include "adc_if.h"
#include "CayenneLpp.h"
#include "sys_sensors.h"
#include "flash_if.h"

/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief LoRa State Machine states
  */
typedef enum TxEventType_e
{
  /**
    * @brief Appdata Transmission issue based on timer every TxDutyCycleTime
    */
  TX_ON_TIMER,
  /**
    * @brief Appdata Transmission external event plugged on OnSendEvent( )
    */
  TX_ON_EVENT
  /* USER CODE BEGIN TxEventType_t */

  /* USER CODE END TxEventType_t */
} TxEventType_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * LEDs period value of the timer in ms
  */
#define LED_PERIOD_TIME 500

/**
  * Join switch period value of the timer in ms
  */
#define JOIN_TIME 2000

/*---------------------------------------------------------------------------*/
/*                             LoRaWAN NVM configuration                     */
/*---------------------------------------------------------------------------*/
/**
  * @brief LoRaWAN NVM Flash address
  * @note last 2 sector of a 128kBytes device
  */
#define LORAWAN_NVM_BASE_ADDRESS                    ((uint32_t)0x0803F000UL)

/* USER CODE BEGIN PD */
#define CONFIGURACION_NVM_BASE_ADDRESS              ((uint32_t)0x0803E000UL)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  LoRa End Node send request
  */
static void SendTxData(void);

/**
  * @brief  TX timer callback function
  * @param  context ptr of timer context
  */
static void OnTxTimerEvent(void *context);

/**
  * @brief  join event callback function
  * @param  joinParams status of join
  */
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);

/**
  * @brief callback when LoRaWAN application has sent a frame
  * @brief  tx event callback function
  * @param  params status of last Tx
  */
static void OnTxData(LmHandlerTxParams_t *params);

/**
  * @brief callback when LoRaWAN application has received a frame
  * @param appData data received in the last Rx
  * @param params status of last Rx
  */
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

/**
  * @brief callback when LoRaWAN Beacon status is updated
  * @param params status of Last Beacon
  */
static void OnBeaconStatusChange(LmHandlerBeaconParams_t *params);

/**
  * @brief callback when LoRaWAN application Class is changed
  * @param deviceClass new class
  */
static void OnClassChange(DeviceClass_t deviceClass);

/**
  * @brief  LoRa store context in Non Volatile Memory
  */
static void StoreContext(void);

/**
  * @brief  stop current LoRa execution to switch into non default Activation mode
  */
static void StopJoin(void);

/**
  * @brief  Join switch timer callback function
  * @param  context ptr of Join switch context
  */
static void OnStopJoinTimerEvent(void *context);

/**
  * @brief  Notifies the upper layer that the NVM context has changed
  * @param  state Indicates if we are storing (true) or restoring (false) the NVM context
  */
static void OnNvmDataChange(LmHandlerNvmContextStates_t state);

/**
  * @brief  Store the NVM Data context to the Flash
  * @param  nvm ptr on nvm structure
  * @param  nvm_size number of data bytes which were stored
  */
static void OnStoreContextRequest(void *nvm, uint32_t nvm_size);

/**
  * @brief  Restore the NVM Data context from the Flash
  * @param  nvm ptr on nvm structure
  * @param  nvm_size number of data bytes which were restored
  */
static void OnRestoreContextRequest(void *nvm, uint32_t nvm_size);

/**
  * Will be called each time a Radio IRQ is handled by the MAC layer
  *
  */
static void OnMacProcessNotify(void);

/**
  * @brief Change the periodicity of the uplink frames
  * @param periodicity uplink frames period in ms
  * @note Compliance test protocol callbacks
  */
static void OnTxPeriodicityChanged(uint32_t periodicity);

/**
  * @brief Change the confirmation control of the uplink frames
  * @param isTxConfirmed Indicates if the uplink requires an acknowledgement
  * @note Compliance test protocol callbacks
  */
static void OnTxFrameCtrlChanged(LmHandlerMsgTypes_t isTxConfirmed);

/**
  * @brief Change the periodicity of the ping slot frames
  * @param pingSlotPeriodicity ping slot frames period in ms
  * @note Compliance test protocol callbacks
  */
static void OnPingSlotPeriodicityChanged(uint8_t pingSlotPeriodicity);

/**
  * @brief Will be called to reset the system
  * @note Compliance test protocol callbacks
  */
static void OnSystemReset(void);

/* USER CODE BEGIN PFP */
static void GuardarConfiguracion(void);
/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief LoRaWAN default activation type
  */
static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

/**
  * @brief LoRaWAN force rejoin even if the NVM context is restored
  */
static bool ForceRejoin = LORAWAN_FORCE_REJOIN_AT_BOOT;

/**
  * @brief LoRaWAN handler Callbacks
  */
static LmHandlerCallbacks_t LmHandlerCallbacks =
{
  .GetBatteryLevel =              GetBatteryLevel,
  .GetTemperature =               GetTemperatureLevel,
  .GetUniqueId =                  GetUniqueId,
  .GetDevAddr =                   GetDevAddr,
  .OnRestoreContextRequest =      OnRestoreContextRequest,
  .OnStoreContextRequest =        OnStoreContextRequest,
  .OnMacProcess =                 OnMacProcessNotify,
  .OnNvmDataChange =              OnNvmDataChange,
  .OnJoinRequest =                OnJoinRequest,
  .OnTxData =                     OnTxData,
  .OnRxData =                     OnRxData,
  .OnBeaconStatusChange =         OnBeaconStatusChange,
  .OnClassChange =                OnClassChange,
  .OnTxPeriodicityChanged =       OnTxPeriodicityChanged,
  .OnTxFrameCtrlChanged =         OnTxFrameCtrlChanged,
  .OnPingSlotPeriodicityChanged = OnPingSlotPeriodicityChanged,
  .OnSystemReset =                OnSystemReset,
};

/**
  * @brief LoRaWAN handler parameters
  */
static LmHandlerParams_t LmHandlerParams =
{
  .ActiveRegion =             ACTIVE_REGION,
  .DefaultClass =             LORAWAN_DEFAULT_CLASS,
  .AdrEnable =                LORAWAN_ADR_STATE,
  .IsTxConfirmed =            LORAWAN_DEFAULT_CONFIRMED_MSG_STATE,
  .TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
  .PingSlotPeriodicity =      LORAWAN_DEFAULT_PING_SLOT_PERIODICITY,
  .RxBCTimeout =              LORAWAN_DEFAULT_CLASS_B_C_RESP_TIMEOUT
};

/**
  * @brief Type of Event to generate application Tx
  */
static TxEventType_t EventType = TX_ON_EVENT;

/**
  * @brief Timer to handle the application Tx
  */
static UTIL_TIMER_Object_t TxTimer;

/**
  * @brief Tx Timer period
  */
static UTIL_TIMER_Time_t TxPeriodicity = APP_TX_DUTYCYCLE;

/**
  * @brief Join Timer period
  */
static UTIL_TIMER_Object_t StopJoinTimer;

/* USER CODE BEGIN PV */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };

char gps_buffer[1];
char latitud[11];
char longitud[12];
char latitud_tentativa[11];
char longitud_tentativa[12];
char gps_fix_quality;
char gps_number_of_satellites[3];
char gps_number_of_satellites_tentativo[3];
char gps_hdop[6];
char gps_hdop_tentativo[6];
char gps_fix_conseguido = 0;
static char modo_de_funcionamiento = 0; //0=Reposo, 1=Alarma
static char cambiar_a_modo_alarma = 0; //Permite hacer un uplink inmediato sin tener que esperar los tiempos del GPS/ESP
static uint32_t heartbeat_reposo = 240 * 60 * 1000; //Expresado en milisegundos
static uint32_t heartbeat_alarma = 1 * 60 * 1000;
static char modo_geolocalizacion = 2; //0=Sólo GPS, 1=Sólo Wi-Fi, 2=GPS+Wi-Fi
static int tiempo_maximo_GPS = 2 * 60; //Expresado en segundos
static uint16_t contador_caracter_esp = 0;
char esp_buffer[1];
static int tiempo_maximo_ESP = 1 * 60; //Expresado en segundos
char esp_mac_conseguido = 0;
volatile char esp_byte_recibido = 0;
volatile char gps_byte_recibido = 0;
char mac1[13];
char mac2[13];
static uint16_t contador_caracter = 0;
static uint16_t contador_comas = 0;
static uint16_t contador_latitud = 0;
static uint16_t contador_longitud = 0;
static uint16_t contador_gps_number_of_satellites = 0;
static uint16_t contador_gps_hdop = 0;
uint16_t estado_esp = 0;
/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
/* USER CODE BEGIN EF */

/* USER CODE END EF */

void LoRaWAN_Init(void)
{
  /* USER CODE BEGIN LoRaWAN_Init_LV */

  /* USER CODE END LoRaWAN_Init_LV */

  /* USER CODE BEGIN LoRaWAN_Init_1 */

	uint64_t *RDAddr = (uint64_t*) CONFIGURACION_NVM_BASE_ADDRESS;
	uint64_t RData = *RDAddr;

	for (int i = 0; i < (uint8_t) RData; i++) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(500);
	}

	if (RData < 10) {
		heartbeat_alarma = RData * 60 * 1000;
	}

	UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_GuardarConfiguracion), UTIL_SEQ_RFU,
			GuardarConfiguracion);
  /* USER CODE END LoRaWAN_Init_1 */

  UTIL_TIMER_Create(&StopJoinTimer, JOIN_TIME, UTIL_TIMER_ONESHOT, OnStopJoinTimerEvent, NULL);

  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);

  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), UTIL_SEQ_RFU, SendTxData);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaStoreContextEvent), UTIL_SEQ_RFU, StoreContext);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaStopJoinEvent), UTIL_SEQ_RFU, StopJoin);

  /* Init Info table used by LmHandler*/
  LoraInfo_Init();

  /* Init the Lora Stack*/
  LmHandlerInit(&LmHandlerCallbacks, APP_VERSION);

  LmHandlerConfigure(&LmHandlerParams);

  /* USER CODE BEGIN LoRaWAN_Init_2 */
  /* USER CODE END LoRaWAN_Init_2 */

  LmHandlerJoin(ActivationType, ForceRejoin);

  if (EventType == TX_ON_TIMER)
  {
    /* send every time timer elapses */
    UTIL_TIMER_Create(&TxTimer, TxPeriodicity, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);
    UTIL_TIMER_Start(&TxTimer);
  }
  else
  {
    /* USER CODE BEGIN LoRaWAN_Init_3 */
		UTIL_TIMER_Create(&TxTimer, heartbeat_reposo, UTIL_TIMER_ONESHOT,
				OnTxTimerEvent, NULL);
		UTIL_TIMER_Start(&TxTimer);
    /* USER CODE END LoRaWAN_Init_3 */
  }

  /* USER CODE BEGIN LoRaWAN_Init_Last */
	LmHandlerSetDutyCycleEnable(true);

  /* USER CODE END LoRaWAN_Init_Last */
}

/* USER CODE BEGIN PB_Callbacks */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case BOTON_Pin:
		if (modo_de_funcionamiento == 0) {
			//Pasamos al modo alarma
			modo_de_funcionamiento = 1;
			cambiar_a_modo_alarma = 1;
			//Encendemos el LED
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			//Hacemos un envío inmediato
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent),
					CFG_SEQ_Prio_0);
		} else {
			//Pasamos al modo reposo
			modo_de_funcionamiento = 0;
			//Apagamos el LED
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			//Nos aseguramos de que están apagados el GPS y el ESP
			HAL_GPIO_WritePin(EN_GPS_GPIO_Port, EN_GPS_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(EN_ESP_GPIO_Port, EN_ESP_Pin, GPIO_PIN_RESET);

			//Cambiamos el perido de envíos
			UTIL_TIMER_Stop(&TxTimer);
			UTIL_TIMER_SetPeriod(&TxTimer, heartbeat_reposo);
			UTIL_TIMER_Start(&TxTimer);
		}
	}
}
#if 0 /* User should remove the #if 0 statement and adapt the below code according with his needs*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUT1_Pin:
      /* Note: when "EventType == TX_ON_TIMER" this GPIO is not initialized */
      if (EventType == TX_ON_EVENT)
      {
        UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
      }
      break;
    case  BUT2_Pin:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaStopJoinEvent), CFG_SEQ_Prio_0);
      break;
    case  BUT3_Pin:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaStoreContextEvent), CFG_SEQ_Prio_0);
      break;
    default:
      break;
  }
}
#endif

/* USER CODE END PB_Callbacks */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */
static void GuardarConfiguracion(void) {
	uint64_t configuration_data[3];
	configuration_data[0] = (uint64_t) heartbeat_alarma / 60000;
	configuration_data[1] = (uint64_t) heartbeat_reposo / 60000;
	configuration_data[2] = (uint64_t) modo_geolocalizacion;
	if (HAL_FLASH_Unlock() == HAL_OK) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_Delay(1000);
		if (FLASH_IF_EraseByPages(PAGE(CONFIGURACION_NVM_BASE_ADDRESS), 1, 0U)
				== FLASH_OK) {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			HAL_Delay(1000);
			if (FLASH_IF_Write(
			CONFIGURACION_NVM_BASE_ADDRESS, (uint8_t*) configuration_data, 24,
			NULL) == FLASH_OK) {
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				HAL_Delay(1000);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				HAL_Delay(1000);
			}
		}
		HAL_FLASH_Lock();
	}
}
/* USER CODE END PrFD */

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_1 */
	uint8_t RxPort = 0;

	if (params != NULL) {
		if (params->IsMcpsIndication) {
			if (appData != NULL) {
				RxPort = appData->Port;
				if (appData->Buffer != NULL) {
					if (RxPort == 99) {
						switch (appData->Buffer[0]) {
						case 0x04:
							//Heartbeat alarma
							if (appData->Buffer[1] == 0x01) {
								if (heartbeat_alarma
										!= appData->Buffer[2] * 60 * 1000) {
									heartbeat_alarma = appData->Buffer[2] * 60
											* 1000;
									if (modo_de_funcionamiento == 1) {
										UTIL_TIMER_Stop(&TxTimer);
										UTIL_TIMER_SetPeriod(&TxTimer,
												heartbeat_alarma);
										UTIL_TIMER_Start(&TxTimer);
									}
									UTIL_SEQ_SetTask(
											(1
													<< CFG_SEQ_Task_GuardarConfiguracion),
											CFG_SEQ_Prio_0);
								}
							}
							break;
						case 0x05:
							//Heartbeat reposo
							if (appData->Buffer[1] == 0x01) {
								if (heartbeat_reposo
										!= appData->Buffer[2] * 60 * 1000) {
									heartbeat_reposo = appData->Buffer[2] * 60
											* 1000;
									if (modo_de_funcionamiento == 0) {
										UTIL_TIMER_Stop(&TxTimer);
										UTIL_TIMER_SetPeriod(&TxTimer,
												heartbeat_reposo);
										UTIL_TIMER_Start(&TxTimer);
									}
									UTIL_SEQ_SetTask(
											(1
													<< CFG_SEQ_Task_GuardarConfiguracion),
											CFG_SEQ_Prio_0);
								}
							}
							break;
						case 0x06:
							//Modo alarma
							if (appData->Buffer[1] == 0x01) {
								if (modo_de_funcionamiento == 0
										&& appData->Buffer[2] == 0x01) {
									//Pasamos al modo alarma
									cambiar_a_modo_alarma = 1;
									modo_de_funcionamiento = 1;
									//Encendemos el LED
									HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,
											GPIO_PIN_SET);
									//Hacemos un envío inmediato
									UTIL_SEQ_SetTask(
											(1
													<< CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent),
											CFG_SEQ_Prio_0);
								} else if (modo_de_funcionamiento == 1
										&& appData->Buffer[2] == 0x00) {
									//Pasamos al modo reposo
									modo_de_funcionamiento = 0;
									//Apagamos el LED
									HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,
											GPIO_PIN_RESET);
									//Nos aseguramos de que están apagados el GPS y el ESP
									HAL_GPIO_WritePin(EN_GPS_GPIO_Port,
									EN_GPS_Pin, GPIO_PIN_RESET);
									HAL_GPIO_WritePin(EN_ESP_GPIO_Port,
									EN_ESP_Pin, GPIO_PIN_RESET);

									//Cambiamos el perido de envíos
									UTIL_TIMER_Stop(&TxTimer);
									UTIL_TIMER_SetPeriod(&TxTimer,
											heartbeat_reposo);
									UTIL_TIMER_Start(&TxTimer);
								}
							}
							break;
						case 0x03:
							if (appData->Buffer[1] == 0x01
									&& appData->Buffer[2] <= 2) {

								if (modo_geolocalizacion
										!= appData->Buffer[2]) {
									modo_geolocalizacion = appData->Buffer[2];

									UTIL_SEQ_SetTask(
											(1
													<< CFG_SEQ_Task_GuardarConfiguracion),
											CFG_SEQ_Prio_0);
								}
							}

							break;
						case 0x07:
							if (appData->Buffer[1] == 0x01
									&& appData->Buffer[2] == 1) {
								HAL_NVIC_SystemReset();
							}
							break;

						}
					}
				}
			}
		}

		/*
		 if (params->RxSlot < RX_SLOT_NONE) {
		 APP_LOG(TS_OFF, VLEVEL_H,
		 "###### D/L FRAME:%04d | PORT:%d | DR:%d | SLOT:%s | RSSI:%d | SNR:%d\r\n",
		 params->DownlinkCounter, RxPort, params->Datarate,
		 slotStrings[params->RxSlot], params->Rssi, params->Snr);
		 }
		 */
	}
  /* USER CODE END OnRxData_1 */
}

static void SendTxData(void)
{
  /* USER CODE BEGIN SendTxData_1 */
	uint16_t batteryLevel = SYS_GetBatteryLevel();
	AppData.Buffer[0] = (uint8_t) ((batteryLevel >> 8) & 0xFF);
	AppData.Buffer[1] = (uint8_t) (batteryLevel & 0xFF);
	//En el tercer byte codifico el modo de funcionamiento, el modo de geolocalización
	//y el hdop copado a 32 (5 bits)
	AppData.Buffer[2] = (modo_de_funcionamiento << 7)
			| (modo_geolocalizacion << 5);
	AppData.BufferSize = 3;
	if (modo_de_funcionamiento == 1 && cambiar_a_modo_alarma==0) {
		for (int j = 3; j < 9; j++) {
			AppData.Buffer[j] = 0;
		}
		SysTime_t tiempoActual;
		SysTime_t tiempoParcial;
		//Estamos en modo de alarma
		//GPS?

		AppData.BufferSize = 9;		//Luego se amplia con las MAC si es preciso

		if (modo_geolocalizacion == 0 || modo_geolocalizacion == 2) {
			gps_fix_conseguido = 0;
			gps_fix_quality = '\0';
			latitud[0] = '\0';
			longitud[0] = '\0';
			latitud_tentativa[0] = '\0';
			longitud_tentativa[0] = '\0';
			gps_hdop[0] = '\0';
			gps_hdop_tentativo[0] = '\0';
			gps_number_of_satellites[0] = '\0';
			gps_number_of_satellites_tentativo[0] = '\0';
			//Habilitamos la alimentación del GPS
			APP_LOG(TS_ON, VLEVEL_L, "GPS ENCENDIDO\r\n");
			HAL_GPIO_WritePin(EN_GPS_GPIO_Port, EN_GPS_Pin, GPIO_PIN_SET);
			tiempoParcial = SysTimeGet();
			//Espero un par de segundos para estabilizar la alimentación
			/*
			 HAL_UART_Receive_IT(&huart2, (uint8_t*) gps_buffer, 1);

			 while (SysTimeGet().Seconds - tiempoParcial.Seconds < 2) {

			 if (gps_byte_recibido == 1) {
			 APP_LOG(TS_ON, VLEVEL_L, "%c\r\n",gps_buffer[0]);
			 gps_byte_recibido = 0;
			 HAL_UART_Receive_IT(&huart2, (uint8_t*) gps_buffer, 1);
			 }

			 }
			 */
			//Si no reinicio la uart no es capaz de recibir los datos del GPS
			//en modo bajo consumo????!!!
			//Podría estar relacionado con esto
			//https://stackoverflow.com/questions/56378415/uart-doesnt-work-after-waking-up-from-stop-mode-in-stm32f4
			MX_USART2_UART_Init();
			tiempoActual = SysTimeGet();
			gps_byte_recibido = 0;
			HAL_UART_Receive_IT(&huart2, (uint8_t*) gps_buffer, 1);
			while ((gps_fix_conseguido == 0)
					&& (SysTimeGet().Seconds - tiempoActual.Seconds
							< tiempo_maximo_GPS)) {
				//$GPGGA,131242.000,4022.4644,N,00343.8720,W,1,7,2.49,572.8,M,51.7,M,,*45
				//Simplemente esperamos a que el GPS obtenga un FIX
				if (gps_byte_recibido == 1) {
					//APP_LOG(TS_ON, VLEVEL_L, "%c\r\n",gps_buffer[0]);
					//HAL_UART_Transmit(&huart1, gps_buffer, 1, 100);
					if (contador_caracter == 0 && gps_buffer[0] == '$') {
						contador_caracter = 1;
					} else if (contador_caracter == 1 && gps_buffer[0] == 'G') {
						contador_caracter = 2;
					} else if (contador_caracter == 2 && gps_buffer[0] == 'P') {
						contador_caracter = 3;
					} else if (contador_caracter == 3 && gps_buffer[0] == 'G') {
						contador_caracter = 4;
					} else if (contador_caracter == 4 && gps_buffer[0] == 'G') {
						contador_caracter = 5;
					} else if (contador_caracter == 5 && gps_buffer[0] == 'A') {
						contador_caracter = 6;
					} else if (contador_caracter == 6 && gps_buffer[0] == ',') {
						contador_caracter = 0;
						contador_comas = 1;
					} else if (contador_comas == 1) {
						if (gps_buffer[0] == ',') {
							contador_comas = 2;
						}
					} else if (contador_comas == 2) {
						if (gps_buffer[0] == ',') {
							contador_comas = 3;
						} else {
							latitud[contador_latitud++] = gps_buffer[0];
						}
					} else if (contador_comas == 3) {
						if (gps_buffer[0] == ',') {
							contador_comas = 4;
						} else {
							latitud[contador_latitud++] = gps_buffer[0]; //N o S
							latitud[10] = '\0';
						}
					} else if (contador_comas == 4) {
						if (gps_buffer[0] == ',') {
							contador_comas = 5;
						} else {
							longitud[contador_longitud++] = gps_buffer[0];
						}
					} else if (contador_comas == 5) {
						if (gps_buffer[0] == ',') {
							contador_comas = 6;
						} else {
							longitud[contador_longitud++] = gps_buffer[0]; //E o W
							longitud[11] = '\0';
						}
					} else if (contador_comas == 6) {
						if (gps_buffer[0] == ',') {
							contador_comas = 7;
						} else {
							gps_fix_quality = gps_buffer[0];
						}
					} else if (contador_comas == 7) {
						if (gps_buffer[0] == ',') {
							gps_number_of_satellites[2] = '\0';
							contador_comas = 8;
						} else {
							gps_number_of_satellites[contador_gps_number_of_satellites++] =
									gps_buffer[0];
						}
					} else if (contador_comas == 8) {
						if (gps_buffer[0] == ',') {
							gps_hdop[contador_gps_hdop] = '\0';
							if (((int) gps_fix_quality - (int) '0') > 0) {
								if ((atof(gps_hdop) < 2.0)
										&& (atof(gps_hdop) >= 1.0)) {
									gps_fix_conseguido = 1;
								} else {
									if (atof(gps_hdop_tentativo) == 0
											|| (atof(gps_hdop_tentativo)
													> atof(gps_hdop))) {
										strncpy(latitud_tentativa, latitud, 11);
										strncpy(longitud_tentativa, longitud,
												12);
										strncpy(
												gps_number_of_satellites_tentativo,
												gps_number_of_satellites, 3);
										strncpy(gps_hdop_tentativo, gps_hdop,
												6);
									}
								}
							}
							contador_caracter = 0;
							contador_comas = 0;
							contador_latitud = 0;
							contador_longitud = 0;
							contador_gps_number_of_satellites = 0;
							contador_gps_hdop = 0;
						} else {
							gps_hdop[contador_gps_hdop++] = gps_buffer[0];
						}
					} else {
						contador_caracter = 0;
						contador_comas = 0;
						contador_latitud = 0;
						contador_longitud = 0;
						contador_gps_number_of_satellites = 0;
						contador_gps_hdop = 0;
					}
					gps_byte_recibido = 0;
					HAL_UART_Receive_IT(&huart2, (uint8_t*) gps_buffer, 1);
				}
			}
		}
		//Deshabilitamos la alimentación del GPS

		APP_LOG(TS_ON, VLEVEL_L, "GPS APAGADO\r\n");
		HAL_GPIO_WritePin(EN_GPS_GPIO_Port, EN_GPS_Pin, GPIO_PIN_RESET);
		if (gps_fix_conseguido == 1 || atof(gps_hdop_tentativo) > 1) {
			long lat = 0;
			long lon = 0;
			if (gps_fix_conseguido == 0) {
				strncpy(latitud, latitud_tentativa, 11);
				strncpy(longitud, longitud_tentativa, 12);
				strncpy(gps_hdop, gps_hdop_tentativo, 6);
				strncpy(gps_number_of_satellites,
						gps_number_of_satellites_tentativo, 3);
			}

			//Tengo que convertir la latitud y longitud a grados
			//y multiplicarlas por 40000 quedándome sólo con la parte
			//entera... porque así lo tengo en myIoT
			float comodin;
			comodin = atof(&latitud[2]) / 60.0;
			latitud[2] = '\0';
			lat = (long) ((atof(latitud) + comodin) * 40000.0);
			if (latitud[9] == 'S') {

				AppData.Buffer[3] = (lat >> 16) | 0x80;
			} else {
				AppData.Buffer[3] = (lat >> 16);
			}
			AppData.Buffer[4] = lat >> 8;
			AppData.Buffer[5] = lat;

			//HAL_UART_Transmit(&huart1, latitud, 11, 100);
			//char str[32];
			//int size_len = sprintf (str, "Lat : %d\n", lat);
			//HAL_UART_Transmit (&huart1, (uint8_t *)str, size_len, HAL_MAX_DELAY);
			comodin = atof(&longitud[3]) / 60.0;
			longitud[3] = '\0';

			lon = (long) ((atof(longitud) + comodin) * 40000.0);

			if (longitud[10] == 'W') {
				AppData.Buffer[6] = (lon >> 16) | 0x80;
			} else {
				AppData.Buffer[6] = (lon >> 16);
			}

			AppData.Buffer[7] = lon >> 8;
			AppData.Buffer[8] = lon;
			comodin = atof(gps_hdop) * 100.0;
			AppData.Buffer[2] = AppData.Buffer[2]
					| (31 < (char) (atoi(gps_hdop)) ?
							31 : (char) (atoi(gps_hdop)));

			//sprintf(&AppData.Buffer[3], "%03lx", lat);
			//sprintf(&AppData.Buffer[6], "%03lx", lon);

			APP_LOG(TS_ON, VLEVEL_L,
					"Fix: %d, Lat: %d, Lon: %d, Fix quality: %c, Satellites: %s, HDOP: %s - %d\r\n",
					gps_fix_conseguido, lat, lon, gps_fix_quality,
					gps_number_of_satellites, gps_hdop, (long ) atof(gps_hdop));
		}
		//Wi-Fi?

		if (modo_geolocalizacion == 1 || modo_geolocalizacion == 2) {
			APP_LOG(TS_ON, VLEVEL_L, "ESP ENCENDIDO\r\n");
			mac1[0] = '\0';
			mac2[0] = '\0';
			esp_mac_conseguido = 0;
			tiempoParcial = SysTimeGet();
			//Espero un par de segundos para estabilizar la alimentación
			while (SysTimeGet().Seconds - tiempoParcial.Seconds < 2) {
			}
			HAL_GPIO_WritePin(EN_ESP_GPIO_Port, EN_ESP_Pin, GPIO_PIN_SET);

			estado_esp = 0;
			esp_byte_recibido = 0;
			HAL_UART_Receive_IT(&huart1, (uint8_t*) esp_buffer, 1);
			tiempoActual = SysTimeGet();
			while (esp_mac_conseguido == 0
					&& (SysTimeGet().Seconds - tiempoActual.Seconds
							< tiempo_maximo_ESP && (esp_mac_conseguido == 0))) {
				//Simplemente esperamos a que el ESP obtenga las MACS
				switch (estado_esp) {
				case 0:
					//Espero a que n 2 segundos sin recibir nada (vaciado del bufer)
					if (esp_byte_recibido == 1) {
						esp_byte_recibido = 0;
						HAL_UART_Receive_IT(&huart1, (uint8_t*) esp_buffer, 1);
						tiempoParcial = SysTimeGet();
					} else if (SysTimeGet().Seconds - tiempoParcial.Seconds
							> 2) {
						estado_esp = 1;
						contador_caracter_esp = 0;
						HAL_UART_Transmit_IT(&huart1, "AT\r\n", 4);
						tiempoParcial = SysTimeGet();
					}
					break;
				case 1:
					if (esp_byte_recibido == 1) {
						tiempoParcial = SysTimeGet();
						if (contador_caracter_esp == 0
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 1
								&& esp_buffer[0] == 'T') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 2
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 3
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 4
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 5
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 6
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 7
								&& esp_buffer[0] == 'O') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 8
								&& esp_buffer[0] == 'K') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 9
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 10
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp = 0;
							estado_esp = 2;
							//Espero 1 segundo
							tiempoParcial = SysTimeGet();
							while (SysTimeGet().Seconds - tiempoParcial.Seconds
									< 1) {
							}
							HAL_UART_Transmit_IT(&huart1, "AT+CWMODE=1\r\n",
									13);
						} else {

							estado_esp = 0;
						}
						esp_byte_recibido = 0;
						HAL_UART_Receive_IT(&huart1, (uint8_t*) esp_buffer, 1);
					} else if (SysTimeGet().Seconds - tiempoParcial.Seconds
							> 1) {
						tiempoParcial = SysTimeGet();
						estado_esp = 0;
					}
					break;
				case 2:
					if (esp_byte_recibido == 1) {
						tiempoParcial = SysTimeGet();
						if (contador_caracter_esp == 0
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 1
								&& esp_buffer[0] == 'T') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 2
								&& esp_buffer[0] == '+') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 3
								&& esp_buffer[0] == 'C') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 4
								&& esp_buffer[0] == 'W') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 5
								&& esp_buffer[0] == 'M') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 6
								&& esp_buffer[0] == 'O') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 7
								&& esp_buffer[0] == 'D') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 8
								&& esp_buffer[0] == 'E') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 9
								&& esp_buffer[0] == '=') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 10
								&& esp_buffer[0] == '1') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 11
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 12
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 13
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 14
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 15
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 16
								&& esp_buffer[0] == 'O') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 17
								&& esp_buffer[0] == 'K') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 18
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 19
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp = 0;
							estado_esp = 3;
							//Espero 1 segundo
							tiempoParcial = SysTimeGet();
							while (SysTimeGet().Seconds - tiempoParcial.Seconds
									< 1) {
							}
							HAL_UART_Transmit_IT(&huart1, "AT+CWLAPOPT=1,8\r\n",
									17);
						} else {
							estado_esp = 0;
						}
						esp_byte_recibido = 0;
						HAL_UART_Receive_IT(&huart1, (uint8_t*) esp_buffer, 1);
					} else if (SysTimeGet().Seconds - tiempoParcial.Seconds
							> 1) {
						tiempoParcial = SysTimeGet();
						estado_esp = 0;
					}
					break;
				case 3:
					if (esp_byte_recibido == 1) {
						tiempoParcial = SysTimeGet();
						if (contador_caracter_esp == 0
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 1
								&& esp_buffer[0] == 'T') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 2
								&& esp_buffer[0] == '+') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 3
								&& esp_buffer[0] == 'C') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 4
								&& esp_buffer[0] == 'W') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 5
								&& esp_buffer[0] == 'L') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 6
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 7
								&& esp_buffer[0] == 'P') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 8
								&& esp_buffer[0] == 'O') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 9
								&& esp_buffer[0] == 'P') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 10
								&& esp_buffer[0] == 'T') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 11
								&& esp_buffer[0] == '=') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 12
								&& esp_buffer[0] == '1') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 13
								&& esp_buffer[0] == ',') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 14
								&& esp_buffer[0] == '8') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 15
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 16
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 17
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 18
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 19
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 20
								&& esp_buffer[0] == 'O') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 21
								&& esp_buffer[0] == 'K') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 22
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 23
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp = 0;
							estado_esp = 4;
							//Espero 2 segundos
							tiempoParcial = SysTimeGet();
							while (SysTimeGet().Seconds - tiempoParcial.Seconds
									< 2) {
							}
							HAL_UART_Transmit_IT(&huart1, "AT+CWLAP\r\n", 10);
						} else {
							estado_esp = 0;
						}
						esp_byte_recibido = 0;
						HAL_UART_Receive_IT(&huart1, (uint8_t*) esp_buffer, 1);
					} else if (SysTimeGet().Seconds - tiempoParcial.Seconds
							> 2) {
						tiempoParcial = SysTimeGet();
						estado_esp = 0;
					}
					break;
				case 4:
					if (esp_byte_recibido == 1) {
						tiempoParcial = SysTimeGet();
						if (contador_caracter_esp == 0
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 1
								&& esp_buffer[0] == 'T') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 2
								&& esp_buffer[0] == '+') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 3
								&& esp_buffer[0] == 'C') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 4
								&& esp_buffer[0] == 'W') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 5
								&& esp_buffer[0] == 'L') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 6
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 7
								&& esp_buffer[0] == 'P') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 8
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 9
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 10
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 11
								&& esp_buffer[0] == '+') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 12
								&& esp_buffer[0] == 'C') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 13
								&& esp_buffer[0] == 'W') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 14
								&& esp_buffer[0] == 'L') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 15
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 16
								&& esp_buffer[0] == 'P') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 17
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 18
								&& esp_buffer[0] == '(') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 19
								&& esp_buffer[0] == '"') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 20) {
							mac1[0] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 21) {
							mac1[1] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 22
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 23) {
							mac1[2] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 24) {
							mac1[3] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 25
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 26) {
							mac1[4] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 27) {
							mac1[5] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 28
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 29) {
							mac1[6] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 30) {
							mac1[7] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 31
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 32) {
							mac1[8] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 33) {
							mac1[9] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 34
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 35) {
							mac1[10] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 36) {
							AppData.BufferSize = 15;
							mac1[11] = esp_buffer[0];
							contador_caracter_esp++;
							for (int i = 0; i < 6; i++) {
								mac1[12 - (2 * i)] = '\0';
								AppData.Buffer[14 - i] = (uint8_t) strtol(
										&mac1[10 - (2 * i)], NULL, 16);
							}

						} else if (contador_caracter_esp == 37
								&& esp_buffer[0] == '"') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 38
								&& esp_buffer[0] == ')') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 39
								&& esp_buffer[0] == 0x0D) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 40
								&& esp_buffer[0] == 0x0A) {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 41
								&& esp_buffer[0] == '+') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 42
								&& esp_buffer[0] == 'C') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 43
								&& esp_buffer[0] == 'W') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 44
								&& esp_buffer[0] == 'L') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 45
								&& esp_buffer[0] == 'A') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 46
								&& esp_buffer[0] == 'P') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 47
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 48
								&& esp_buffer[0] == '(') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 49
								&& esp_buffer[0] == '"') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 50) {
							mac2[0] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 51) {
							mac2[1] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 52
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 53) {
							mac2[2] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 54) {
							mac2[3] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 55
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 56) {
							mac2[4] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 57) {
							mac2[5] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 58
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 59) {
							mac2[6] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 60) {
							mac2[7] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 61
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 62) {
							mac2[8] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 63) {
							mac2[9] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 64
								&& esp_buffer[0] == ':') {
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 65) {
							mac2[10] = esp_buffer[0];
							contador_caracter_esp++;
						} else if (contador_caracter_esp == 66) {
							AppData.BufferSize = 21;
							mac2[11] = esp_buffer[0];
							contador_caracter_esp++;

							for (int i = 0; i < 6; i++) {
								mac2[12 - (2 * i)] = '\0';
								AppData.Buffer[20 - i] = (uint8_t) strtol(
										&mac2[10 - (2 * i)], NULL, 16);
							}

							esp_mac_conseguido = 1;
							break;
						} else {
							estado_esp = 0;
						}
						esp_byte_recibido = 0;
						HAL_UART_Receive_IT(&huart1, (uint8_t*) esp_buffer, 1);
					} else if (SysTimeGet().Seconds - tiempoParcial.Seconds
							> 2) {
						tiempoParcial = SysTimeGet();
						estado_esp = 0;
					}
					break;
				}
			}
			//APP_LOG(TS_ON, VLEVEL_L, "ESP APAGADO\r\n");
			HAL_GPIO_WritePin(EN_ESP_GPIO_Port, EN_ESP_Pin, GPIO_PIN_RESET);
		}

	}

	LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;
	UTIL_TIMER_Time_t nextTxIn = 0;
	AppData.Port = LORAWAN_USER_APP_PORT;
	status = LmHandlerSend(&AppData, LmHandlerParams.IsTxConfirmed, true);
	if (LORAMAC_HANDLER_SUCCESS == status) {
		APP_LOG(TS_ON, VLEVEL_L, "SEND REQUEST\r\n");
	} else if (LORAMAC_HANDLER_DUTYCYCLE_RESTRICTED == status) {
		nextTxIn = LmHandlerGetDutyCycleWaitTime();
		if (nextTxIn > 0) {
			APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n",
					(nextTxIn / 1000));
		}
	}

	UTIL_TIMER_Stop(&TxTimer);
	if (modo_de_funcionamiento == 1 || cambiar_a_modo_alarma == 1) {
		APP_LOG(TS_ON, VLEVEL_L, "MODO ALARMA ACTIVADO\r\n");
		UTIL_TIMER_SetPeriod(&TxTimer, MAX(heartbeat_alarma, nextTxIn));
		cambiar_a_modo_alarma = 0;
		modo_de_funcionamiento = 1;
	} else {
		APP_LOG(TS_ON, VLEVEL_L, "MODO REPOSO ACTIVADO\r\n");
		UTIL_TIMER_SetPeriod(&TxTimer, MAX(heartbeat_reposo, nextTxIn));
	}
	UTIL_TIMER_Start(&TxTimer);

  /* USER CODE END SendTxData_1 */
}

static void OnTxTimerEvent(void *context)
{
  /* USER CODE BEGIN OnTxTimerEvent_1 */

  /* USER CODE END OnTxTimerEvent_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

  /*Wait for next tx slot*/
  UTIL_TIMER_Start(&TxTimer);
  /* USER CODE BEGIN OnTxTimerEvent_2 */
	//Detengo el timer porque lo voy a arrancar
	//al terminar el envío en SendTxData
	UTIL_TIMER_Stop(&TxTimer);

  /* USER CODE END OnTxTimerEvent_2 */
}

/* USER CODE BEGIN PrFD_LedEvents */

/* USER CODE END PrFD_LedEvents */

static void OnTxData(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_1 */
  /* USER CODE END OnTxData_1 */
}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_1 */
  /* USER CODE END OnJoinRequest_1 */
}

static void OnBeaconStatusChange(LmHandlerBeaconParams_t *params)
{
  /* USER CODE BEGIN OnBeaconStatusChange_1 */
  /* USER CODE END OnBeaconStatusChange_1 */
}

static void OnClassChange(DeviceClass_t deviceClass)
{
  /* USER CODE BEGIN OnClassChange_1 */
  /* USER CODE END OnClassChange_1 */
}

static void OnMacProcessNotify(void)
{
  /* USER CODE BEGIN OnMacProcessNotify_1 */

  /* USER CODE END OnMacProcessNotify_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);

  /* USER CODE BEGIN OnMacProcessNotify_2 */

  /* USER CODE END OnMacProcessNotify_2 */
}

static void OnTxPeriodicityChanged(uint32_t periodicity)
{
  /* USER CODE BEGIN OnTxPeriodicityChanged_1 */

  /* USER CODE END OnTxPeriodicityChanged_1 */
  TxPeriodicity = periodicity;

  if (TxPeriodicity == 0)
  {
    /* Revert to application default periodicity */
    TxPeriodicity = APP_TX_DUTYCYCLE;
  }

  /* Update timer periodicity */
  UTIL_TIMER_Stop(&TxTimer);
  UTIL_TIMER_SetPeriod(&TxTimer, TxPeriodicity);
  UTIL_TIMER_Start(&TxTimer);
  /* USER CODE BEGIN OnTxPeriodicityChanged_2 */

  /* USER CODE END OnTxPeriodicityChanged_2 */
}

static void OnTxFrameCtrlChanged(LmHandlerMsgTypes_t isTxConfirmed)
{
  /* USER CODE BEGIN OnTxFrameCtrlChanged_1 */

  /* USER CODE END OnTxFrameCtrlChanged_1 */
  LmHandlerParams.IsTxConfirmed = isTxConfirmed;
  /* USER CODE BEGIN OnTxFrameCtrlChanged_2 */

  /* USER CODE END OnTxFrameCtrlChanged_2 */
}

static void OnPingSlotPeriodicityChanged(uint8_t pingSlotPeriodicity)
{
  /* USER CODE BEGIN OnPingSlotPeriodicityChanged_1 */

  /* USER CODE END OnPingSlotPeriodicityChanged_1 */
  LmHandlerParams.PingSlotPeriodicity = pingSlotPeriodicity;
  /* USER CODE BEGIN OnPingSlotPeriodicityChanged_2 */

  /* USER CODE END OnPingSlotPeriodicityChanged_2 */
}

static void OnSystemReset(void)
{
  /* USER CODE BEGIN OnSystemReset_1 */

  /* USER CODE END OnSystemReset_1 */
  if ((LORAMAC_HANDLER_SUCCESS == LmHandlerHalt()) && (LmHandlerJoinStatus() == LORAMAC_HANDLER_SET))
  {
    NVIC_SystemReset();
  }
  /* USER CODE BEGIN OnSystemReset_Last */

  /* USER CODE END OnSystemReset_Last */
}

static void StopJoin(void)
{
  /* USER CODE BEGIN StopJoin_1 */

  /* USER CODE END StopJoin_1 */

  UTIL_TIMER_Stop(&TxTimer);

  if (LORAMAC_HANDLER_SUCCESS != LmHandlerStop())
  {
    APP_LOG(TS_OFF, VLEVEL_M, "LmHandler Stop on going ...\r\n");
  }
  else
  {
    APP_LOG(TS_OFF, VLEVEL_M, "LmHandler Stopped\r\n");
    if (LORAWAN_DEFAULT_ACTIVATION_TYPE == ACTIVATION_TYPE_ABP)
    {
      ActivationType = ACTIVATION_TYPE_OTAA;
      APP_LOG(TS_OFF, VLEVEL_M, "LmHandler switch to OTAA mode\r\n");
    }
    else
    {
      ActivationType = ACTIVATION_TYPE_ABP;
      APP_LOG(TS_OFF, VLEVEL_M, "LmHandler switch to ABP mode\r\n");
    }
    LmHandlerConfigure(&LmHandlerParams);
    LmHandlerJoin(ActivationType, true);
    UTIL_TIMER_Start(&TxTimer);
  }
  UTIL_TIMER_Start(&StopJoinTimer);
  /* USER CODE BEGIN StopJoin_Last */

  /* USER CODE END StopJoin_Last */
}

static void OnStopJoinTimerEvent(void *context)
{
  /* USER CODE BEGIN OnStopJoinTimerEvent_1 */

  /* USER CODE END OnStopJoinTimerEvent_1 */
  if (ActivationType == LORAWAN_DEFAULT_ACTIVATION_TYPE)
  {
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaStopJoinEvent), CFG_SEQ_Prio_0);
  }
  /* USER CODE BEGIN OnStopJoinTimerEvent_Last */

  /* USER CODE END OnStopJoinTimerEvent_Last */
}

static void StoreContext(void)
{
  LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;

  /* USER CODE BEGIN StoreContext_1 */

  /* USER CODE END StoreContext_1 */
  status = LmHandlerNvmDataStore();

  if (status == LORAMAC_HANDLER_NVM_DATA_UP_TO_DATE)
  {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA UP TO DATE\r\n");
  }
  else if (status == LORAMAC_HANDLER_ERROR)
  {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA STORE FAILED\r\n");
  }
  /* USER CODE BEGIN StoreContext_Last */

  /* USER CODE END StoreContext_Last */
}

static void OnNvmDataChange(LmHandlerNvmContextStates_t state)
{
  /* USER CODE BEGIN OnNvmDataChange_1 */

  /* USER CODE END OnNvmDataChange_1 */
  if (state == LORAMAC_HANDLER_NVM_STORE)
  {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA STORED\r\n");
  }
  else
  {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA RESTORED\r\n");
  }
  /* USER CODE BEGIN OnNvmDataChange_Last */

  /* USER CODE END OnNvmDataChange_Last */
}

static void OnStoreContextRequest(void *nvm, uint32_t nvm_size)
{
  /* USER CODE BEGIN OnStoreContextRequest_1 */

  /* USER CODE END OnStoreContextRequest_1 */
  /* store nvm in flash */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    if (FLASH_IF_EraseByPages(PAGE(LORAWAN_NVM_BASE_ADDRESS), 1, 0U) == FLASH_OK)
    {
      FLASH_IF_Write(LORAWAN_NVM_BASE_ADDRESS, (uint8_t *)nvm, nvm_size, NULL);
    }
    HAL_FLASH_Lock();
  }
  /* USER CODE BEGIN OnStoreContextRequest_Last */

  /* USER CODE END OnStoreContextRequest_Last */
}

static void OnRestoreContextRequest(void *nvm, uint32_t nvm_size)
{
  /* USER CODE BEGIN OnRestoreContextRequest_1 */

  /* USER CODE END OnRestoreContextRequest_1 */
  UTIL_MEM_cpy_8(nvm, (void *)LORAWAN_NVM_BASE_ADDRESS, nvm_size);
  /* USER CODE BEGIN OnRestoreContextRequest_Last */

  /* USER CODE END OnRestoreContextRequest_Last */
}

