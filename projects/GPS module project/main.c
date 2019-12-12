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
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "nokia5110.h"
#include "uart.h"
#include "pa6h_gps.h"
#include "timer.h"
//#include "softuart.h"

#define UART_BAUD_RATE 9600
#define LED_PIN0     PD7
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

typedef enum 
{
    START_STATE = 1,
	NOGPS_STATE,
    NEWDATA_STATE,
    IDLE_STATE
} state_t;

char rcv_data[500] = "$GPRMC,080553.000,A,4913.6585,N,01634.4322,E,2.82,71.74,271119,,,A*53\r\n$GPVTG,71.74,T,,M,2.82,N,5.23,K,A*04\r\n$GPGGA,080557.000,4913.6594,N,01634.4352,E,1,8,1.15,283.6,M,43.5,M,,*52\r\n$GPGSA,A,3,01,03,23,11,19,17,,,,,,,2.33,2.13,0.94*00\r\n$GPGSV,3,1,12,01,75,146,25,03,64,276,30,11,54,185,29,17,34,303,32*78\r\n$GPGSV,3,2,12,23,28,205,22,31,24,092,22,19,21,319,28,40,17,124,*77\r\n$GPGSV,3,3,12,08,03,182,,09,01,213,,22,,,20,14,,,21*76\r\n";
//char transmit_data[200];
char Latitude[11];
char Longitude[11];
T_GPS_data curr_data;
int i = 0 ,j = 0, k = 0;
volatile bool received_frame = false;



state_t current_state = START_STATE;
void initialization(void);
void display_data(void);
extern asm();

int main(void)
{
	for(;;)
	{
		switch (current_state) 
		{
			case START_STATE:
				initialization();
			//	char header[] ={"Time;Latitude;Longitude;Speed[kmh];Altitude;ActSats;\r\n"};
			//	softuart_puts(header);
				current_state = NOGPS_STATE;
				break;

			
			case NOGPS_STATE:
				//nokia_lcd_clear();
                nokia_lcd_set_cursor(10,16);
				nokia_lcd_write_string("NO GPS", 2);
				nokia_lcd_render();
                nokia_lcd_clear();
                current_state =  NEWDATA_STATE;//IDLE_STATE;
				break;

			
			case NEWDATA_STATE:
				nokia_lcd_clear();
				gps_get_data(&rcv_data[0],&curr_data);
				//parse_data(&curr_data);
				if(curr_data.valid == true)
				{
					display_data();
				}
				else
				{
					current_state = NOGPS_STATE;	
				}
				current_state = IDLE_STATE;
				break;
				
			case IDLE_STATE:
				//nokia_lcd_write_string(transmit_data,1);
				_NOP();
				//nokia_lcd_render();
				break;

			default:
				current_state = IDLE_STATE;
		}
	}
	
   return (0);
}
	
void initialization(void)
{
	//GPIO_config_output(&DDRD, LED_PIN0);
    DDRD|=_BV(LED_PIN0);
    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_write_string("1648",1);
//    
    nokia_lcd_render();
    _delay_ms(500);
    nokia_lcd_clear();
    nokia_lcd_render();
	PORTD &= ~(1<<LED_PIN0);
	TIM_config_prescaler(TIM2, TIM_PRESC_256);
	TIM_config_interrupt(TIM2, TIM_OVERFLOW_DISABLE);

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
 //   softuart_init();
//    softuart_turn_rx_on(); /* redundant - on by default */

	sei();
}
	
void display_data(void)
{
	k = 0;
	nokia_lcd_set_cursor(0,0);  // time
	for(j = 0; j < 6; j++)
	{
		if(curr_data.time[j] != '\0')
		{
			nokia_lcd_write_char(curr_data.time[j],1);
			//transmit_data[k++] = curr_data.time[j];
			if(j%2 ==1 && j!=5)
			{
				nokia_lcd_write_char(':',1);
				//transmit_data[k++] = ':';
			}
		}
		
	}
    //transmit_data[k++] = ';';
	nokia_lcd_set_cursor(0,9);  //Latitude
	nokia_lcd_write_char(curr_data.lat_dir,1);
    //transmit_data[k++] = curr_data.lat_dir;
	sprintf(Latitude,"%f",curr_data.latitude_deg);   
	for(j = 0;j<9;j++)
	{
        if(Latitude[j] != '\0')
		{
		    nokia_lcd_write_char(Latitude[j],1);
            //transmit_data[k++] = Latitude[j];
        }    
	}
	
    //transmit_data[k++] = ';';
	nokia_lcd_set_cursor(0,18); // printing longitude
	nokia_lcd_write_char(curr_data.lon_dir,1);
    //transmit_data[k++] = curr_data.lon_dir;
	sprintf(Longitude,"%f",curr_data.longitude_deg);    
	for(j = 0;j<9;j++)
	{
        if(Longitude[j] != '\0')
		{
		    nokia_lcd_write_char(Longitude[j],1);
            //transmit_data[k++] = Longitude[j];
        }  
	}
	
	//transmit_data[k++] = ';';
	nokia_lcd_set_cursor(0,27); 
	nokia_lcd_write_string("Speed:",1);
	for(j = 0;j<4;j++)
	{
		if(curr_data.speed_kmh[j] != '\0')
		{   
            nokia_lcd_write_char(curr_data.speed_kmh[j],1);
            //transmit_data[k++] = curr_data.speed_kmh[j];
        }  
	}
	//transmit_data[k++] = ';';
	nokia_lcd_set_cursor(72,27); 
	nokia_lcd_write_string("AS",1);

	nokia_lcd_set_cursor(0,36); 
	nokia_lcd_write_string("Alt:",1);
	for(j = 0;j<5;j++)
	{
        if(curr_data.altitude[j] != '\0')
		{    
		    nokia_lcd_write_char(curr_data.altitude[j],1);
            //transmit_data[k++] = curr_data.altitude[j];
        }      
	}
	
    //transmit_data[k++] = ';';
	nokia_lcd_set_cursor(74,36); 
	for(j = 0;j<1;j++)
	{
        if(curr_data.num_of_act_sats[j] != '\0')
		{    
		    nokia_lcd_write_char(curr_data.num_of_act_sats[j],1);
            //transmit_data[k++] = curr_data.num_of_act_sats[j];
        }      
	}
    //transmit_data[k++] = ';';
    //transmit_data[k++] = '\r';
   // transmit_data[k++] = '\n';
   // transmit_data[k++] = '\0';
  // nokia_lcd_write_string(rcv_data,1);
	nokia_lcd_render();
//	softuart_puts(transmit_data);    // "implicit" PSTR
	//softuart_puts( "--\r\n" );  // string "from RAM"
   //received_frame = false;
	 //  PORTD &= ~(1<<LED_PIN0);
}
	
	
	
	
	
	
	
   

ISR(USART_RX_vect)
{
	TCNT2 = 0;
    //GPIO_write(&DDRD,LED_PIN0,0);
    PORTD &= ~(1<<LED_PIN0);
	rcv_data[i++] = UDR0;
    TIM_config_interrupt(TIM2, TIM_OVERFLOW_ENABLE);
	
}
ISR(TIMER2_OVF_vect)
//ISR(TIMER1_OVF_vect)
{
	i = 0;
    //rcvd_frame = true;
    //if(rcv_data[0] != 0)
    
        

    //GPIO_write(&DDRD,LED_PIN0,1);
    PORTD |= (1<<LED_PIN0);
    current_state = NEWDATA_STATE;
    
   TIM_config_interrupt(TIM2, TIM_OVERFLOW_DISABLE);
}