/*
 * --------------------------------------------------------------------
 * Author:      Martin Kosut, Jan Hocz									
 *              Dept. of Radio Electronics, Brno Univ. of Technology	
 * Created:     2019-11-20												
 * Last update: 2019-12-17												
 * Platform:    ATmega2560, AVR 8-bit Toolchain 3.6.2					
 * --------------------------------------------------------------------
 * Description: Communication with GPS module; data -> PC; data -> Nokia 5110 display.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "nokia5110.h"
#include "uart.h"
#include "pa6h_gps.h"
#include "timer.h"

#define UART_BAUD_RATE 9600
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

typedef enum 
{
    START_STATE = 1,
	NOGPS_STATE,
    NEWDATA_STATE,
    IDLE_STATE
} state_t;


char *rcv_data = NULL;
T_GPS_data *curr_data = NULL;
int i = 0 ,j = 0, k = 0;
state_t current_state;// = START_STATE;

void initialization(void);
void display_data(void);



int main(void)
{
	current_state = START_STATE;
	
	for(;;)
	{
		switch (current_state)
		{
			case START_STATE:
				initialization();
				rcv_data = (char *)malloc(sizeof(char) * 600);
				uart1_puts("Time,Latitude,Longitude,Speed[km/h],Altitude[m],Active_Satelites,\r\n");
				current_state = NOGPS_STATE;
				break;

			
			case NOGPS_STATE:		//no GPS signal
				nokia_lcd_clear();
				nokia_lcd_set_cursor(10,16);
				nokia_lcd_write_string("NO GPS", 2);
				nokia_lcd_render();
				current_state =  IDLE_STATE; 
				break;

			
			case NEWDATA_STATE:		//processing received string from GPS module
				curr_data = (T_GPS_data*)malloc(sizeof(T_GPS_data));
				gps_get_data(&rcv_data[0],curr_data);
				nokia_lcd_clear();

				if(curr_data->valid == 1)
				{
					nokia_lcd_clear();
					display_data();
					current_state = IDLE_STATE;
				}
				else
				{
					current_state = NOGPS_STATE;
				}
				
				free(curr_data);
				rcv_data = (char *)malloc(sizeof(char) * 600);
				break;
			
			
			case IDLE_STATE:	//waiting for receiving new data
				_NOP();
				break;

			default:
				current_state = IDLE_STATE;
		}
	}
	
	return (0);
}

void initialization(void)
{
	TIM_config_prescaler(TIM2, TIM_PRESC_256);				//4ms timer
	TIM_config_interrupt(TIM2, TIM_OVERFLOW_DISABLE);

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	uart1_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

	_delay_ms(50);
	nokia_lcd_init();
	nokia_lcd_power(1);
	_delay_ms(50);
	nokia_lcd_write_picture();								//draw UREL logo stored in flash
	nokia_lcd_render();
	_delay_ms(1500);
	
	sei();
}

void display_data(void)
{
	k = 0;
	char comp[11];
	char *transmit_data = NULL;

	transmit_data = (char *)malloc(sizeof(char) * 100);

//--------------------------------------------------------------------------------
	nokia_lcd_set_cursor(0,0);  							//Time
	for(j = 0; j < 6; j++)
	{
		if(curr_data->time[j] != '\0')
		{
			nokia_lcd_write_char(curr_data->time[j],1);
			transmit_data[k++] = curr_data->time[j];
			if(j%2 ==1 && j!=5)
			{
				nokia_lcd_write_char(':',1);
				transmit_data[k++] = ':';
			}
		}
		
	}
	transmit_data[k++] = ',';
	
//--------------------------------------------------------------------------------
	nokia_lcd_set_cursor(0,10);  							//Latitude
	nokia_lcd_write_char(curr_data->lat_dir,1);
	if(curr_data->lat_dir == 'N')
		transmit_data[k++] = '+';
	else if(curr_data->lat_dir == 'S')
		transmit_data[k++] = '-';

	sprintf(comp,"%f",curr_data->latitude_deg);		//convert float to string
	for(j = 0;j<9;j++)
	{
		if(comp[j] != '\0')
		{
			nokia_lcd_write_char(comp[j],1);
			transmit_data[k++] = comp[j];
		}
	}
	transmit_data[k++] = ',';
	
//-----------------------------------------------------------------------------------
	nokia_lcd_set_cursor(0,19); 							//Longitude
	nokia_lcd_write_char(curr_data->lon_dir,1);
	if(curr_data->lon_dir == 'E')
		transmit_data[k++] = '+';
	else if(curr_data->lon_dir == 'W')
		transmit_data[k++] = '-';

	sprintf(comp,"%f",curr_data->longitude_deg);	//convert float to string
	for(j = 0;j<9;j++)
	{
		if(comp[j] != '\0')
		{
			nokia_lcd_write_char(comp[j],1);
			transmit_data[k++] = comp[j];
		}
	}
	
	transmit_data[k++] = ',';

//------------------------------------------------------------------------------------
	nokia_lcd_set_cursor(0,28);								//Speed in km/h
	nokia_lcd_write_string("Speed:",1);
	for(j = 0;j<4;j++)
	{
		if(curr_data->speed_kmh[j] != '\0')
		{
			nokia_lcd_write_char(curr_data->speed_kmh[j],1);
			transmit_data[k++] = curr_data->speed_kmh[j];
		}
	}
	transmit_data[k++] = ',';
	
//------------------------------------------------------------------------------------
	nokia_lcd_set_cursor(0,37);								//Altitude in metres
	nokia_lcd_write_string("Alt:",1);
	for(j = 0;j<5;j++)
	{
		if(curr_data->altitude[j] != '\0')
		{
			nokia_lcd_write_char(curr_data->altitude[j],1);
			transmit_data[k++] = curr_data->altitude[j];
		}
	}
	transmit_data[k++] = ',';
//------------------------------------------------------------------------------------
	nokia_lcd_set_cursor(72,28);							//Active satellites
	nokia_lcd_write_string("AS",1);
	nokia_lcd_set_cursor(74,37);
	for(j = 0;j<1;j++)
	{
		if(curr_data->num_of_act_sats[j] != '\0')
		{
			nokia_lcd_write_char(curr_data->num_of_act_sats[j],1);
			transmit_data[k++] = curr_data->num_of_act_sats[j];
		}
	}
	transmit_data[k++] = ',';
	
//------------------------------------------------------------------------------------
	transmit_data[k++] = '\r';								//end of transmit data message
    transmit_data[k++] = '\n';
	transmit_data[k++] = '\0';

	nokia_lcd_render();
	uart1_puts(transmit_data);
	free(transmit_data);
}






ISR(USART0_RX_vect)				//interrupt when USART receive byte
{
	TCNT2 = 0;
	TIFR2 = TIFR2 | (1<<TOV2);
	rcv_data[i++] = UDR0;
	TIMSK2 = TIMSK2 | _BV(TOIE2); //timer 2 overflow enable
	//TIM_config_interrupt(TIM2, TIM_OVERFLOW_ENABLE);
}

ISR(TIMER2_OVF_vect)			//interrupt after 4ms after receiving last byte
{
	rcv_data[++i] = '\0';
	i = 0;
	current_state = NEWDATA_STATE;
	TIMSK2 = TIMSK2 & ~_BV(TOIE2);	//timer 2 overflow disable
	//TIM_config_interrupt(TIM2, TIM_OVERFLOW_DISABLE);

}