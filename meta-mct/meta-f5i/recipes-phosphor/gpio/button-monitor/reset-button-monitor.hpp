#pragma once

#include "util.hpp"

typedef struct{
    unsigned int buttonValue = 0;
    unsigned int gpioBase = 0;
    unsigned int gpioE2 = 0;
    unsigned int counter = 0;
    bool buttonStatus = false;
    double start, finish, wait, duration;
}Button;

/*
 * @desc Button's action when button press
 * @param button - intput button with parameter
 * @return null
 */
void buttonPress(Button *button);

/*
 * @desc Button's action when button release
 * @param button - intput button with parameter
 * @return null
 */
void buttonRelease(Button *button);

/*
 * @desc Button's action when button timeout for a period
 * @param button - intput button with parameter
 * @return null
 */
void buttonWaiting(Button *button);