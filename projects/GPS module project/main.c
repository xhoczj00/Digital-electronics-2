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
#include <stdio.h>
#include <stdlib.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "nokia5110.h"
#include "uart.h"
#include "pa6h_gps.h"
#include "gpio.h"

#define UART_BAUD_RATE 9600
#define LED_PIN0     PD4



int main(void)
{
    GPIO_config_output(&DDRD, LED_PIN0);
    GPIO_write(&DDRD,LED_PIN0,1);
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    //uart_init(9600);
    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_write_picture();
    nokia_lcd_render();
     _delay_ms(1000);
     nokia_lcd_clear();
    nokia_lcd_render();
    for(;;)
    {
    
        //nokia_lcd_set_cursor(0,0);
        gps_get_data();
        //nokia_lcd_render();
    }
   

    
    /*
    for (;;)
    {
     //S = uart_getc();
        for(int i = 0; i < 400; i++)
        {
            rcv_data[i] = uart_getc();
            if(rcv_data[i] == 0x0A)
                break;
            
        }
        //sprintf(rcv_data, rcv_data); 
        
        nokia_lcd_clear();
        nokia_lcd_set_cursor(0,0);
        nokia_lcd_write_string(rcv_data,1);
        
        nokia_lcd_render();

        _delay_ms(300);
     }
    */
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
       
        

       
    //}
//
    return (0);
}
