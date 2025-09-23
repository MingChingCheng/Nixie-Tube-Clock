
#include "RotaryEncoder.h"
#include <Arduino.h>


#define PIN_RE_CLK 4
#define PIN_RE_DT 7
#define PIN_RE_SW 2

volatile int pos = 0;
volatile int new_pos = 0;

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// increment for every step is "1"
// couterclockwise(CCW): minus
// clockwise(CW): plus
RotaryEncoder encoder(PIN_RE_CLK, PIN_RE_DT, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
// increment for every step is "2"
// RotaryEncoder encoder(PIN_RE_CLK, PIN_RE_DT, RotaryEncoder::LatchMode::TWO03);

void reset_position()
{
  encoder.setPosition(0);
}

void setup()
{
  Serial.begin(115200);
  while (! Serial)
  {
    delay(10);  // wait untill serila port prepared
  };

  Serial.println("SimplePollRotator example for the RotaryEncoder library.");
  Serial.print("Pos: ");
  Serial.println(pos);
  
  // External interrupt for Arduino Uno: D2 and D3
  //                    for Arduino Nano: D2 and D3
  attachInterrupt(digitalPinToInterrupt(PIN_RE_SW), reset_position, FALLING);

} // setup()


// Read the current position of the encoder and print out when changed.
void loop()
{
  encoder.tick();

  new_pos = encoder.getPosition();
  if (pos != new_pos) {
    Serial.print("pos:");
    Serial.print(new_pos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    pos = new_pos;
  }

}

