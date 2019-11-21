/*
 * --------------------------------------------------------------------
 * Author:      Martin Kosut, Jan Hocz									
 *             Dept. of Radio Electronics, Brno Univ. of Technology	
 * Created:     2019-11-20												
 * Last update: 2019-11-20												
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2					
 * --------------------------------------------------------------------
 * Description: Communication with GPS module; data -> PC; data -> Nokia 5110 display.
 * TODO: Make it work.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include "nokia5110.h"



/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/

int main(void)
{
    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_write_string("const char *str", 3);
    /* Infinite loop */
    for (;;)
    {
       
	    {
             
        }
       
    }
//
    return (0);
}
