/**
 * TTNMAD_MAILBOX
 * Juan Félix Mateos
 * Febrero 2022
 * https://os.mbed.com/docs/mbed-os/v6.15/apis/lora-tech.html
 */

//Hay que cerrar el jumper de la cara posterior del beakout del APDS-9930
#include "mbed.h"

#include "HTU21D.h"
  

#include <stdio.h>
#include "mbed_stats.h"
#include "i2c_api.h"
#include "i2c.h"

#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"

#include "trace_helper.h"
#include "lora_radio_helper.h"


#include "KVStore.h"
#include "kvstore_global_api.h"
#include <string>
#define DEBUG 1

#define VCC_HTU21D  PA_1
#define VCC_SENSORS  PA_0
#define LDR  PB_2
#define SOIL_MOISTURE  PA_10


// Application helpers
//#include "DummySensor.h"

using namespace events;
uint8_t rx_buffer[30];

int periodoHeartbeat; //En minutos
bool uplinkACK=false;

#define MAX_NUMBER_OF_EVENTS            10
#define CONFIRMED_MSG_RETRY_COUNTER     3

LowPowerTimeout heartbeat;

static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS *EVENTS_EVENT_SIZE);
static void lora_event_handler(lorawan_event_t event);
static LoRaWANInterface lorawan(radio);
static lorawan_app_callbacks_t callbacks;

void leer_sensores(){
    uint8_t payload[9];
    int32_t comodin;
    unsigned int medidas[10], medidas_ordenadas[10], minimo, indice_minimo;
    DigitalOut LED(PA_8);
    LED.write(1);    
    

    AnalogIn bat(ADC_VREF);
    
    int16_t retcode;
        
    
    //Calculate Voltage data

    //Vdd:        CPU Vdd line voltage
    //            (If VREF+ connected Vdd and VREF- connected GND)
    //dt_adc:     ADC data (12bit)
    //VREFINT:    Internal bandgap voltage
    //            1.182V(min), 1.212V(typ), 1.232V(max)
    //VREFINT_CAL Raw data acquired at temperature of 30 °C,
    //            Vdd = 3.3 V Addr: 0x1FFF75AA-0x1FFF75AB

    //VREFINT_CAL = 3000 * VREF(Factory) / 4096      [V]
    //Vdd         = VREFINT_CAL / VREFINT(ADC float) [V] 
    
    uint16_t    vref_cal; 
    double      vdd_calibed;
    double      vref_calibed;
    vref_cal = *(__IO uint16_t *)((uint32_t)0x1fff75aa);
    vref_calibed = 3.3f * (double)vref_cal / 4096.0f;
    
    vdd_calibed  = vref_calibed / bat.read();   
    comodin=vdd_calibed*1000;
    
    payload[0] = comodin >> 8;
    payload[1] = comodin;
    
    
    DigitalOut EN_SENSORS(VCC_SENSORS);
    EN_SENSORS.write(1);
    AnalogIn SOIL_MOISTURE_pin(SOIL_MOISTURE);
    //Medida de moisture    
    for (unsigned char i = 0; i < 10; i++) {
        medidas[i] = SOIL_MOISTURE_pin.read_u16(); //Devuelve un número entre 0 y FFFF
        wait_us(300000);
    }
    for (unsigned char i = 0; i < 10; i++) {
        minimo = 0xFFFF;
        for (unsigned char j = 0; j < 10; j++) {
            if (medidas[j] < minimo) {
                minimo = medidas[j];
                indice_minimo = j;
            }
        }
        medidas_ordenadas[i] = medidas[indice_minimo];
        medidas[indice_minimo] = 0xFFFF;
    }
    comodin = (medidas_ordenadas[4] + medidas_ordenadas[5]) / 2;
    //Tengo que reducirlo a 10 bits (1023) porque es el máximo en Arduino
    comodin=(comodin*1023)/65535;
    payload[5] = comodin >> 8;
    payload[6] = comodin;
    //Medida de LDR    
    AnalogIn LDR_pin(LDR);
    for (unsigned char i = 0; i < 10; i++) {
        medidas[i] = LDR_pin.read_u16();;
        wait_us(300000);
    }
    for (unsigned char i = 0; i < 10; i++) {
        minimo = 0xFFFF;
        for (unsigned char j = 0; j < 10; j++) {
            if (medidas[j] < minimo) {
                minimo = medidas[j];
                indice_minimo = j;
            }
        }
        medidas_ordenadas[i] = medidas[indice_minimo];
        medidas[indice_minimo] = 0xFFFF;
    }
    comodin = (medidas_ordenadas[4] + medidas_ordenadas[5]) / 2;
    comodin=(comodin*1023)/65535;
    payload[7] = comodin >> 8;
    payload[8] = comodin;

    EN_SENSORS.write(0);

    DigitalOut powerHTU21D(VCC_HTU21D);
    powerHTU21D.write(1);
    HTU21D htu(PB_4, PA_7); //Temp humid sensor || SDA, SCL
    
    float H21Temp;
    float H21Hum;
    H21Temp = htu.sample_ctemp();
    H21Hum = htu.sample_humid();
    
    payload[2] = ((int)(H21Temp * 10)) >> 8;
    payload[3] = ((int)(H21Temp * 10));
    payload[4] = ((int)(H21Hum * 2));
    powerHTU21D.write(0);
    LED.write(0);
    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, payload, 9,
                        uplinkACK==0?0x01:0x02);

    if (retcode < 0) {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - WOULD BLOCK\r\n")
        : printf("\r\n send() - Error code %d \r\n", retcode);

        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
            //retry in 3 seconds
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                //ev_queue.call_in(3s, send_message);
            }
        }
        return;
    }

    printf("\r\n %d bytes scheduled for transmission \r\n", retcode);
        
}

void enviarHeartbeat(){    
    ev_queue.call(&leer_sensores); 
}

int main() {
    //LED=0;
    // setup tracing
    setup_trace();

    printf("TTNMAD_MAILBOX\n");
    //Obtenego la configuración (periodo heartbeat (hb) y uplink con (ack))    
    int res = MBED_ERROR_NOT_READY;
    char kv_key[] = {"/kv/hbm"};
    char kv_value[5];
    char str[5];
    kv_info_t info;

    kv_get_info(kv_key, &info);
    //printf("kv_get_info key: %s\n", kv_key);
    //printf("kv_get_info info - size: %u, flags: %lu\n", info.size, info.flags);

    res = kv_get(kv_key, kv_value, info.size, 0);
    if(res == MBED_SUCCESS){
        printf("Periodo heartbeat: [%s] minutos\n", kv_value);
        sscanf(kv_value, "%d", &periodoHeartbeat);
    }else{ 
        strncpy(str, "10", 5);
        kv_set(kv_key, str, strlen(str), 0);
        printf("Estableciendo periodo heartbeat predeterminado: 10 minutos\n");
        periodoHeartbeat=10;
    }
    strncpy(kv_key, "/kv/ack", 7);
    kv_get_info(kv_key, &info);
    res = kv_get(kv_key, kv_value, info.size, 0);
    if(res == MBED_SUCCESS){
        printf("Uplinks con ack: [%s]\n", kv_value);
        int temp;
        sscanf(kv_value, "%d", &temp);
        uplinkACK=temp;
    }else{ 
        strncpy(str, "0", 5);
        kv_set(kv_key, str, strlen(str), 0);
        printf("Estableciendo Uplinks con ACK predeterminado: No\n");
        uplinkACK=0;
    }

    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("\r\n LoRa initialization failed! \r\n");
        return -1;
    }

    printf("\r\n Mbed LoRaWANStack initialized \r\n");        
    
    //printf("Estado del pin PA_10 %d",reed.read());
    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER)
            != LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
        return -1;
    }

    printf("\r\n CONFIRMED message retries : %d \r\n",
           CONFIRMED_MSG_RETRY_COUNTER);
    
    lorawan.disable_adaptive_datarate();
    
    retcode = lorawan.connect();

    if (retcode == LORAWAN_STATUS_OK ||
            retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
    } else {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        return -1;
    }

    printf("\r\n Connection - In Progress ...\r\n");

    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();     
    return 0;
    
}

// Function to convert integer to
// character array
char* convertIntegerToChar(int N)
{
    
    // Count digits in number N
    int m = N;
    int digit = 0;
    do {
 
        // Increment number of digits
        digit++;
 
        // Truncate the last
        // digit from the number
        m /= 10;
    }while (m);
 
    // Declare char array for result
    char* arr;
 
    // Declare duplicate char array
    char arr1[digit];
 
    // Memory allocation of array
    arr = (char*)malloc(digit);
 
    // Separating integer into digits and
    // accommodate it to character array
    int index = 0;
    do {
 
        // Separate last digit from
        // the number and add ASCII
        // value of character '0' is 48
        arr1[++index] = N % 10 + '0';
 
        // Truncate the last
        // digit from the number
        N /= 10;
    }while (N);

    // Reverse the array for result
    int i;
    for (i = 0; i < index; i++) {
        arr[i] = arr1[index - i];
    }
 
    // Char array truncate by null

    arr[i] = '\0';

    // Return char array
    return (char*)arr;
}

/**
 * Receive a message from the Network Server
 */
static void receive_message()
{
    uint8_t port;
    int flags;
    int16_t retcode = lorawan.receive(rx_buffer, sizeof(rx_buffer), port, flags);

    if (retcode < 0) {
        printf("\r\n receive() - Error code %d \r\n", retcode);
        return;
    }

    printf(" RX Data on port %u (%d bytes) (flags: %02x): ", port, retcode,flags);
    for (uint8_t i = 0; i < retcode; i++) {
        printf("%02x ", rx_buffer[i]);
    }

    printf("\r\n");
    //El downlink del periodo de heartbeat tendrá el formato canal (04), tipo de valor (01=digital output), valor (0 a 255), FF (ACK)
    //Sólo voy a atender los downlink que entren por el puerto 99
    if(port==99){
        if(rx_buffer[0]==0x04 &&rx_buffer[1]==0x01){
            char kv_key[] = {"/kv/hbm"};            
            char str[5];                   
            strncpy(str, convertIntegerToChar(rx_buffer[2]), 5);
            kv_set(kv_key, str, strlen(str), 0);
            printf("Estableciendo periodo heartbeat: [%s].\n",str);
            periodoHeartbeat=rx_buffer[2];
            //Reconfiguro el periodo de envío                
            heartbeat.attach(&enviarHeartbeat, periodoHeartbeat*60.0);
        }else if(rx_buffer[0]==0x05 &&rx_buffer[1]==0x01){
            char kv_key[] = {"/kv/ack"};            
            char str[5]; 
            if(rx_buffer[2]==1){
                //desactivar
                strncpy(str, convertIntegerToChar(0), 5);
                uplinkACK=0;
            } else{
                strncpy(str, convertIntegerToChar(1), 5);
                uplinkACK=1;
            }                 
            //strncpy(str, convertIntegerToChar(rx_buffer[2]), 5);
            kv_set(kv_key, str, strlen(str), 0);
            printf("Estableciendo Uplinks con ACK: [%s].\n",str);
            //uplinkACK=rx_buffer[2];
        }else if(rx_buffer[0]==0x06 &&rx_buffer[1]==0x01&&rx_buffer[2]==0x01){
            //soft reset
            NVIC_SystemReset();
        }
    }
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

static void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            lorawan.set_datarate(DR_4);//SF8
            printf("\r\n Connection - Successful \r\n"); 
            ev_queue.call(leer_sensores); 
            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("\r\n Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            //Si el uplink es con ACK, sólo llegaremos a este estado
            //cuando se reciba el ACK
            //En este caso, si se supera el número de intentos
            //sin recibir el ACK, se pasa al estado TX_TIMEOUT
            printf("\r\n Message Sent to Network Server \r\n"); 
            heartbeat.attach(&enviarHeartbeat, periodoHeartbeat*60.0);
            break;
        case TX_TIMEOUT:
            //Podríamos llegar a este estado por no recibir el ACK de un Uplink
            printf("\r\n Se ha superado el número máximo de intentos sin recibir el ACK \r\n"); 
            heartbeat.attach(&enviarHeartbeat, periodoHeartbeat*60.0);
            break;
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            // try again
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                //send_message();
            }
            break;
        case RX_DONE:
            printf("\r\n Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("\r\n Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE:
            printf("\r\n OTAA Failed - Check Keys \r\n");
            break;
        case UPLINK_REQUIRED:
            printf("\r\n Uplink required by NS \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                ev_queue.call(leer_sensores); 
            }
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}