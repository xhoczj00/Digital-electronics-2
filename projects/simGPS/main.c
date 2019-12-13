
/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2017-10-27
 * Last update: 2019-11-08
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Scan TWI bus for all connected slave devices and transmit info
 *    to UART.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>         // itoa() function
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/
const char message1[] = "$GPGGA,080714.000,4913.6237,N,01634.4839,E,1,8,1.15,276.1,M,43.5,M,,*52\r\n$GPGSA,A,3,01,22,14,03,23,11,19,17,,,,,1.45,1.15,0.88*0F\r\n$GPGSV,3,1,12,22,85,003,12,01,73,146,12,03,65,278,13,11,53,184,13*76\r\n$GPGSV,3,2,12,14,41,057,10,17,34,301,12,23,29,205,11,31,25,091,11*78\r\n$GPGSV,3,3,12,19,21,319,13,32,16,046,14,09,02,214,,08,02,181,*77\r\n$GPRMC,080714.000,A,4913.6237,N,01634.4839,E,0.61,186.44,271119,,,A*68\r\n$GPVTG,186.44,T,,M,0.61,N,1.14,K,A*31\r\n";
const char message2[] = "$GPGGA,080704.000,4913.6256,N,01634.4842,E,1,8,1.15,276.8,M,43.5,M,,*51\r\n$GPGSA,A,3,01,22,14,03,23,11,19,17,,,,,1.45,1.15,0.88*0F\r\n$GPGSV,4,1,13,22,85,003,13,01,73,146,18,03,65,278,15,11,53,184,15*7B\r\n$GPGSV,4,2,13,14,42,057,25,17,34,302,19,23,29,205,16,31,25,091,16*73\r\n$GPGSV,4,3,13,33,25,219,,19,21,319,15,32,16,046,23,08,02,181,*72\r\n$GPGSV,4,4,13,09,01,214,*44\r\n$GPRMC,080704.000,A,4913.6256,N,01634.4842,E,1.23,230.36,271119,,,A*6E\r\n$GPVTG,230.36,T,,M,1.23,N,2.28,K,A*31\r\n";
//"$GPGGA,235948.799,,,,,0,0,,,M,,M,,*4E\r\n$GPGSA,A,1,,,,,,,,,,,,,,,*1E\r\n$GPRMC,235948.799,V,,,,,0.00,0.00,050180,,,N*47\r\n$GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32\r\n";

volatile uint8_t send = 0;
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Test all TWI slave addresses and scan
 *          connected devices.
 *  Input:  None
 *  Return: None
 */
 int i = 0;
int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    uart_puts("GPS Simulation");
    /* Timer1 */
    TIM_config_prescaler(TIM1, TIM_PRESC_256);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART.
    // Infinite loop
    for (;;)
    {
        if(send == 1)
        {
            TIM_config_interrupt(TIM1, TIM_OVERFLOW_DISABLE);
            for(uint16_t i = 0; i < sizeof(message1); i++)
            {
                uart_putc(message1[i]);
            }
            send = 0;
            TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
        }
        else if (send == 2)
        {
            TIM_config_interrupt(TIM1, TIM_OVERFLOW_DISABLE);
            for(uint16_t i = 0; i < sizeof(message2); i++)
            {
                uart_putc(message2[i]);
            }
            send = 0;
            TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
        }
    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Update state of TWI Finite
 *         State Machine.
 */
ISR(TIMER1_OVF_vect)
{
    if(i == 0)
    {
        send = 1;
        i = 1;
    }
    else if(i == 1)
    {
        send = 2;
        i = 0;
    }
        

}