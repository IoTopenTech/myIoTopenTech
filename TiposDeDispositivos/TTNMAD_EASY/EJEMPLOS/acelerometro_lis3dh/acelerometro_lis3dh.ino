#include <SparkFunLIS3DH.h>
#include <Wire.h>
LIS3DH acelerometro( I2C_MODE, 0x19 );
void setup() {
  acelerometro.settings.accelSampleRate = 200;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
  acelerometro.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16
  Serial.begin(115200);
  while (!Serial);
  if ( acelerometro.begin() != 0 )  {
    Serial.println("Error iniciando el acelerómetro en 0x19.");
  }  else  {
    Serial.println("Acelerómetro detectado en 0x19.");
  }
}
void loop() {
  Serial.print(acelerometro.readFloatAccelX(), 4);
  Serial.print(",");  
  Serial.print(acelerometro.readFloatAccelY(), 4);
  Serial.print(",");  
  Serial.println(acelerometro.readFloatAccelZ(), 4);
  //delay(10);
}
