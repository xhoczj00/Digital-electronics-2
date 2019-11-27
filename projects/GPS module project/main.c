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
#include "uart.h"

#define UART_BAUD_RATE 9600


/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/


int main(void)
{
    uint8_t S,i;
    uint8_t COMM[85];
    
    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_write_picture();
    nokia_lcd_render();
     _delay_ms(1000);
     nokia_lcd_clear();
     nokia_lcd_render();
    /* Infinite loop */
    //char i = 0,j = 0;
    
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    uart_puts("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");

    for (;;)
    {
     S = uart_getc();
     if (S == '$')
     {
        for(i = 0;i<85;i++)
        {
            COMM[i] = uart_getc();
            nokia_lcd_write_char(COMM[i], 1);
            nokia_lcd_render();
        }
        
        nokia_lcd_clear();
        nokia_lcd_render();
     }
      /*  for(int k = 0; k<84;k++)
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
        */
       
        

       
    }
//
    return (0);
}
