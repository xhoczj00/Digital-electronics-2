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
#include "timer.h"
#include "softuart.h"

#define UART_BAUD_RATE 9600
#define LED_PIN0     PD4

typedef enum 
{
    START_STATE = 1,
	NOGPS_STATE,
    NEWDATA_STATE,
    IDLE_STATE
} state_t;

char rcv_data[500] = "$GPRMC,080553.000,A,4913.6585,N,01634.4322,E,2.82,71.74,271119,,,A*53\r\n$GPVTG,71.74,T,,M,2.82,N,5.23,K,A*04\r\n$GPGGA,080557.000,4913.6594,N,01634.4352,E,1,8,1.15,283.6,M,43.5,M,,*52\r\n$GPGSA,A,3,01,03,23,11,19,17,,,,,,,2.33,2.13,0.94*00\r\n$GPGSV,3,1,12,01,75,146,25,03,64,276,30,11,54,185,29,17,34,303,32*78\r\n$GPGSV,3,2,12,23,28,205,22,31,24,092,22,19,21,319,28,40,17,124,*77\r\n$GPGSV,3,3,12,08,03,182,,09,01,213,,22,,,20,14,,,21*76\r\n";
char Latitude[11];
char Longitude[11];
T_GPS_data curr_data;
int j = 0;
volatile bool received_frame = false;



state_t current_state = START_STATE;


int main(void)
{
	for(;;)
	{
		switch (current_state) 
		{
			case START_STATE:
				initialization();
				current_state = NOGPS_STATE;
				break;

			
			case NOGPS_STATE:
				//nokia_lcd_clear();
				nokia_lcd_write_string("NO SIGNAL", 2);
				nokia_lcd_render();
				break;

			
			case NEWDATA_STATE:
				gps_get_data(&rcv_data[0]);
				parse_data(&curr_data);
				
				current_state = IDLE_STATE;
				break;
				
			case IDLE_STATE:
				break;

			default:
				current_state = IDLE_STATE;
		}
	}
	
   return (0);
}
	
void initialization(void)
{
	GPIO_config_output(&DDRD, LED_PIN0);
    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_write_string("1648",1);
//    
    nokia_lcd_render();
    _delay_ms(500);
    nokia_lcd_clear();
    nokia_lcd_render();
	PORTD |= (1<<LED_PIN0);
	TIM_config_prescaler(TIM1, TIM_PRESC_256);
	TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

	//uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    softuart_init();
    softuart_turn_rx_on(); /* redundant - on by default */
	sei();
}
	
void display_data(void)
{
	int i = 0;
	nokia_lcd_set_cursor(0,0);  // time
	for(j = 0; j < 6; j++)
	{
		if(curr_data.time[j] != '\0')
		{
			nokia_lcd_write_char(curr_data.time[j],1);
			rcv_data[i++] = curr_data.time[j];
			if(j%2 ==1 && j!=5)
			{
				nokia_lcd_write_char(':',1);
				rcv_data[i++] = ':';
			}
		}
		
	}
    rcv_data[i++] = ';';
	nokia_lcd_set_cursor(0,9);  //Latitude
	nokia_lcd_write_char(curr_data.lat_dir,1);
    rcv_data[i++] = curr_data.latitude_dir;
	sprintf(Latitude,"%f",curr_data.latitude_deg);   
	for(j = 0;j<9;j++)
	{
        if(curr_data.latitude_deg[j] != '\0')
		{
		    nokia_lcd_write_char(Latitude[j],1);
            rcv_data[i++] = curr_data.latitude_deg[j];
        }    
	}
	
    rcv_data[i++] = ';';
	nokia_lcd_set_cursor(0,18); // printing longitude
	nokia_lcd_write_char(curr_data.lon_dir,1);
    rcv_data[i++] = curr_data.lon_dir;
	sprintf(Longitude,"%f",curr_data.longitude_deg);    
	for(j = 0;j<9;j++)
	{
        if(curr_data.latitude_deg[j] != '\0')
		{
		    nokia_lcd_write_char(Longitude[j],1);
            rcv_data[i++] = curr_data.longitude_deg[j];
        }  
	}
	
	rcv_data[i++] = ';';
	nokia_lcd_set_cursor(0,27); 
	nokia_lcd_write_string("Speed:",1);
	for(j = 0;j<4;j++)
	{
		if(curr_data.speed_kmh[j] != '\0')
		{   
            nokia_lcd_write_char(curr_data.speed_kmh[j],1);
            rcv_data[i++] = curr_data.speed_kmh[j];
        }  
	}
	rcv_data[i++] = ';';
	nokia_lcd_set_cursor(72,27); 
	nokia_lcd_write_string("AS",1);

	nokia_lcd_set_cursor(0,36); 
	nokia_lcd_write_string("Alt:",1);
	for(j = 0;j<5;j++)
	{
        if(curr_data.altitude[j] != '\0')
		{    
		    nokia_lcd_write_char(curr_data.altitude[j],1);
            rcv_data[i++] = curr_data.altitude[j];
        }      
	}
	
    rcv_data[i++] = ';';
	nokia_lcd_set_cursor(74,36); 
	for(j = 0;j<1;j++)
	{
        if(curr_data.num_of_act_sats[j] != '\0')
		{    
		    nokia_lcd_write_char(curr_data.num_of_act_sats[j],1);
            rcv_data[i++] = curr_data.altitude[j];
        }      
	}
    rcv_data[i++] = ';';
  // nokia_lcd_write_string(rcv_data,1);
	nokia_lcd_render();
	softuart_puts(rcv_data);    // "implicit" PSTR
	softuart_puts( "--\r\n" );  // string "from RAM"
   //received_frame = false;
	   PORTD &= ~(1<<LED_PIN0);
}
	
	
	
	
	
	
	
    
	//buffer = return_buffer_ptr();
	/*for(j = 8; j<36;j+9)
	{
		for(k = 0;k<84;k++)
		{
			nokia_lcd_set_pixel(k,j,1);
		}
	}
	*/
	//parse_data(&curr_data);

    //gps_get_data(&rcv_data[0]);
    //parse_data(&curr_data);
    
     /*   
	for(;;)
    {
        
        if(received_frame == true)
        {
            
            

        }
    }
	

    return (0);*/
}

ISR(USART_RX_vect)
{
	TCNT0 = 0;
    //GPIO_write(&DDRD,LED_PIN0,0);
    PORTD &= ~(1<<LED_PIN0);
	rcv_data[i++] = UDR0;
    //TIM_config_interrupt(TIM0, TIM_OVERFLOW_ENABLE);
	
}
//ISR(TIMER2_OVF_vect)
ISR(TIMER1_OVF_vect)
{
	i = 0;
    //rcvd_frame = true;
    //TIM_config_interrupt(TIM0, TIM_OVERFLOW_DISABLE);
    
    //if(rcv_data[0] != 0)
    {
        

            //GPIO_write(&DDRD,LED_PIN0,1);
        PORTD |= (1<<LED_PIN0);
        current_state = NEWDATA_STATE;
    }
   
}