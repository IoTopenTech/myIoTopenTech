void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}
void loop() {
  const float BETA = 3950; // Coeficiente beta de la NTC; su R25 es 10k
  float temperatura = (1. / ((1./(25+273.15))+((1./3950.)*log(((4095./analogRead(PB4))-1)*8000./10000.))))-273.15;
  Serial.print("Temperatura= ");
  Serial.println(temperatura);
  delay(1000);
}
