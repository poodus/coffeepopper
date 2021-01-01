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
// OLED Display

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// INPUT PINS
const int powerButtonPin = 2; // a momentary push button for toggling power on and off
const int heaterPotPin = A0; // potentiometer for controlling heater

// OUTPUT PINS
const int mainSSRPin = 8; // ssr for main power
const int heaterSSRPin = 6; // ssr for pwm'ing the heater coil

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
int millis_offset = 0;

int roasterOn = 0;
int powerButtonState = 0;

void setup() {
  pinMode(powerButtonPin, INPUT);
  pinMode(heaterSSRPin, OUTPUT);
  pinMode(mainSSRPin, OUTPUT);

  Serial.begin(9600);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.println(F("Hello, world!"));

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  display.clearDisplay();

  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
}
int firstOn = 1;
void loop() {
  updateRoasterPowerState(); // toggles roasterOn
  digitalWrite(mainSSRPin, roasterOn);

  if (roasterOn) {
    int reading = analogRead(heaterPotPin);
    Serial.println(reading);
    int mappedReading = map(reading, 0, 1023, 0, 255);
    analogWrite(heaterSSRPin, mappedReading);
    if (firstOn) {
      firstOn = 0;
      millis_offset = millis();
    }
  }
  else {
    firstOn = 1;
    analogWrite(heaterSSRPin, 0);
  }
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Temp = ");
  display.setTextSize(2);
  display.println(thermocouple.readFahrenheit());
  
  display.setTextSize(1);
  display.setCursor(0, 20);
  int seconds = (millis() - millis_offset) / 1000;
  int minutes = seconds / 60;
  seconds = seconds % 60;
  
  display.print("Time = ");
  display.print(minutes);
  display.print("min ");
  display.print(seconds);
  display.print("sec");
  display.display();
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
