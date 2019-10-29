    /*
 * ---------------------------------------------------------------------
 * Author:      Jan Hocz, Martin Kosut                      
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-23
 * Last update: 2019-10-29
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "timer.h"
#include "segment.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_D1          PB5
#define BTN_S1          PC1 // PCINT 9
#define BTN_S2          PC2 // PCINT 10
#define BTN_S3          PC3 // PCINT 11

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Shows decimal values ​​on 7-segment display.
 *  Input:  None
 *  Return: None
 */
int i,c = 0;
int main(void)
{
    /* D1 led */
    GPIO_config_output(&DDRB , LED_D1);
    
	/* Pin Change Interrupts 11:9 */
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT11);
	/* 4ms Timer0 overflow enable*/
    TIM_config_prescaler(TIM0, TIM_PRESC_256);
    TIM_config_interrupt(TIM0, TIM_OVERFLOW_ENABLE);
    
	/* 7-segment display interface */
    GPIO_config_output(&DDRB , SEGMENT_DATA );
    GPIO_config_output(&DDRD , SEGMENT_CLK );
    GPIO_config_output(&DDRD , SEGMENT_LATCH );

    /* Enable interrupts by setting the global interrupt mask */
    sei();

    /* Infinite loop */
    for (;;) 
	{
        SEG_putc(i, c); 
    }

    return (0);
}

/**
 *  Brief: Pin Change Interrupt 11:9 routine. Toggle a LED.
 */
ISR(PCINT1_vect)
{
    GPIO_toggle(&PORTB, LED_D1);
	
	//count 0-9
    i++;
    if(i == 10) 
	{
		i = 0;
	}
}

/**
 *  Brief: Pin Change Interrupt 11:9 routine. Toggle a LED.
 */
ISR(TIMER0_OVF_vect)	//multiplexing display
{
	//count 0-3
    c++;
    if(c == 4)
    {
        c = 0;
    }
}
