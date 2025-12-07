#line 1 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
#include <Arduino.h>
#include "config.h"
#include "testing.h"
#include <jled.h>

int sw_set_time = -1; // update sw
bool sw_timer_reset_leds = false;
bool sw_t_cooldown = false; // switch

// timer variables to count down chosen time
unsigned long tr_now;
unsigned long tr_last_sleep = 0;
unsigned long tr_last = 0;
unsigned long tr_phase;
unsigned long tr_current_period;
bool tr_infinity = true;
bool tr_updated = false;
bool tr_activity = false;
bool tr_sleep_mode = false;
byte tr_bin_storage = 0;
// ==========

bool rgb_channel_active = false;

int rv_brightness_v[2]; // 0 - mono, 1 - rgb
int rv_rgb_v[2];
int rv_channel_updated = false;

int rv_mono_animation = false;
auto ld_mono_fade = JLed(LD_MONO).Forever();

// shift register for UI leds

byte sr_bin = 0;        // pwm ready byte for shift register to display
byte sr_bin_static = 0; // which are turned on

// simple soft pwm for UI leds

unsigned long sr_now;
unsigned long sr_last = 0;
unsigned long sr_phase;
unsigned long sr_leds_state = true;

//=============

/**
 * leds:
 * 0 - 15min
 * 1 - 30min
 * 2 - 45min
 * 3 - 90min
 *
 * 4 - rgb led
 * 5 - mono led
 */
#line 56 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void set_led(int led, bool value);
#line 62 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void HSVtoRGB(float H, float S, float V, byte &r, byte &g, byte &b);
#line 113 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
bool updateRVs(int index);
#line 138 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void updateSwitches();
#line 193 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void updateRVs();
#line 231 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void updateTimer();
#line 295 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void updateShiftRegister();
#line 331 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void setup_pins();
#line 347 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void setup();
#line 360 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void loop();
#line 56 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void set_led(int led, bool value)
{
    sr_bin_static &= ~(1 << led); // wyzeruj bit n
    sr_bin_static |= (value << led);
}

void HSVtoRGB(float H, float S, float V, byte &r, byte &g, byte &b)
{
    float C = V * S;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = V - C;

    float r1, g1, b1;

    if (H < 60)
    {
        r1 = C;
        g1 = X;
        b1 = 0;
    }
    else if (H < 120)
    {
        r1 = X;
        g1 = C;
        b1 = 0;
    }
    else if (H < 180)
    {
        r1 = 0;
        g1 = C;
        b1 = X;
    }
    else if (H < 240)
    {
        r1 = 0;
        g1 = X;
        b1 = C;
    }
    else if (H < 300)
    {
        r1 = X;
        g1 = 0;
        b1 = C;
    }
    else
    {
        r1 = C;
        g1 = 0;
        b1 = X;
    }

    r = (r1 + m) * 255;
    g = (g1 + m) * 255;
    b = (b1 + m) * 255;
}

// index - index from table of rv values 0 or 1
bool updateRVs(int index)
{
    bool updated = false;
    int x = analogRead(RV_BRIGHTNESS);
    byte y = abs(rv_brightness_v[index] - x);

    if (y > rv_min_sensitivity && y < rv_max_sensitivity)
    {
        rv_brightness_v[index] = x;
        updated = true;
        tr_activity = true;
    }

    x = analogRead(RV_RGB);
    y = abs(rv_rgb_v[index] - x);
    if (y > rv_min_sensitivity && y < rv_max_sensitivity)
    {
        rv_rgb_v[index] = x;
        updated = true;
        tr_activity = true;
    }

    return updated;
}

void updateSwitches()
{
    if (digitalRead(SW_1_MONO) == LOW)
    {
        rgb_channel_active = false;
        tr_activity = true;

        set_led(4, rgb_channel_active);
        set_led(5, !rgb_channel_active);
    }
    else if (analogRead(SW_3_RGB) < 512)
    {
        rgb_channel_active = true;
        tr_activity = true;

        set_led(4, rgb_channel_active);
        set_led(5, !rgb_channel_active);
    }

    if (digitalRead(SW_2_T) == LOW && sw_t_cooldown == false)
    {
        tr_activity = true;
        tr_infinity = false;
        sw_t_cooldown = true;
        sw_set_time += 1;

        if (sw_set_time > 3)
        {
            sw_set_time = -1;
            for (size_t i = 0; i < 4; i++)
            {
                set_led(i, false);
            }
            sw_timer_reset_leds = true;
            tr_infinity = true;
            tr_updated = true;
        }

        if (sw_set_time >= 0)
        {
            set_led(sw_set_time, true);

            tr_current_period = tr_periods_ms[sw_set_time];
            tr_updated = true;
        }
        delay(20);
    }

    if (digitalRead(SW_2_T) == HIGH)
    {
        sw_t_cooldown = false;
        delay(10);
    }
}

void updateRVs()
{
    // RV brightness
    if (rgb_channel_active)
    {
        /* if (updateRVs(1))
         {
             byte r, g, b;
             HSVtoRGB(rv_rgb_v[1], 1.0, rv_brightness_v[1] / 1023.0f, r, g, b);
             analogWrite(LD_RED, r);
             analogWrite(LD_GREEN, g);
             analogWrite(LD_BLUE, b);
         }*/
    }
    else if (updateRVs(0))
    {
        byte x = rv_brightness_v[0] / 4;
        int v = rv_rgb_v[0];

        if (v > rv_min_sensitivity) // animation
        {

            ld_mono_fade.Breathe(float(ld_mono_breathe_anim_max_time_ms) * (float(v) / 1024.0f));

            byte min_brght = 20;
            ld_mono_fade.MaxBrightness(x > min_brght ? x : min_brght + 5);

            ld_mono_fade.MinBrightness(min_brght);
        }
        else
        {
            ld_mono_fade.MaxBrightness(255);
            ld_mono_fade.MinBrightness(0);
            ld_mono_fade.Set(x);
        }
    }
}

void updateTimer()
{
    tr_now = millis();

    if (tr_activity)
    {
        if (tr_sleep_mode)
        {
            
            if (sw_timer_reset_leds)
            {
                sw_timer_reset_leds = false;

                sr_bin_static = tr_bin_storage;
                sr_bin_static = sr_bin_static ^ 0b00001111;
            }
            else
            {
                sr_bin_static = tr_bin_storage | sr_bin_static;
            }


            for (size_t i = 0; i < 8; i++)
            {
                Serial.print(bitRead(sr_bin_static, i));
            }
            Serial.println(" - sr_bin_static");

            for (size_t i = 0; i < 8; i++)
            {
                Serial.print(bitRead(tr_bin_storage, i));
            }
            Serial.println(" - tr_bin_storage");
        }
        tr_sleep_mode = false;
        tr_activity = false;
        tr_last_sleep = tr_now;
    }

    if (tr_now > tr_sleep_mode_ms + tr_last_sleep && !tr_sleep_mode)
    {
        tr_sleep_mode = true;
        // turn off UI leds
        tr_bin_storage = sr_bin_static;
        sr_bin_static = 0;
    }

    if (tr_infinity)
        return;

    if (tr_updated)
    {
        tr_updated = false;
        tr_last = tr_now;
    }

    if (tr_now > tr_current_period + tr_last)
    {
        // turn off mode
        Serial.print("It's over period [s]: ");
        Serial.println(tr_current_period / 1000);
    }
}

void updateShiftRegister()
{
    sr_now = micros();
    sr_phase = sr_now - sr_last;

    if (sr_phase < 2)
        return;

    sr_last = sr_now;

    for (size_t i = 0; i < 6; i++)
    {
        if ((sr_bin_static & (1 << i)) != 0) // i-bit is equal 1, so turn it on!!!
        {
            if (sr_leds_state) // switch on
            {
                sr_bin &= ~(1 << i); // wyzeruj bit n
                sr_bin |= (1 << i);
            }
            else // switch off
            {
                sr_bin &= ~(1 << i); // wyzeruj bit n
            }
        }
        else
        {
            sr_bin &= ~(1 << i);
        }
    }
    sr_leds_state = !sr_leds_state;

    digitalWrite(SR_LATCH, LOW);
    shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, sr_bin);
    digitalWrite(SR_LATCH, HIGH);
}

void setup_pins()
{
    pinMode(SR_DATA, OUTPUT);
    pinMode(SR_CLOCK, OUTPUT);
    pinMode(SR_LATCH, OUTPUT);

    pinMode(SW_1_MONO, INPUT_PULLUP);
    pinMode(SW_2_T, INPUT_PULLUP);
    // pinMode(SW_3_RGB, INPUT_PULLUP); //cut out because it is now analog input

    pinMode(LD_RED, OUTPUT);
    pinMode(LD_GREEN, OUTPUT);
    pinMode(LD_BLUE, OUTPUT);
    pinMode(LD_MONO, OUTPUT);
}

void setup()
{
    setup_pins();
    Serial.begin(9600);

    rgb_channel_active = false;
    set_led(5, true);

    rv_brightness_v[0] = rv_brightness_v[1] = analogRead(RV_BRIGHTNESS);
    rv_rgb_v[0] = rv_rgb_v[1] = analogRead(RV_RGB);
    ld_mono_fade.Set(rv_brightness_v[0] / 4);
}

void loop()
{
    updateSwitches();

    updateTimer();

    updateRVs();

    updateShiftRegister();

    ld_mono_fade.Update();
}

