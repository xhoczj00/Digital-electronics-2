    /*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-09-27
 * Last update: 2019-07-16
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description: Blink a LED with delay function.
 * TODO: Verify functionality of LED blinker.
 * NOTE: Macro _BV converts a bit number into a byte value (see AVR Libc
 * Reference Manual).
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>

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
    /* Set output pin */
    DDRB |= (1 << LED_PIN1) | (1 << LED_PIN0);          
    DDRD &= ~ (1 << BUTTON);
    /* Turn LED off */
    PORTB &= ~((1 << LED_PIN1) | (1 << LED_PIN0));   
    
    PORTD |= (1 << BUTTON);

    /* Infinite loop */
    for (;;)
    {
        loop_until_bit_is_clear(PIND , BUTTON);
        {
             PORTB ^= (1 << LED_PIN1) | (1 << LED_PIN0) ;
             _delay_ms(BLINK_DELAY);
        }
       
    }
//
    return (0);
}
