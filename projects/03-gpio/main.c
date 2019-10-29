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
#include <util/delay.h>
#include "gpio.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_PIN0     PB5
#define LED_PIN1     PB0
#define BUTTON       PD2
#define BLINK_DELAY 500

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
  * Brief:  Main program. Blink a LED with delay function.
  * Input:  None
  * Return: None
  */
int main(void)
{
    GPIO_config_output(&DDRB, LED_PIN0);
    GPIO_config_output(&DDRB, LED_PIN1);
    GPIO_config_input_pullup(&DDRD, &PORTD, BUTTON);
    
    /* Infinite loop */
    for (;;)
    {
        if(GPIO_read(&PIND, BUTTON) == 0)
        {
            GPIO_toggle(&PORTB, LED_PIN0);
            GPIO_toggle(&PORTB, LED_PIN1);
            _delay_ms(BLINK_DELAY);
        }
    }
	
    return (0);
}