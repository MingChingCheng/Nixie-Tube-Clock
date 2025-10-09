// This code is designed with Arduino Nano


/* Include libraries */
// For Humidity Sensor AM2320
#include <Adafruit_AM2320.h>
#include <Adafruit_Sensor.h>

// For Real Time Clock
#include <DS3231.h>
#include <Wire.h>

// For Rotary Encoder
#include <RotaryEncoder.h>
#include <Arduino.h>



/* Define functions */
void show_time();
void show_temp();
void set_time();
void show_humidity();
uint8_t dec_to_BCD(int number);
void change_mode();


/* Define Pins */ 
// 74HC595
const int latch_pin = D7;
const int clock_pin = D4;
const int data_pin = D2;

// nixie tube
const int nixie_brightness_pin = D3;

// LED
const int led_brightness_pin = D13;
const int led_red_pin = D9;
const int led_green_pin = D10;
const int led_blue_pin = D11;

// I2C
const int SCL_pin = A5;
const int SDA_pin = A4;

// Rotary encoder
const int rotary_switch_pin = A2;
const int rotary_data_pin = A1;
const int rotary_clock_pin = A0;


/* Define Objects and global variables of all components */
// For Humidity sensor
Adafruit_AM2320 am2320 = Adafruit_AM2320();
float humidity = 0;
float temp = 0;

// For Real Time Clock
DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;
int hour = 0;
int new_hour = 0;
int minute = 0;
int new_minute = 0;

// For Rotary Encoder
volatile int pos = 0;
volatile int new_pos = 0;
RotaryEncoder encoder(rotary_clock_pin, rotary_data_pin, RotaryEncoder::LatchMode::FOUR3);





void setup() {

  Serial.begin(9600);
  while (!Serial) {
    delay(10);
    // hang out until serial port open
  }


  Wire.begin();  // Start the I2C interface
  am2320.begin();

  // Define pin mode
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Real Time Clock
  // mode = false: 24h
  // mode = true: 12h
  myRTC.setClockMode(false);
  
  // Detect rotary encoder
  // TODO:
  // may need to re-layout??
  // External interrupt for Arduino Uno: D2 and D3
  //                    for Arduino Nano: D2 and D3
  encoder.setPosition(0);
  attachInterrupt(digitalPinToInterrupt(rotary_switch_pin), change_mode, FALLING);
}

void loop() {

  temp = am2320.readTemperature();
  humidity = am2320.readHumidity();

  show_time();
  delay(100);
}


void show_time() {

  // read time by RTC functions
  int hour = myRTC.getHour(h12Flag, pmFlag);
  int minute = myRTC.getMinute();
  
  // convert time to BCD format
  uint8_t hour_uint8 = dec_to_BCD(hour);
  uint8_t minute_uint8 = dec_to_BCD(minute);

  Serial.print("Hour = ");
  Serial.println(hour);
  Serial.println(dec_to_BCD(hour), BIN);

  Serial.print("Minute = ");
  Serial.println(minute);
  Serial.println(dec_to_BCD(minute), BIN);
  
  // send data to display
  byte high_Byte = highByte(hour_uint8);
  byte low_Byte = lowByte(minute_uint8);
  
  digitalWrite(latchPin, LOW); // let latch to LOW before sending data
  shiftOut(dataPin, clockPin, MSBFIRST, high_Byte); // send "High Byte" to further 74HC595
  shiftOut(dataPin, clockPin, MSBFIRST, low_Byte); // send "Low Byte" to closer 74HC595
  digitalWrite(latchPin, HIGH); // let latch to HIGH after sending data


}


uint8_t dec_to_BCD(int number) {
  /*
    TODO:
    may need to invert the order of tens and ones

    Convert a decimal number to a combination of 2 BCD number
    Example 1:
    number = 59
    return = 0101 1001

    Example 2:
    number = 3
    return = 0000 0011
  */
  int tens = int(number / 10);
  int ones = int(number % 10);

  return (tens << 4) + ones;
}

void set_time() {

  // monitor the rotary 
  encoder.tick();

  // initial state
  new_hour = hour;
  new_minute = minute;

  // TODO: 
  // setting mode change
  // set hour 
  new_pos = encoder.getPosition();
  if (pos != new_pos) {
    Serial.print("pos:");
    Serial.print(new_pos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    new_hour = new_hour + (int)(encoder.getDirection())
    pos = new_pos;
  }

  // set minute
  new_pos = encoder.getPosition();
  if (pos != new_pos) {
    Serial.print("pos:");
    Serial.print(new_pos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    new_minute = new_minute + (int)(encoder.getDirection())
    pos = new_pos;
  }


  // store to RTC while the button is clicked
  myRTC.setHour(hour);
  myRTC.setMinute(minute);
}


void change_mode()
{
  // TODO:
  // change mode
  //   show time
  //   set time
  //   poison
  //   show temp
  //   show humidity
  
}