#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

/* TIMER, countdown of each period [ms] */

// unsigned long tr_periods_ms[4] = {
//     15 * 60UL * 1000UL, //0 led
//     30 * 60UL * 1000UL, //1 led
//     90 * 60UL * 1000UL, //2 led 
//     150 * 60UL * 1000UL //3 led
//     //period 4 - infinity
// };

//TESTING
unsigned long tr_periods_ms[4] = {
    10 * 1000UL, //0 led
    20 * 1000UL, //1 led
    30 * 1000UL, //2 led 
    40 * 1000UL, //3 led
    //period 4 - infinity
};

constexpr unsigned long tr_sleep_mode_ms = 4 * 1000UL; // how long UI leds will be powered to avoid distracting sex and other plays in a bed :) Maybe i will find a girlfriend in the future, However I dont believe in that until studies, adn even during studies in city. ;9

constexpr byte rv_min_sensitivity = 3; //default: 3
constexpr byte rv_max_sensitivity = 65; //default 50
constexpr int ld_mono_breathe_anim_max_time_ms = 20000; // maximal time of animation breath


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


#endif // PIN_CONFIG_H