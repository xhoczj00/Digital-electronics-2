/*
 * ---------------------------------------------------------------------
 * Author:      Martin Kosut, Jan Hocz
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2019-10-06
 * Last update: 2019-11-09
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Analog-to-digital conversion with displaying result on LCD and 
 *    transmitting via UART.
 * 
 * Note:
 *    Peter Fleury's UART library.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>             // itoa() function
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "lcd.h"
#include "uart.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Read ADC result and transmit via UART.
 *  Input:  None
 *  Return: None
 */
int main(void)
{
    // LCD display
    lcd_init(LCD_DISP_ON);

    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Display string without auto linefeed
    lcd_puts("LCD testing");
    
    // Put string to ringbuffer for transmitting via UART.
    uart_puts("UART testing\r\n");

    // ADC
    ADMUX &= ~(_BV(REFS1) | _BV(MUX0) | _BV(MUX1)| _BV(MUX2)| _BV(MUX3));
    ADMUX |= _BV(REFS0); 
    ADCSRA |= (_BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
    
	// Timer1
    TIM_config_prescaler(TIM1 ,TIM_PRESC_64);
    TIM_config_interrupt(TIM1 ,TIM_OVERFLOW_ENABLE);
    
	// Infinite loop
    for (;;) 
	{
    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Start ADC conversion.
 */
ISR(TIMER1_OVF_vect)
{
    ADCSRA |= _BV(ADSC);
}

/**
 *  Brief: ADC complete interrupt routine. Update LCD and UART 
 *         transmiter.
 */
ISR(ADC_vect)
{
    uint16_t value = 0;
    char uart_string[6];

	// Read 10-bit ACD value
    value = ADC;

    if(value < 90)
    {
        sprintf(uart_string, "RIGHT"); 
    }
        
    else if(value >= 90 && value <= 110)
    {
        sprintf(uart_string, "UP"); 
    }
    else if(value >= 235 && value <= 280)
    {
        sprintf(uart_string, "DOWN"); 
    }   
    else if(value >= 390 && value <= 430)
    {
        sprintf(uart_string, "LEFT"); 
    }  
    else if(value >= 620 && value <= 680)
    {
        sprintf(uart_string, "SELECT"); 
    }   
    else
        sprintf(uart_string, "No selection"); 


    lcd_clrscr();
    lcd_gotoxy(0,0);
	
	uart_puts("\r\n\033[4;32m");   
	uart_puts(uart_string);
	uart_putc(' ');
	uart_puts("\033[0m");  
	
    lcd_puts(uart_string);   
}