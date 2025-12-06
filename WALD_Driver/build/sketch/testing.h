#line 1 "D:\\Documents\\GitHub\\WALD\\WALD_Driver\\testing.h"
#ifndef TESTING_H
#define TESTING_H
#include <Arduino.h>

class TEST_UNIT 
{
public:
    /**
     * Print out values of VARIABLE RESISTOR like potentiometer on screen,
     * @param pin Analog pin on board
     * @return value of RV [0-1024]
     */
    static int TEST_RV(uint8_t pin);

    /**
     * Print out status of switch
     * @param pin Digital pin on board
     * @return state of switch
     */
    static bool TEST_SW(uint8_t pin);

};



#endif