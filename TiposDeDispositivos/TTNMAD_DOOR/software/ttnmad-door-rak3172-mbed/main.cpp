/**
 * TTNMAD_DOOR
 * Juan Félix Mateos
 * Diciembre 2021
 * https://os.mbed.com/docs/mbed-os/v6.15/apis/lora-tech.html
 */
#include "mbed.h"

#include "CayenneLPP.h"
CayenneLPP lpp(11);
void enviarPuertaCerrada();
void enviarPuertaAbierta();
#include <stdio.h>

#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"

// Application helpers
//#include "DummySensor.h"
#include "trace_helper.h"
#include "lora_radio_helper.h"

#include "KVStore.h"
#include "kvstore_global_api.h"
#include <string>
#define DEBUG 1

using namespace events;

// Max payload size can be LORAMAC_PHY_MAXPAYLOAD.
// This example only communicates with much shorter messages (<30 bytes).
// If longer messages are used, these buffers must be changed accordingly.
//uint8_t tx_buffer[30];
uint8_t rx_buffer[30];

bool puertaAbierta;
int periodoHeartbeat; //En minutos
bool uplinkACK;

#define R_PULL_DOWN 8200.0
#define R_25 10000.0
#define TEMPERATURA_NOMINAL 25.0
#define COEFICIENTE_BETA 3950.0

/*
 * Sets up an application dependent transmission timer in ms. Used only when Duty Cycling is off for testing
 */
#define TX_TIMER                        10000

/**
 * Maximum number of events for the event queue.
 * 10 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS            10

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER     3

/**
 * Dummy pin for dummy sensor
 */
//#define PC_9                            0
#define reed_pin    PA_10

InterruptIn reed(reed_pin,PullDown);

LowPowerTimeout heartbeat;



/**
 * Dummy sensor class object
 */
//DS1820  ds1820(PC_9);

/**
* This event queue is the global event queue for both the
* application and stack. To conserve memory, the stack is designed to run
* in the same thread as the application and the application is responsible for
* providing an event queue to the stack that will be used for ISR deferment as
* well as application information event queuing.
*/
static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS *EVENTS_EVENT_SIZE);

/**
 * Event handler.
 *
 * This will be passed to the LoRaWAN stack to queue events for the
 * application which in turn drive the application.
 */
static void lora_event_handler(lorawan_event_t event);

/**
 * Constructing Mbed LoRaWANInterface and passing it the radio object from lora_radio_helper.
 */
static LoRaWANInterface lorawan(radio);

/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;
/**
 * Sends a message to the Network Server
 */
static void send_message(int sensor_value){    
    //sensor_value puede ser 0 o 1, para indicar el estado de la puerta
    //o 2 para indicar un uplink automáticatico en respuesta a un downlink con fpending

    
    DigitalOut NTC_power(PA_0);
    DigitalOut LED(PA_8);
    AnalogIn NTC_pin(PB_2);
    NTC_power.write(1);
    LED.write(1);
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
    AnalogIn bat(ADC_VREF);
    uint16_t    vref_cal; 
    double      vdd_calibed;
    double      vref_calibed;
    vref_cal = *(__IO uint16_t *)((uint32_t)0x1fff75aa);
    vref_calibed = 3.3f * (double)vref_cal / 4096.0f;
    
    vdd_calibed  = vref_calibed / bat.read();   

    //Cálculo temperatura NTC
    //[for Thermistor connected to Gnd ] T = 1/((1/Tnominal) + ln( ((Rother * adcVal) / (adcMax - adcVal))/Rnominal)/B)
    //[for Thermistor connected to Vcc ] T = 1/((1/Tnominal) + ln(((Rother * (adcMax - adcVal)) / adcVal)/Rnominal)/B)
    
    float temperature, resistance;
    unsigned int ADCvalue = NTC_pin.read_u16(); //Devuelve un número entre 0 y FFFF
    NTC_power.write(0);
    temperature= 1.0f/((1.0f/((float)TEMPERATURA_NOMINAL + 273.15f)) + log((float)R_PULL_DOWN * ((65536.0f / (float)ADCvalue)  - 1.0f) / (float)R_25)/(float)COEFICIENTE_BETA);
      
    lpp.addAnalogInput(1,vdd_calibed); 
    if(sensor_value<2){  
        lpp.addDigitalInput(2, sensor_value);
    }else{
        //Si el mensaje no es de apertura/cierre enviamos el estado actual de la puerta
        lpp.addDigitalInput(2, !reed.read());
    }
    lpp.addTemperature(3, temperature-273.15); 
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

void enviarPuertaAbierta(){
    //Deshabilito la interrupción
    reed.fall(NULL);
    puertaAbierta=true;
    ev_queue.call(&send_message,1);

}
void enviarPuertaCerrada(){
    //Deshabilito la interrupción
    reed.rise(NULL);
    puertaAbierta=false;
    ev_queue.call(&send_message,0); 
}
void enviarHeartbeat(){    
    ev_queue.call(&send_message,2); 
}

/**
 * Entry point for application
 */
int main(void){    
    // setup tracing
    setup_trace();
    printf("TTNMAD_DOOR\n");
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
        strncpy(str, "30", 5);
        kv_set(kv_key, str, strlen(str), 0);
        printf("Estableciendo periodo heartbeat predeterminado: 30 minutos\n");
        periodoHeartbeat=30;
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

    // Enable adaptive data rate
    /*
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        return -1;
    }

    printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");
    */
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
            heartbeat.attach(&enviarHeartbeat, periodoHeartbeat*60.0);
        }
        if(rx_buffer[0]==0x05 &&rx_buffer[1]==0x01){
            char kv_key[] = {"/kv/ack"};            
            char str[5];                   
            strncpy(str, convertIntegerToChar(rx_buffer[2]), 5);
            kv_set(kv_key, str, strlen(str), 0);
            printf("Estableciendo Uplinks con ACK: [%s].\n",str);
            uplinkACK=rx_buffer[2];
        }
    }
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * Event handler
 */
static void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            lorawan.set_datarate(DR_4);//SF8
            printf("\r\n Connection - Successful \r\n");            
            puertaAbierta=!reed.read();
            if(puertaAbierta){
                enviarPuertaAbierta();
            }else{
                enviarPuertaCerrada();
            }
            
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
            //Después de cada envío reprogramamos el heartbeat
            heartbeat.attach(&enviarHeartbeat, periodoHeartbeat*60.0);
            if(puertaAbierta){
                if(reed==1){
                    //La puerta se ha cerrado mientras enviabamos el mensaje de apertura
                    enviarPuertaCerrada();                  
                }else{
                    //reed=0
                    //La puerta sigue abierta tras enviar el mensaje de apertura
                    reed.rise(&enviarPuertaCerrada);
                }        
            } else{
                if(reed==1){
                    //La puerta sigue cerrada tras enviar el mensaje de cierre
                    reed.fall(&enviarPuertaAbierta);
                }else{
                    //La puerta se ha abierto mientras enviabamos el mensaje de cierre
                    enviarPuertaAbierta();                   
                }     
            }      
            break;
        case TX_TIMEOUT:
            //Podríamos llegar a este estado por no recibir el ACK de un Uplink
            printf("\r\n Se ha superado el número máximo de intentos sin recibir el ACK \r\n"); 
            //Después de cada envío reprogramamos el heartbeat
            heartbeat.attach(&enviarHeartbeat, periodoHeartbeat*60.0);
            if(puertaAbierta){
                if(reed==1){
                    //La puerta se ha cerrado mientras enviabamos el mensaje de apertura
                    enviarPuertaCerrada();                  
                }else{
                    //reed=0
                    //La puerta sigue abierta tras enviar el mensaje de apertura
                    reed.rise(&enviarPuertaCerrada);
                }        
            } else{
                if(reed==1){
                    //La puerta sigue cerrada tras enviar el mensaje de cierre
                    reed.fall(&enviarPuertaAbierta);
                }else{
                    //La puerta se ha abierto mientras enviabamos el mensaje de cierre
                    enviarPuertaAbierta();                   
                }     
            }  
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
                send_message(2);
            }
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}

// EOF
