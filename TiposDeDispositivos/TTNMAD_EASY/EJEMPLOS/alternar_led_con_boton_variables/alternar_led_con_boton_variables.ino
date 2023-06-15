bool led_encendido=false;
void setup() {  
  pinMode(PA0,OUTPUT);
  pinMode(PA8,INPUT_PULLUP);
}
void loop() {
  if(digitalRead(PA8)==LOW){
    led_encendido=!led_encendido;
    delay(100);
  }
  digitalWrite(PA0,led_encendido);
}
