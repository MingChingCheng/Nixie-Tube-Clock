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



/* Define functions */
void show_time();
void show_temp();
void show_humidity();
void display(int a, int b, int c);


/* Define Pins */
// 74HC595

const int latch_pin = 7;
const int clock_pin = 4;
const int data_pin = 2;

// Nixie tube
const int nixie_brightness_pin = 3;

// LED
// TODO: check led pin later
const int led_brightness_pin = 13;
const int led_red_pin = 9;
const int led_green_pin = 10;
const int led_blue_pin = 11;

// I2C
const int SCL_pin = A5;
const int SDA_pin = A4;

// Rotary encoder
// TODO: check rotary encoder pin later
const int rotary_switch_pin = A2;
const int rotary_data_pin = A1;
const int rotary_clock_pin = A0;

// fan
const int fan_pin = 5;


/* Define Objects and global variables of all components */
// Humidity sensor
Adafruit_AM2320 am2320 = Adafruit_AM2320();
float humidity = 0;
float temp = 0;

// Real Time Clock
DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;
int hour = 0;
int new_hour = 0;
int minute = 0;
int new_minute = 0;

// Rotary Encoder
volatile int pos = 0;
volatile int new_pos = 0;
volatile int state = 0;
RotaryEncoder encoder(rotary_clock_pin, rotary_data_pin, RotaryEncoder::LatchMode::FOUR3);





void setup() {

  // Define pin mode
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(nixie_brightness_pin, OUTPUT);

  pinMode(led_brightness_pin, OUTPUT);
  pinMode(led_red_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  pinMode(led_blue_pin, OUTPUT);

  pinMode(fan_pin, OUTPUT);


  // Initial components
  Wire.begin();               // Start the I2C interface
  am2320.begin();             // Humidity sensor
  myRTC.setClockMode(false);  // Real Time Clock  mode = false: 24h, mode = true: 12h
  encoder.setPosition(0);     // Rotary encoder initialize

  // attachInterrupt(digitalPinToInterrupt(rotary_switch_pin), change_mode, FALLING);
}

void loop() {

  show_time();
  delay(10);
}


void show_time() {

  // get time from RTC
  hour = myRTC.getHour(h12Flag, pmFlag);
  minute = myRTC.getMinute();

  // transform to digits
  int hour_tens = (int)(hour / 10);
  int hour_ones = (int)(hour % 10);
  int minute_tens = (int)(minute / 10);
  int minute_ones = (int)(minute % 10);

  // output
  display(hour_tens, hour_ones, minute_tens, minute_ones);
}


void show_temp() {

  // get temp from AM2320
  temp = am2320.readTemperature();


  // transform to digits
  int temp_int = (int)(temp * 100);

  int temp_tens = (int)(temp_int / 1000 % 10);
  int temp_ones = (int)(temp_int / 100 % 10);
  int temp_p_ones = (int)(temp_int / 10 % 10);
  int temp_p_tens = (int)(temp_int / 1 % 10);

  // output
  display(temp_tens, temp_ones, temp_p_ones, temp_p_tens);
}

void show_humidity() {

  // get humidity from AM2320
  humidity = am2320.readHumidity();

  // transform to digits
  int humidity_int = (int)(temp * 100);

  int humidity_tens = (int)(humidity_int / 1000 % 10);
  int humidity_ones = (int)(humidity_int / 100 % 10);
  int humidity_p_ones = (int)(humidity_int / 10 % 10);
  int humidity_p_tens = (int)(humidity_int / 1 % 10);

  // output
  display(humidity_tens, humidity_ones, humidity_p_ones, humidity_p_tens);
}


void display(int a, int b, int c, int d) {

  // convert digits to binary format
  byte low_Byte = (a << 4) | (b);
  byte high_Byte = (c << 4) | (d);

  // sending data to two 74HC595 ICs
  digitalWrite(latch_pin, LOW);                        // pull down "latch pin" before sending data
  shiftOut(data_pin, clock_pin, MSBFIRST, high_Byte);  // sending data to farther 74HC595
  shiftOut(data_pin, clock_pin, MSBFIRST, low_Byte);   // sending data to closer 74HC595
  digitalWrite(latch_pin, HIGH);                       // pull up "latch pin" after sending data

  // wait for display
  delay(50);
}
