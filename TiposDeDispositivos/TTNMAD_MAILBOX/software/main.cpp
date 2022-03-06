/**
 * TTNMAD_MAILBOX
 * Juan Félix Mateos
 * Febrero 2022
 * https://os.mbed.com/docs/mbed-os/v6.15/apis/lora-tech.html
 */

//Hay que cerrar el jumper de la cara posterior del beakout del APDS-9930
#include "mbed.h"
#include "CayenneLPP.h"
CayenneLPP lpp(16);

#include "APDS9930JFMATEOS.h"
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
    DigitalOut LED(PA_8);
    LED.write(1);
    apds9930 APDS9930_2(PB_7,PB_6);      //I2C_1 WORKS
    apds9930 APDS9930_1(PB_4,PA_7);         //I2C_3 WORKS AND DEEP SLEEP
    apds9930 APDS9930_3(PA_11,PA_12);         //I2C_2 WORKS

    uint16_t *proximity_pointer;
    uint16_t proximity_data;
    proximity_pointer=&proximity_data;
    AnalogIn bat(ADC_VREF);
    
    int16_t retcode;
    lpp.reset();
    
    
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

    
    
    lpp.addAnalogInput(1,vdd_calibed); 
    
    
    APDS9930_1.init();
    APDS9930_2.init();
    APDS9930_3.init();
   
    
    
    APDS9930_1.enableProximitySensor(false);
    APDS9930_2.enableProximitySensor(false);
    APDS9930_3.enableProximitySensor(false);
    
    
   
    /*
    When a start condition is detected on the I2C bus, the
    device transitions to the Idle state where it checks the
    Enable register (0x00) PON bit. If PON is disabled, the device
    will return to the Sleep state to save power. Otherwise, the
    device will remain in the Idle state until a proximity or ALS
    function is enabled.
    */
    
    
    APDS9930_1.enablePower();
    ThisThread::sleep_for(100ms);
    APDS9930_1.readProximity(proximity_pointer);
    APDS9930_1.disablePower();
    printf("Proximidad 1: %u\r\n",proximity_data);
    lpp.addAnalogInput(2,(int16_t)proximity_data/100.0);
    
    APDS9930_2.enablePower();
    ThisThread::sleep_for(100ms);
    APDS9930_2.readProximity(proximity_pointer);
    APDS9930_2.disablePower();
    printf("Proximidad 2: %u\r\n",proximity_data);
    lpp.addAnalogInput(3,(int16_t)proximity_data/100.0);
    
    APDS9930_3.enablePower();
    ThisThread::sleep_for(100ms);
    APDS9930_3.readProximity(proximity_pointer);
    APDS9930_3.disablePower(); 
    printf("Proximidad 3: %u\r\n\r\n",proximity_data);     
    lpp.addAnalogInput(4,(int16_t)proximity_data/100.0);
    
    LED.write(0);
    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, lpp.getBuffer(), lpp.getSize(),
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
        printf("Estableciendo periodo heartbeat predeterminado: 60 minutos\n");
        periodoHeartbeat=1;
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
            strncpy(str, convertIntegerToChar(rx_buffer[2]), 5);
            kv_set(kv_key, str, strlen(str), 0);
            printf("Estableciendo Uplinks con ACK: [%s].\n",str);
            uplinkACK=rx_buffer[2];
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