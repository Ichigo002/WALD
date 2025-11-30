#include "testing.h"

int TEST_UNIT::TEST_RV(uint8_t pin)
{
    int v = analogRead(pin);
    Serial.print("TEST_UNIT::TEST_RV | Pin: ");
    Serial.print(pin);
    Serial.print(" | value = ");
    Serial.println(v);
    return v;
}

bool TEST_UNIT::TEST_SW(uint8_t pin)
{
    bool v = digitalRead(pin);
    Serial.print("TEST_UNIT::TEST_SW | Pin: ");
    Serial.print(pin);
    Serial.print(" | value = ");
    Serial.println(v);
    return v;
}
