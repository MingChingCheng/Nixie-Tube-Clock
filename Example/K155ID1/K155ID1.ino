#include <DS3231.h>


const int pinA = 5;
const int pinB = 4;
const int pinC = 3;
const int pinD = 2;
const int pinPWM = 6;



void setup() {
  // put your setup code here, to run once:
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinPWM, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  BCD to Decimal
    	D	C	B	A
   0	0	0	0	0
   1	0	0	0	1
   2	0	0	1	0
   3	0	0	1	1
   4	0	1	0	0
   5	0	1	0	1
   6	0	1	1	0
   7	0	1	1	1
   8	1	0	0	0
   9	1	0	0	1
  */
  int i = 0;
  analogWrite(pinPWM, 20);

  for(i=0 ; i<=9 ; i++)
  {
    Serial.println(i);
    if(i==0)
    {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, LOW);
      digitalWrite(pinD, LOW);
    }
    if(i==1)
    {
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, LOW);
      digitalWrite(pinD, LOW);
    }
    if(i==2)
    {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
      digitalWrite(pinC, LOW);
      digitalWrite(pinD, LOW);
    }
    if(i==3)
    {
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, HIGH);
      digitalWrite(pinC, LOW);
      digitalWrite(pinD, LOW);
    }
    if(i==4)
    {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, HIGH);
      digitalWrite(pinD, LOW);
    }
    if(i==5)
    {
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, HIGH);
      digitalWrite(pinD, LOW);
    }
    if(i==6)
    {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
      digitalWrite(pinC, HIGH);
      digitalWrite(pinD, LOW);
    }
    if(i==7)
    {
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, HIGH);
      digitalWrite(pinC, HIGH);
      digitalWrite(pinD, LOW);
    }
    if(i==8)
    {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, LOW);
      digitalWrite(pinD, HIGH);
    }
    if(i==9)
    {
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, LOW);
      digitalWrite(pinD, HIGH);
    }

    delay(1000);
  } 
  



}
