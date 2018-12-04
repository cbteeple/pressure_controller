void setup() {
  // put your setup code here, to run once:
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  //analogWrite(6,255);
  //analogWrite(9,0);
  //delay(1000);
  
  //analogWrite(6,0);
  //analogWrite(9,255);
  //delay(1000);

  digitalWrite(6,HIGH);
  digitalWrite(9,LOW);
  delay(1000);

  digitalWrite(6,LOW);
  digitalWrite(9,HIGH);
  delay(1000);

}
