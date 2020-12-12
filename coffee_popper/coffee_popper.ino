#include "Adafruit_MAX31855.h"

/*
  CoffeePopper

  This is a simple controller for hacked 'Nostalgia' popcorn popper
  from Sweet Maria's.
  https://www.sweetmarias.com/nostalgia-electric-popcorn-popper.html

  The current configuration uses two SSR's - one to turn the roaster on,
  one for PWM'ing the heater coil.

  Debouncing code was adapted from Arduino sample code.

  2020-12-12

*/

// INPUT PINS
const int powerButtonPin = 2; // a momentary push button for toggling power on and off
const int heaterPotPin = A0; // potentiometer for controlling heater

// OUTPUT PINS
const int mainSSRPin = 7; // ssr for main power
const int heaterSSRPin = 9; // ssr for pwm'ing the heater coil

// POWER BUTTON
int lastPowerButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Thermocouple
#define MAXDO   5
#define MAXCS   4
#define MAXCLK  3

// Initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

int roasterOn = 0;
int powerButtonState = 0;

void setup() {
  pinMode(powerButtonPin, INPUT);
  pinMode(heaterSSRPin, OUTPUT);
  pinMode(mainSSRPin, OUTPUT);

  Serial.begin(9600);

  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
}

void loop() {
  updateRoasterPowerState(); // toggles roasterOn
  digitalWrite(mainSSRPin, roasterOn);

  if (roasterOn) {
    int reading = analogRead(heaterPotPin);
    int mappedReading = map(reading, 0, 1023, 0, 255);
    analogWrite(heaterSSRPin, mappedReading);
  }
  else {
    analogWrite(heaterSSRPin, 0);
  }

//  Serial.print("Int. Temp = ");
//  Serial.println(thermocouple.readInternal());
  Serial.print("Ext. Temp = ");
  Serial.println(thermocouple.readFahrenheit());
  

}

/*
   updateRoasterPowerState()

   This power button is a toggle to turn the roaster on and off.
   Check for debouncing, and if a valid press is detected,
   flip the roaster's power state.
*/
void updateRoasterPowerState() {

  int buttonReading = digitalRead(powerButtonPin);

  // If the button reading changed, due to noise or pressing:
  if (buttonReading != lastPowerButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  // if button has been down for long enough
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed:
    if (buttonReading != powerButtonState) {
      powerButtonState = buttonReading;

      // toggle state
      if (powerButtonState == HIGH) {
        roasterOn = !roasterOn;
      }
    }
  }
  lastPowerButtonState = buttonReading;
}
