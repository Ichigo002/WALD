#line 1 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
#include <Arduino.h>
#include "config.h"
#include "testing.h"
#include <jled.h>

int sr_bin = 0;

int set_time = -1;
bool sw_t_cooldown = false;

bool rgb_channel_active = false;

int rv_brightness_v[2]; // 0 - mono, 1 - rgb
int rv_rgb_v[2];
int rv_channel_updated = false;

int rv_mono_animation = false;
auto ld_mono_fade = JLed(LD_MONO).Forever();

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
#line 30 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void set_led(int led, bool value);
#line 36 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void updateRegister();
#line 43 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void HSVtoRGB(float H, float S, float V, byte &r, byte &g, byte &b);
#line 94 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
bool updateRVs(int index);
#line 117 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void handleSwitches();
#line 168 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void handleRVs();
#line 206 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void setup_pins();
#line 222 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void setup();
#line 237 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void loop();
#line 30 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\WALD_Driver.ino"
void set_led(int led, bool value)
{
    sr_bin &= ~(1 << led); // wyzeruj bit n
    sr_bin |= (value << led);
}

void updateRegister()
{
    digitalWrite(SR_LATCH, LOW);
    shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, sr_bin);
    digitalWrite(SR_LATCH, HIGH);
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
    }

    x = analogRead(RV_RGB);
    y = abs(rv_rgb_v[index] - x);
    if (y > rv_min_sensitivity && y < rv_max_sensitivity)
    {
        rv_rgb_v[index] = x;
        updated = true;
    }

    return updated;
}

void handleSwitches()
{
    if (digitalRead(SW_1_MONO) == LOW)
    {
        rgb_channel_active = false;

        set_led(4, rgb_channel_active);
        set_led(5, !rgb_channel_active);

        updateRegister();
    }
    else if (analogRead(SW_3_RGB) < 512)
    {
        rgb_channel_active = true;

        set_led(4, rgb_channel_active);
        set_led(5, !rgb_channel_active);

        updateRegister();
    }

    if (digitalRead(SW_2_T) == LOW && sw_t_cooldown == false)
    {
        sw_t_cooldown = true;
        set_time += 1;

        if (set_time > 3)
        {
            set_time = -1;
            for (size_t i = 0; i < 4; i++)
            {
                set_led(i, false);
            }
        }

        if (set_time >= 0)
        {
            set_led(set_time, true);
        }

        updateRegister();
        delay(20);
    }

    if (digitalRead(SW_2_T) == HIGH)
    {
        sw_t_cooldown = false;
        delay(10);
    }
}

void handleRVs()
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

    updateRegister();
}

void loop()
{
    // TEST_UNIT::TEST_RV(RV_BRIGHTNESS);
    // TEST_UNIT::TEST_RV(RV_RGB); // BROKEN
    // TEST_UNIT::TEST_SW(SW_1_MONO);
    // TEST_UNIT::TEST_SW(SW_2_T);
    // TEST_UNIT::TEST_SW(SW_3_RGB);
    handleSwitches();
    handleRVs();

    ld_mono_fade.Update();
}

