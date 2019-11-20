/***********************************************************************
 * 
 * Implementation of LFSR-based (Linear Feedback Shift Register) 
 * pseudo-random generator in AVR assembly.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-2019 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include "gpio.h"



/* Function prototypes -----------------------------------------------*/
void fsm_random(void);
extern uint8_t generator_asm(uint8_t current);


/* Main --------------------------------------------------------------*/
/* Generate a sequence of LFSR preudo-random values using 4- and 8-bit
 * structure. */
int main(void)
{
    GPIO_config_output(&DDRB , PB5);
    for (;;) 
    {
      generator_asm(3) ; 
      
    }

    return (0);
}
