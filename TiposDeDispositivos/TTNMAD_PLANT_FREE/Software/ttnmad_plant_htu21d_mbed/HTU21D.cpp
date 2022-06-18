	

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
 * HTU21D Humidity and Temperature sensor.
 *
 * Datasheet, specs, and information:
 *
 * https://www.sparkfun.com/products/12064
 */
 
/**
 * Includes
 */
#include "HTU21D.h"
 
HTU21D::HTU21D(PinName sda, PinName scl) {
    _sda = sda;
    _scl = scl;
    /*
    i2c_ = new I2C(sda, scl);
    //400KHz, as specified by the datasheet.
    i2c_->frequency(400000);
    */
 
 
}
 
float HTU21D::sample_ctemp(void) {
 
    char tx[1];
    char rx[2];
    I2C i2c_(_sda,_scl);
    i2c_.frequency(400000);
    tx[0] = TRIGGER_TEMP_MEASURE; // Triggers a temperature measure by feeding correct opcode.
    i2c_.write((HTU21D_I2C_ADDRESS << 1) & 0xFE, tx, 1);
    wait_us(50000); // Per datasheet, wait long enough for device to sample temperature
    
    // Reads triggered measure
    i2c_.read((HTU21D_I2C_ADDRESS << 1) | 0x01, rx, 2);
    wait_us(1000);
    
    // Algorithm from datasheet to compute temperature.
    unsigned int rawTemperature = ((unsigned int) rx[0] << 8) | (unsigned int) rx[1];
    rawTemperature &= 0xFFFC;
 
    float tempTemperature = rawTemperature / (float)65536; //2^16 = 65536
    float realTemperature = -46.85 + (175.72 * tempTemperature); //From page 14
    i2c_.free();
    return realTemperature;
 
}
 
int HTU21D::sample_ftemp(void){
    int temptemp = sample_ctemp();
    int ftemp = temptemp * 1.8 + 32;
    
    return ftemp;
}
 
int HTU21D::sample_ktemp(void){
    int temptemp = sample_ctemp();
    int ktemp = temptemp + 274;
    
    return ktemp;
    
    
}
 
float HTU21D::sample_humid(void) {
 
    char tx[1];
    char rx[2];
    I2C i2c_(_sda,_scl);
    i2c_.frequency(400000);
    tx[0] = TRIGGER_HUMD_MEASURE; // Triggers a humidity measure by feeding correct opcode.
    i2c_.write((HTU21D_I2C_ADDRESS << 1) & 0xFE, tx, 1);
    wait_us(16000); // Per datasheet, wait long enough for device to sample humidity
    
    // Reads triggered measure
    i2c_.read((HTU21D_I2C_ADDRESS << 1) | 0x01, rx, 2);
    wait_us(1000);
    
    //Algorithm from datasheet.
    unsigned int rawHumidity = ((unsigned int) rx[0] << 8) | (unsigned int) rx[1];
 
    rawHumidity &= 0xFFFC; //Zero out the status bits but keep them in place
    
    //Given the raw humidity data, calculate the actual relative humidity
    float tempRH = rawHumidity / (float)65536; //2^16 = 65536
    float rh = -6 + (125 * tempRH); //From page 14
    i2c_.free();
 
    return rh;
 
}