// This code is designed with Arduino Nano
// Display time in 24-hour format on 4-digit Nixie tube


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

void change_mode();
void idle_check();

void poison();
void poison_check();

void set_time();
void set_hour_1();
void set_hour_2();
void set_minute_1();
void set_minute_2();



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

// Mode
#define IDLE_TIME = 30000;   // 30 seconds
unsigned long idle_start_time;  // record the start time of idle

volatile enum CLOCK_MODE {
  SHOW_TIME,          // 0
  SHOW_TEMP,          // 1
  SHOW_HUMIDITY,      // 2
  SETTING_HOUR_1,     // 3
  SETTING_HOUR_2,     // 4
  SETTING_MINUTE_1,   // 5
  SETTING_MINUTE_2,   // 6
  POISON,             // 7
  NUMBER_OF_MODES
} clock_mode;


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
bool poison_mode_finished = false;

// Rotary Encoder
volatile int pos = 0;
volatile int new_pos = 0;
volatile int state = 0;
RotaryEncoder encoder(rotary_clock_pin, rotary_data_pin, RotaryEncoder::LatchMode::FOUR3);





void setup() {

  // Define pin mode
  pinMode(nixie_brightness_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  

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
  attachInterrupt(digitalPinToInterrupt(rotary_switch_pin), change_mode, FALLING);
}

void loop() {

  switch(clock_mode) {

    case SHOW_TIME:
      show_time();
      break;

    case SHOW_TEMP:
      show_temp();
      break;

    case SHOW_HUMIDITY:
      show_humidity();
      break;

    case SETTING_HOUR_1:
      // TODO: setting hour tens
      break;

    case SETTING_HOUR_2:
      // TODO: setting hour ones
      break;

    case SETTING_MINUTE_1:
      //TODO: setting minute tens
      break;

    case SETTING_MINUTE_2:
      // TODO: setting minute ones
      break;

    case POISON:
      poison();
      break;
      
    default:
      // change to default mode
      clock_mode = SHOW_TIME;
      break;

  } // end switch


  poison_check();
  idle_check();
  
  delay(10);

} // end loop


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

void change_mode() {
  // change to next mode
  // if at the last mode, go back to the first mode
  clock_mode = (clock_mode + 1) % NUMBER_OF_MODES;

  // reset idle timer
  if (clock_mode != SHOW_TIME) {
    idle_start_time = millis();
  }

}

void idle_check() {
  // check if in idle time
  if (clock_mode != SHOW_TIME) {
    unsigned long current_time = millis();
    if (current_time - idle_start_time >= IDLE_TIME) {
      clock_mode = SHOW_TIME;
    }
  }
}

void poison() {

  // loop through digits 0-9
  int i;
  for ( i=0 ; i<10 ; i++ ) {
    display(i, i, i, i);
    delay(500);
  }

  // return to show time mode
  clock_mode = SHOW_TIME;
}

void poison_check() {

  // Go to poison mode at every hour

  // check if already ran poison mode this hour
  if (poison_mode_finished == true) {
    return;
  }

  
  if ( minute == 0) {
    poison_mode_finished = true;
    clock_mode = POISON;
  }

  if ( minute != 0) {
    poison_mode_finished = false;
  }
}