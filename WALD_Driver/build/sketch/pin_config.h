#line 1 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\pin_config.h"
#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

/* Controlling 8bit register sn74hc595n */
constexpr uint8_t SR_DATA = 4; // dataPin serial in
constexpr uint8_t SR_CLOCK = 7; // shift register clock
constexpr uint8_t SR_LATCH = 8; // storage regsiter clock

/*
    LEDs of timer:
    description - [bit of register]
    15min  - 0
    30min  - 1
    1h     - 2
    2h     - 3
    rgb ch - 4
    monoch - 5
*/

/* End of controls */

constexpr uint8_t SW_1_MONO = 11; //  one color leds. Extra: Press for 3 sec to change transition mode
constexpr uint8_t SW_2_T = 12; // set timer to turn off autoamtically.
constexpr uint8_t SW_3_RGB = 2; // A2 analog, rgb leds. Extra: Press for 3 sec to change transition mode

constexpr uint8_t LD_RED = 5; // pin to control mosfet responsible for red light
constexpr uint8_t LD_GREEN = 6; // pin to control mosfet responsible for blue light
constexpr uint8_t LD_BLUE = 9; // pin to control mosfet responsible for green light
constexpr uint8_t LD_MONO = 3; // mono channel leds, 

constexpr uint8_t RV_RGB = 0; // pin A0 to measure rgb analog potentiometer
constexpr uint8_t RV_BRIGHTNESS = 1; // pin A1 to measure brightness analog potentiometer


void setup_pins()
{
    pinMode(SR_DATA, OUTPUT);
    pinMode(SR_CLOCK, OUTPUT);
    pinMode(SR_LATCH, OUTPUT);

    pinMode(SW_1_MONO, INPUT_PULLUP); 
    pinMode(SW_2_T, INPUT_PULLUP);
    //pinMode(SW_3_RGB, INPUT_PULLUP); //cut out because it is now analog input

    pinMode(LD_RED, OUTPUT);
    pinMode(LD_GREEN, OUTPUT);
    pinMode(LD_BLUE, OUTPUT);
    pinMode(LD_MONO, OUTPUT);

}

#endif // PIN_CONFIG_H