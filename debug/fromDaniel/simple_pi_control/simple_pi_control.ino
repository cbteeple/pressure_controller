
int ValvePin = 3;
int SensorPin = A0;
int SensorValue = 0; 



int Ref_mean = 868;
int Ref = 0;
float A = 127.0;
float f_ref = 0.1;
float w = 2.0*3.14*f_ref;
int Err=0;
int Cmd = 0;
int Kp = 2;

int Ki = 1;
int SumError = 0;

unsigned long Count = (long)(1000000.0*1.0/f_ref);

unsigned long time = 0;
unsigned long old_time = 0;
unsigned long t=0;

int Period = 500;



float Position2Voltage(float input){
  float result;
  //result = pow(input,2);
  result = -0.016*pow(input,3.0)+0.025*pow(input,2.0)-0.068*input+4;
  return result;
}

int Ref_mean1 = (int)(Position2Voltage(2.0)*1023.0/4.6);
int Ref_mean2 = (int)(Position2Voltage(3.0)*1023.0/4.6);
int Ref_mean3 = (int)(Position2Voltage(4.0)*1023.0/4.6);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ValvePin, OUTPUT);
  setPwmFrequency(ValvePin, 1024);

  Ref_mean = Ref_mean1;
  Serial.println(Ref_mean1);
  Serial.println(Ref_mean2);
  Serial.println(Ref_mean3);
  
  delay(5000);
  Serial.println("Start");
  Count =0;
  old_time = micros();
  //analogWrite(ValvePin,127);

  
}

void loop() {
  /*Serial.println("0");
  analogWrite(ValvePin,0);
  delay(1000);
  Serial.println("128");
  analogWrite(ValvePin,128);
  delay(1000);
  Serial.println("255");
  analogWrite(ValvePin,255);
  delay(1000);
  */
  
  /*  
  for (int i=0;i<255;i++)
  {
    analogWrite(ValvePin,i);
    Serial.println(i);
    delay(100);
  }
  for (int i=255;i>0;i--)
  {
    analogWrite(ValvePin,i);
    Serial.println(i);
    delay(100);
  }
  */

  /*
  sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);
  delay(100);
  */

  
  time = micros();
  //Serial.println((int)((float)A*sin(w*(float)time/1000000.0)));

  //Ref = Ref_mean + (int)((float)A*sin(w*(float)time/1000000.0/10.0));

  Ref = Ref_mean;

  SensorValue = analogRead(SensorPin);
  Err = -(Ref-SensorValue);

  SumError = SumError + Err;

  Cmd = Kp*Err + (Ki*SumError)/4;

  Cmd = constrain(Cmd, 0, 255);
  analogWrite(ValvePin,Cmd);

  
  Serial.print("Count ");
  Serial.print(Count);
  Serial.print(" Sensor ");
  Serial.print(SensorValue);
  Serial.print(" Ref ");
  Serial.print(Ref);
  Serial.print(" Err ");
  Serial.print(Err);
  Serial.print(" Cmd ");
  Serial.println(Cmd);
  

  delay(100);
  Count++;
  if (Count>50){
    if (Ref_mean==Ref_mean1){
      Ref_mean=Ref_mean2;
    } else if (Ref_mean==Ref_mean2)
    {
      Ref_mean=Ref_mean3;
    } else
    {
      Ref_mean=Ref_mean1;
    }
    Count=0;
  }


// 3.5 cm --> 744
// 2.5 cm --> 821
// 1 cm --> 868

  /*
  analogWrite(ValvePin,0);
  delay(Period);
  
  analogWrite(ValvePin,255);
  delay(Period);
  */

  /*
  // sine wave on PWM
  time = micros();

  t = t + (time-old_time);
  old_time = time;
  
  
  w = 2.0*3.14*f_ref;
  Cmd = (int)(A*sin(w*(float)t/1000000.0-0.5*3.1415))+127;
  //Serial.println((int)((float)A*sin(w*(float)time/1000000.0))+127);
  Cmd = constrain(Cmd,0,255);
  analogWrite(ValvePin,Cmd);
  Serial.println(Cmd);

  //Serial.println(Cmd);

  //Serial.println(time+(int)(1000000.0*1.0/f_ref),4);

  */


  /*Serial.print(time);
    Serial.print(" ");
    Serial.println(Count); 
  */
  //f change
  /*if (time>=Count)
  {
    Serial.println("f change //////////////////////////////////////////////////");
    //analogWrite(ValvePin,128);
    //delay(500);
    
    time = micros();
    f_ref = f_ref+0.1;
    Count = time+(long)(1000000.0/f_ref);
    //Serial.println(f_ref,4);
    //Serial.println((float)Count/1000000.0,4);
    t=0;
    old_time=time;
  }
  */

  
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
