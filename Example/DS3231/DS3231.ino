/*

Prints time stamps for 5 seconds using getXXX functions

Based on DS3231_set.pde
by Eric Ayars
4/11

Added printing back of time stamps and increased baud rate
(to better synchronize computer and RTC)
Andy Wickert
5/15/2011

*/

#include <DS3231.h>
#include <Wire.h>

DS3231 myRTC;

bool century = false;
bool h12Flag;
bool pmFlag;


uint8_t dec_to_BCD(int number){
  /*
  Convert a decimal number to a combination of 2 BCD number
  Example1:
  number = 59
  return = 0101 1001

  Example2:
  number = 3
  return = 0000 0011
  */
  int tens = int(number / 10);
  int ones = int(number % 10);

  return (tens << 4) + ones;
}

void setup() {
  // Start the serial port
  Serial.begin(57600);

  // Start the I2C interface
  Wire.begin();
}

void loop() {
  // display time
  delay(1000);

  int hour = myRTC.getHour(h12Flag, pmFlag);
  int minute = myRTC.getMinute();
  int second = myRTC.getSecond();


  Serial.print("Hour = ");
  Serial.println(hour);
  Serial.println(dec_to_BCD(hour), BIN);

  Serial.print("Minute = ");
  Serial.println(minute);
  Serial.println(dec_to_BCD(minute), BIN);

  Serial.print("Second = ");
  Serial.println(second);
  Serial.println(dec_to_BCD(second), BIN);

}