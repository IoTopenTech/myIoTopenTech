byte duty_cycle=0;
int incremento=1;
void setup() {
  
}
void loop() {
  if(duty_cycle==255){
    incremento=-5;
  }else if(duty_cycle==0){
    incremento=5;
  }
  duty_cycle+=incremento;
  analogWrite(PA0,duty_cycle);
  delay(10); 
}
