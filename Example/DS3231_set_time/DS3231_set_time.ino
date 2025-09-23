/*
DS3231_set.pde
Eric Ayars
4/11

Test of set-time routines for a DS3231 RTC

*/

#include <DS3231.h>
#include <Wire.h>

DS3231 myRTC;



void setup() {
    // Start the serial port
    Serial.begin(57600);
    
    // Start the I2C interface
    Wire.begin();
}

void loop() {
    

        
        myRTC.setClockMode(false);  // set to 24h
        //setClockMode(true); // set to 12h

        int year = 25;
        int month = 4;
        int date = 27;
        int dow = 7;

        int hour = 0;
        int minute = 32;
        int second = 25;

        myRTC.setYear(year);
        myRTC.setMonth(month);
        myRTC.setDate(date);
        myRTC.setDoW(dow);
        myRTC.setHour(hour);
        myRTC.setMinute(minute);
        myRTC.setSecond(second);
        
        // Test of alarm functions
        // set A1 to one minute past the time we just set the clock
        // on current day of week.
        myRTC.setA1Time(dow, hour, minute+1, second, 0x0, true,
                        false, false);
        // set A2 to two minutes past, on current day of month.
        myRTC.setA2Time(date, hour, minute+2, 0x0, false, false,
                        false);
        // Turn on both alarms, with external interrupt
        myRTC.turnOnAlarm(1);
        myRTC.turnOnAlarm(2);
        
    
    delay(1000);
}