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
#include <stdbool.h>
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
    //nokia_lcd_write_string("aaaaaa", 1);
    
    /* Infinite loop */
    char i = 0,j = 0;

    for (;;)
    {
        for(int k = 0; k<84;k++)
        {
            nokia_lcd_set_pixel(i, j, 1);
            _delay_ms(100);
       
	        nokia_lcd_render();
            i++;
        }
        for(int k = 0; k<48;k++)
        {
            nokia_lcd_set_pixel(i, j, 1);
            _delay_ms(100);
       
	        nokia_lcd_render();
            j++;
        }
        
       
        

       
    }
//
    return (0);
}
