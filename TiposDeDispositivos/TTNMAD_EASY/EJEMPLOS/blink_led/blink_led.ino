void setup() {
  // put your setup code here, to run once:
  pinMode(PA0,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PA0,HIGH);
  delay(1000);
  digitalWrite(PA0,LOW);
  delay(1000);
}
