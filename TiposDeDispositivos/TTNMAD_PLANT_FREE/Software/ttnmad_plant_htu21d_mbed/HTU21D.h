/**
 * @author Alex Lipford
 * Georgia Institute of Technology 
 * ECE 4180 Embeded Systems Design
 * Professor Hamblen
 * 10/19/2014
 * 
 * @section LICENSE
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alexlipford@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 *
 * @section DESCRIPTION
 *
 * Honeywell HTU21D Humidity and Temperature sensor.
 *
 * Datasheet, specs, and information:
 *
 * https://www.sparkfun.com/products/12064
 */
 
#ifndef HTU21D_H
#define HTU21D_H
 
/**
 * Includes
 */
#include "mbed.h"
 
/**
 * Defines
 */
// Acquired from Datasheet.
 
#define HTU21D_I2C_ADDRESS  0x40 
#define TRIGGER_TEMP_MEASURE  0xE3
#define TRIGGER_HUMD_MEASURE  0xE5
 
 
//Commands.
#define HTU21D_EEPROM_WRITE 0x80
#define HTU21D_EEPROM_READ  0x81
 
 
/**
 * Honeywell HTU21D digital humidity and temperature sensor.
 */
class HTU21D {
private:
    PinName _sda;
    PinName _scl;
 
public:
 
    /**
     * Constructor.
     *
     * @param sda mbed pin to use for SDA line of I2C interface.
     * @param scl mbed pin to use for SCL line of I2C interface.
     */
    HTU21D(PinName sda, PinName scl);
 
 
    //Samples the temperature, input void, outputs an int in celcius.
    float sample_ctemp(void);
    
       //Samples the temperature, input void, outputs an int in fahrenheit.
    int sample_ftemp(void);
    
       //Samples the temperature, input void, outputs an int in kelvin.
    int sample_ktemp(void);
    
    //Samples the humidity, input void, outputs and int.
    float sample_humid(void);
 
   
 
private:
 
    I2C* i2c_;
 
    /**
     * Write to EEPROM or RAM on the device.
     *
     * @param EepromOrRam 0x80 -> Writing to EEPROM
     * @param address Address to write to.
     * @param data Data to write.
     */
    void write(int EepromOrRam, int address, int data);
 
    /**
     * Read EEPROM or RAM on the device.
     *
     * @param EepromOrRam 0x81 -> Reading from EEPROM
     * @param address Address to read from.
     * @return The contents of the memory address.
     */
    int read(int EepromOrRam, int address);
 
};
 
#endif /* HTU21D_H */