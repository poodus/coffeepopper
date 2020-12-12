# coffeepopper
Arduino-controlled coffee roaster

Making a decent small-batch coffee roaster out of a popcorn popper.

The current configuration uses two Solid State Relays (SSRs) - one to 
turn the roaster on, and one for PWM'ing the heater coil. You don't
need an SSR for the on/off relay, it's just what I had around.

These particular SSR's have an input LED which is great for doing a dry run
and confirming that your power button and heater potentiometer are wired correctly.

Next: 
add a K-type thermocouple to measure approximate bean temperature
Add a display to read temperature
Experiment with PID controller in software

PARTS:

'Nostalgia' coffee roaster from Sweet Maria's
This is a decent and cheap option, but many popcorn poppers have similar internals
https://www.sweetmarias.com/nostalgia-electric-popcorn-popper.html

Solid State Relays
SSR-40 DA 3
3-32VDC input to control 24-380VAC
Fotek SSR Series 
https://www.sparkfun.com/products/13015

Arduino Uno or similar micro.