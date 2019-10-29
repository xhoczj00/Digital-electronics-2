    /*
 * ---------------------------------------------------------------------
 * Author:      Jan Hocz, Martin Kosut
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-09
 * Last update: 2019-10-09
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gpio.h"
#include "timer.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_PIN0     PB5
#define LED_PIN1     PB0
#define BUTTON       PD2

/* Variables ---------------------------------------------------------*/
int i = 0;
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
  * Brief:  Main program. Interrupt configuration.
  * Input:  None
  * Return: None
  */
  


int main(void)
{
    GPIO_config_output(&DDRB, LED_PIN0);
    GPIO_config_output(&DDRB, LED_PIN1);
	
    TIM_config_prescaler(TIM0, TIM_PRESC_1024);
    TIM_config_interrupt(TIM0, TIM_OVERFLOW_ENABLE);

    TIM_config_prescaler(TIM1, TIM_PRESC_256);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    sei();
    
    /* Infinite loop */

    for (;;)
    {
       
    }
	
    return (0);
}

/**
  * Brief:  Toggle LEDs when timer 0, 1 overflows
  * Input:  None
  * Return: None
  */
ISR(TIMER0_OVF_vect)
{
    i++;
    if(i==10)
    {
        i=0;
        GPIO_toggle(&PORTB, LED_PIN1);
    }
}

ISR(TIMER1_OVF_vect)
{
    
    GPIO_toggle(&PORTB, LED_PIN0);
    
}
