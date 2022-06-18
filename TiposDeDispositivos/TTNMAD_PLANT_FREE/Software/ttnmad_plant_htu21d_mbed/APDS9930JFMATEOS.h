
#ifndef MBED_APDS9930_H
#define MBED_APDS9930_H
#include "mbed.h"

/* APDS-9930 I2C address */
#define APDS9930_I2C_ADDR       0x39

/* Command register modes */
#define REPEATED_BYTE           0x80
#define AUTO_INCREMENT          0xA0
#define SPECIAL_FN              0xE0

/* Error code for returned values */
#define ERROR                   0xFF

/* Acceptable device IDs */
#define APDS9930_ID_1           0x12
#define APDS9930_ID_2           0x39

/* Misc parameters */
#define FIFO_PAUSE_TIME         30      // Wait period (ms) between FIFO reads

/* APDS-9930 register addresses */
#define APDS9930_ENABLE         0x00
#define APDS9930_ATIME          0x01
#define APDS9930_PTIME          0x02
#define APDS9930_WTIME          0x03
#define APDS9930_AILTL          0x04
#define APDS9930_AILTH          0x05
#define APDS9930_AIHTL          0x06
#define APDS9930_AIHTH          0x07
#define APDS9930_PILTL          0x08
#define APDS9930_PILTH          0x09
#define APDS9930_PIHTL          0x0A
#define APDS9930_PIHTH          0x0B
#define APDS9930_PERS           0x0C
#define APDS9930_CONFIG         0x0D
#define APDS9930_PPULSE         0x0E
#define APDS9930_CONTROL        0x0F
#define APDS9930_ID             0x12
#define APDS9930_STATUS         0x13
#define APDS9930_Ch0DATAL       0x14
#define APDS9930_Ch0DATAH       0x15
#define APDS9930_Ch1DATAL       0x16
#define APDS9930_Ch1DATAH       0x17
#define APDS9930_PDATAL         0x18
#define APDS9930_PDATAH         0x19
#define APDS9930_POFFSET        0x1E


/* Bit fields */
#define APDS9930_PON            0b00000001
#define APDS9930_AEN            0b00000010
#define APDS9930_PEN            0b00000100
#define APDS9930_WEN            0b00001000
#define APSD9930_AIEN           0b00010000
#define APDS9930_PIEN           0b00100000
#define APDS9930_SAI            0b01000000

/* On/Off definitions */
#define OFF                     0
#define ON                      1

/* Acceptable parameters for setMode */
#define POWER                   0
#define AMBIENT_LIGHT           1
#define PROXIMITY               2
#define WAIT                    3
#define AMBIENT_LIGHT_INT       4
#define PROXIMITY_INT           5
#define SLEEP_AFTER_INT         6
#define ALL                     7

/* LED Drive values */
#define LED_DRIVE_100MA         0
#define LED_DRIVE_50MA          1
#define LED_DRIVE_25MA          2
#define LED_DRIVE_12_5MA        3

/* Proximity Gain (PGAIN) values */
#define PGAIN_1X                0
#define PGAIN_2X                1
#define PGAIN_4X                2
#define PGAIN_8X                3

/* ALS Gain (AGAIN) values */
#define AGAIN_1X                0
#define AGAIN_8X                1
#define AGAIN_16X               2
#define AGAIN_120X              3

/* Interrupt clear values */
#define CLEAR_PROX_INT          0xE5
#define CLEAR_ALS_INT           0xE6
#define CLEAR_ALL_INTS          0xE7

/* Default values */
#define DEFAULT_ATIME           0xED
#define DEFAULT_WTIME           0xFF
#define DEFAULT_PTIME           0xFF
#define DEFAULT_PPULSE          0x08
#define DEFAULT_POFFSET         0       // 0 offset
#define DEFAULT_CONFIG          0
#define DEFAULT_PDRIVE          LED_DRIVE_100MA
//#define DEFAULT_PDRIVE          LED_DRIVE_12_5MA
#define DEFAULT_PDIODE          2
#define DEFAULT_PGAIN           PGAIN_1X
#define DEFAULT_AGAIN           AGAIN_1X
#define DEFAULT_PILT            0       // Low proximity threshold
#define DEFAULT_PIHT            50      // High proximity threshold
#define DEFAULT_AILT            0xFFFF  // Force interrupt for calibration
#define DEFAULT_AIHT            0
#define DEFAULT_PERS            0x22    // 2 consecutive prox or ALS for int.

/* ALS coefficients */
#define DF                      52
#define GA                      0.49
#define B                       1.862
#define C                       0.746
#define D                       1.291

/* State definitions */
enum {
  NOTAVAILABLE_STATE,
  NEAR_STATE,
  FAR_STATE,
  ALL_STATE
};


 
/** interface class for configuring, sending and recieving data using an APDS-9930 */
class apds9930
{
private:
    PinName _sda;
    PinName _scl;
   
public:
  
    apds9930(PinName sda, PinName scl);
    bool init(void);
    uint8_t getMode(void);
    bool setMode(uint8_t mode, uint8_t enable);


// Turn the APDS-9930 on and off
bool enablePower(void);
bool disablePower(void);

// Enable or disable specific sensors
bool enableLightSensor(bool interrupts);
bool disableLightSensor(void);
bool enableProximitySensor(bool interrupts);
bool disableProximitySensor(void);

// LED drive strength control
uint8_t getLEDDrive(void);
bool setLEDDrive(uint8_t drive);

// Gain control
bool setProximityGain(uint8_t gain);
uint8_t getProximityGain(void);
bool setAmbientLightGain(uint8_t gain);
uint8_t getAmbientLightGain(void);
bool setProximityDiode(uint8_t gain);
uint8_t getProximityDiode(void);

// Get and set interrupt enables
uint8_t getAmbientLightIntEnable(void);
bool setAmbientLightIntEnable(uint8_t enable);
uint8_t getProximityIntEnable(void);
bool setProximityIntEnable(uint8_t enable);

// Clear interrupts
bool clearAmbientLightInt(void);
bool clearProximityInt(void);
bool clearAllInts(void);

// Get and set light interrupt thresholds
bool getLightIntLowThreshold(uint16_t *threshold);
bool setLightIntLowThreshold(uint16_t threshold);
bool getLightIntHighThreshold(uint16_t *threshold);
bool setLightIntHighThreshold(uint16_t threshold);

// Proximity Interrupt Threshold
uint16_t getProximityIntLowThreshold(void);
bool setProximityIntLowThreshold(uint16_t threshold);
uint16_t getProximityIntHighThreshold(void);
bool setProximityIntHighThreshold(uint16_t threshold);

/* Proximity methods */
bool readProximity(uint16_t *val);

// Ambient light methods
bool readCh0Light(uint16_t *val);
bool readCh1Light(uint16_t *val);
float floatAmbientToLux(uint16_t Ch0, uint16_t Ch1);
unsigned long ulongAmbientToLux(uint16_t Ch0, uint16_t Ch1);
bool readAmbientLightLuxF(float *val);
bool readAmbientLightLuxUL(unsigned long *val);

float max_value(float x, float y);

// I2C

bool read_byte(uint8_t reg, uint8_t *val);
bool write_byte(uint8_t reg, uint8_t val);
bool write_cmd(uint8_t val);
bool abort();

    
};
#endif