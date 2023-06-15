void setup() {  
  pinMode(PA0,OUTPUT);
  pinMode(PA8,INPUT_PULLUP);
}
void loop() {
  digitalWrite(PA0,!digitalRead(PA8));
}
