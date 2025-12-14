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

const int NIXIE_BRIGHTNESS = 0;  // brightness level (0-255), 0 is the brightest
const int LED_BRIGHTNESS = 50;   // brightness level (0-255), 0 is the brightest
const int IDLE_TIME = 30000;     // 30 seconds

/* Define functions */
void blinking_nixie_tube(int duration_ms = 500);
void turn_on_nixie_tube(int brightness = NIXIE_BRIGHTNESS);
void turn_off_nixie_tube();


void show_time();
void show_temp();
void show_humidity();
void display(int a, int b, int c, int d);

void change_mode();
void idle_check();

void poison();
void poison_check();

void set_time();
void set_hour_tens();
void set_hour_ones();
void set_minute_tens();
void set_minute_ones();
int update_digit(int value, int direction, int max_value);

void led_set_color(int red, int green, int blue, int brightness = LED_BRIGHTNESS);


/* Define Pins */
// 74HC595
const int latch_pin = 7;
const int clock_pin = 4;
const int data_pin = 2;

// Nixie tube
const int nixie_brightness_pin = 6;

// LED
const int led_brightness_pin = 13;
const int led_red_pin = 11;
const int led_green_pin = 10;
const int led_blue_pin = 9;

// I2C
const int SCL_pin = A5;
const int SDA_pin = A4;

// Rotary encoder
const int rotary_switch_pin = 3;
const int rotary_data_pin = A1;
const int rotary_clock_pin = A0;

// fan
const int fan_pin = 5;


/* Define Objects and global variables of all components */

// Mode
unsigned long idle_start_time;  // record the start time of idle
bool poison_mode_finished = false;

enum CLOCK_MODE {
  SHOW_TIME,             // 0
  SHOW_TEMP,             // 1
  SHOW_HUMIDITY,         // 2
  SETTING_HOUR_TENS,     // 3
  SETTING_HOUR_ONES,     // 4
  SETTING_MINUTE_TENS,   // 5
  SETTING_MINUTE_ONES,   // 6
  SET_TIME,              // 7
  NUMBER_OF_MODES
};
CLOCK_MODE clock_mode = SHOW_TIME;

// Humidity sensor
Adafruit_AM2320 am2320 = Adafruit_AM2320();
float humidity = 0;
float temperature = 0;

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
volatile int new_direction = 0;
volatile int last_direction = 0;
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
  myRTC.setClockMode(false);  // Real Time Clock mode = false: 24h, mode = true: 12h

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

    case SETTING_HOUR_TENS:
      set_hour_tens();
      break;

    case SETTING_HOUR_ONES:
      set_hour_ones();
      break;

    case SETTING_MINUTE_TENS:
      set_minute_tens();
      break;

    case SETTING_MINUTE_ONES:
      set_minute_ones();
      break;

    case SET_TIME:
      set_time();
      break;
      
    default:
      // change to default mode
      clock_mode = SHOW_TIME;
      break;

  } // end switch


  poison_check();
  idle_check();
  
  delay(50);

} // end loop


void turn_on_nixie_tube(int brightness) {
  // turn on the nixie tube 
  // and set brightness
  analogWrite(nixie_brightness_pin, brightness);
}

void turn_off_nixie_tube() {
  // turn off the nixie tube by
  // set brightness to min
  analogWrite(nixie_brightness_pin, 255);
}

void blinking_nixie_tube(int duration_ms) {
  // blinking nixie tube every duration

  // get current time
  unsigned long current_time = millis();

  if ( (current_time / duration_ms) % 2 == 0 ) {
    turn_on_nixie_tube();
  }
  else {
    turn_off_nixie_tube();
  }
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
  turn_on_nixie_tube();
  display(hour_tens, hour_ones, minute_tens, minute_ones);
}

void show_temp() {

  // get temp from AM2320
  temperature = am2320.readTemperature();


  // transform to digits
  int temperature_int = (int)(temperature * 100);

  int temperature_tens   = (int)(temperature_int / 1000 % 10);
  int temperature_ones   = (int)(temperature_int /  100 % 10);
  int temperature_p_ones = (int)(temperature_int /   10 % 10);
  int temperature_p_tens = (int)(temperature_int /    1 % 10);

  // output
  turn_on_nixie_tube();
  display(temperature_tens, temperature_ones, temperature_p_ones, temperature_p_tens);
}

void show_humidity() {

  // get humidity from AM2320
  humidity = am2320.readHumidity();

  // transform to digits
  int humidity_int = (int)(humidity * 100);

  int humidity_tens   = (int)(humidity_int / 1000 % 10);
  int humidity_ones   = (int)(humidity_int /  100 % 10);
  int humidity_p_ones = (int)(humidity_int /   10 % 10);
  int humidity_p_tens = (int)(humidity_int /    1 % 10);

  // output
  turn_on_nixie_tube();
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
}

void change_mode() {
  // change to next mode
  // if at the last mode, go back to the first mode
  clock_mode = (CLOCK_MODE)((clock_mode + 1) % NUMBER_OF_MODES);

  // reset idle timer
  if (clock_mode != SHOW_TIME) {
    idle_start_time = millis();
  }

  // initialize when entering setting mode
  if (clock_mode == SETTING_HOUR_TENS) {
    new_hour = hour;
    new_minute = minute;
  }

}

void idle_check() {
  // check if in idle time
  unsigned long current_time = millis();

  // if idle time exceeded, go back to show time mode
  if (clock_mode == SHOW_TEMP ||
      clock_mode == SHOW_HUMIDITY ) {
    if (current_time - idle_start_time >= IDLE_TIME) {
      clock_mode = SHOW_TIME;
    }
  }

  // if in setting mode and idle time exceeded, go to set time mode
  if (clock_mode == SETTING_HOUR_TENS ||
      clock_mode == SETTING_HOUR_ONES ||
      clock_mode == SETTING_MINUTE_TENS ||
      clock_mode == SETTING_MINUTE_ONES ) {
    if (current_time - idle_start_time >= IDLE_TIME) {
      clock_mode = SET_TIME;
    }
  }

}

void poison() {
  
  // Set the brightness to max
  turn_on_nixie_tube(0);

  // loop through digits 0-9
  int i;
  for ( i=0 ; i<10 ; i++ ) {
    display(i, i, i, i);
    delay(450);
  }

}

void poison_check() {

  // only run poison mode in show time mode
  if (clock_mode == SHOW_TIME) {
  
    // Go to poison mode at every hour
    if ( minute == 0 && poison_mode_finished == false) {
      poison_mode_finished = true;
      poison();
    }

    if ( minute != 0 ) {
      poison_mode_finished = false;
    }
  }
}

void set_time() {
  // set time to RTC
  myRTC.setHour(new_hour);
  myRTC.setMinute(new_minute);

  // retrieve time from RTC
  hour = myRTC.getHour(h12Flag, pmFlag);
  minute = myRTC.getMinute();

  int hour_tens = (int)(hour / 10);
  int hour_ones = (int)(hour % 10);
  int minute_tens = (int)(minute / 10);
  int minute_ones = (int)(minute % 10);

  display(hour_tens, hour_ones, minute_tens, minute_ones);

  // blink quickly to indicate time is set
  int i = 0;
  for ( i=0 ; i<2 ; i++ ) {
    turn_off_nixie_tube();
    delay(200);
    turn_on_nixie_tube();
    delay(200);
  }

  // return to show time mode
  clock_mode = SHOW_TIME;
}

void set_hour_tens() {

  // get old time from RTC
  int hour_tens = (int)(hour / 10);
  int hour_ones = (int)(hour % 10);
  int minute_tens = (int)(minute / 10);
  int minute_ones = (int)(minute % 10);

  int new_hour_tens = (int)(new_hour / 10);
  
  // read rotary encoder
  encoder.tick();
  new_direction = (int)(encoder.getDirection());
  
  // update hour tens based on rotary encoder direction
  if (new_direction != last_direction) {

    // reset idle timer
    idle_start_time = millis();
    
    // update hour tens
    // possible value: 00, 01, 02, ..., 23
    // max value is 2
    new_hour_tens = update_digit(hour_tens, new_direction, 2);

    last_direction = new_direction;
  }
  
  // display new time blinking every 500ms
  blinking_nixie_tube(500);
  
  // update new hour
  display(new_hour_tens, hour_ones, minute_tens, minute_ones);
  new_hour = new_hour_tens * 10 + hour_ones;
}

void set_hour_ones() {
  
  // get old time from RTC
  int hour_tens = (int)(hour / 10);
  int hour_ones = (int)(hour % 10);
  int minute_tens = (int)(minute / 10);
  int minute_ones = (int)(minute % 10);

  int new_hour_tens = (int)(new_hour / 10);
  int new_hour_ones = (int)(new_hour % 10);
  
  // read rotary encoder
  encoder.tick();
  new_direction = (int)(encoder.getDirection());
  
  // update hour tens based on rotary encoder direction
  if (new_direction != last_direction) {

    // reset idle timer
    idle_start_time = millis();

    if (new_hour_tens == 2) {
      // possible value: 20, 21, 22, 23
      // max value is 3
      new_hour_ones = update_digit(hour_ones, new_direction, 3);
    }
    else {
      // possible value: 00, 01, 02, ..., 09, 10, 11, ..., 19
      // max value is 9
      new_hour_ones = update_digit(hour_ones, new_direction, 9);
    }

    last_direction = new_direction;
  }
  
  // display new time blinking every 500ms
  blinking_nixie_tube(500);
  
  // update new hour
  display(new_hour_tens, new_hour_ones, minute_tens, minute_ones);
  new_hour = new_hour_tens * 10 + new_hour_ones;
}

void set_minute_tens() {

  // get old time from RTC
  int hour_tens = (int)(hour / 10);
  int hour_ones = (int)(hour % 10);
  int minute_tens = (int)(minute / 10);
  int minute_ones = (int)(minute % 10);

  int new_hour_tens = (int)(new_hour / 10);
  int new_hour_ones = (int)(new_hour % 10);
  int new_minute_tens = (int)(new_minute / 10);
  
  // read rotary encoder
  encoder.tick();
  new_direction = (int)(encoder.getDirection());
  
  // update hour tens based on rotary encoder direction
  if (new_direction != last_direction) {

    // reset idle timer
    idle_start_time = millis();
    
    // update minute tens
    // possible value: 00, 01, ..., 59
    // max value is 5
    new_minute_tens = update_digit(minute_tens, new_direction, 5);
    last_direction = new_direction;
  }
  
  // display new time blinking every 500ms
  blinking_nixie_tube(500);
  
  // update new hour
  display(new_hour_tens, new_hour_ones, new_minute_tens, minute_ones);
  new_minute = new_minute_tens * 10 + minute_ones;
}

void set_minute_ones() {
  
  // get old time from RTC
  int hour_tens = (int)(hour / 10);
  int hour_ones = (int)(hour % 10);
  int minute_tens = (int)(minute / 10);
  int minute_ones = (int)(minute % 10);

  int new_hour_tens = (int)(new_hour / 10);
  int new_hour_ones = (int)(new_hour % 10);
  int new_minute_tens = (int)(new_minute / 10);
  int new_minute_ones = (int)(new_minute % 10);

  // read rotary encoder
  encoder.tick();
  new_direction = (int)(encoder.getDirection());
  
  // update hour tens based on rotary encoder direction
  if (new_direction != last_direction) {

    // reset idle timer
    idle_start_time = millis();

    // update minute tens
    // possible value: 00, 01, ..., 59
    // max value is 9
    new_minute_ones = update_digit(minute_ones, new_direction, 9);
    last_direction = new_direction;
  }
  
  // display new time blinking every 500ms
  blinking_nixie_tube(500);
  
  // update new hour
  display(new_hour_tens, new_hour_ones, new_minute_tens, new_minute_ones);
  new_hour = new_minute_tens * 10 + new_minute_ones;
}

int update_digit(int value, int direction, int max_value) {
  
  // clockwise (CW): plus
  if (direction == 1) {
    return (value + 1) % (max_value + 1);
  }
  // counterclockwise (CCW): minus
  else if (direction == -1) {
    return (value - 1 + (max_value + 1)) % (max_value + 1);
  }
  // no rotation
  else {
    return value;
  }
}

void led_set_color(int red, int green, int blue, int brightness) {
  // set LED color with brightness control

  // set brightness
  analogWrite(led_brightness_pin, brightness);

  // set color
  analogWrite(led_red_pin, red);
  analogWrite(led_green_pin, green);
  analogWrite(led_blue_pin, blue);

}