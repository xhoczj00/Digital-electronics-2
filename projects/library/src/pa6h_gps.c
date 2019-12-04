﻿/*
 * pa6h_gps.c
 *
 * Created: 28.11.2019 20:36:17
 *  Author: Martin Košút
 */ 


#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pa6h_gps.h"
#include "uart.h"
//

//
T_GPS_msgs msg;
T_GPS_data data;
char* return_buffer_ptr(void)
{
	return &rcv_data[0];
}

int frame_split(char *rcv_msg, int start, int stop)	//splitting messages out of frame and adding them in GPS_msgs structure
{
	char name[6];
	char GPRMCname[6] = "$GPRMC";
	char GPVTGname[6] = "$GPVTG";
	char GPGGAname[6] = "$GPGGA";
	char GPGSAname[6] = "$GPGSA";
	char GPGSVname[6] = "$GPGSV";
	char GPGSVmsg_number = 0;
	char num_of_GPGSVmsgs = 0;
	int i = 0;
	
	for(i = 0; i < 6; i++)							//get name of current message
	{
		name[i] = rcv_msg[start + i];
	}

													//copy message into right place in structure
	if (compare_two_strings(&name[0],&GPRMCname[0],6) == 1)		//GPRMC
	{
		for(i = start; i < stop; i++)
		{
			msg.GPRMC_msg[i - start] = rcv_msg[i];
		}
		msg.GPRMC_fresh = true;
		//strcpy(msg.GPRMC_msg, full_msg);
	}
	else if (compare_two_strings(&name[0],&GPVTGname[0],6) == 1)	//GPVTG
	{
		for(i = start; i < stop; i++)
		{
			msg.GPVTG_msg[i - start] = rcv_msg[i];
		}
		msg.GPVTG_fresh = true;
		//strcpy(msg.GPVTG_msg, full_msg);
	}
	else if (compare_two_strings(&name[0],&GPGGAname[0],6) == 1)	//GPGGA
	{
		for(i = start; i < stop; i++)
		{
			msg.GPGGA_msg[i - start] = rcv_msg[i];
		}
		msg.GPGGA_fresh = true;
		//strcpy(msg.GPGGA_msg, full_msg);
	}
	else if (compare_two_strings(&name[0],&GPGSAname[0],6) == 1)	//GPGSA
	{
		for(i = start; i < stop; i++)
		{
			msg.GPGSA_msg[i - start] = rcv_msg[i];
		}
		msg.GPGSA_fresh = true;
		//strcpy(msg.GPGGA_msg, full_msg);
	}
	else if (compare_two_strings(&name[0],&GPGSVname[0],6) == 1)	//GPGSV, max 3 messages in row
	{										
		num_of_GPGSVmsgs = rcv_msg[start + 7];			//get number of messages in row
		GPGSVmsg_number = rcv_msg[start + 9];			//get number of current message	

		switch(GPGSVmsg_number)
		{
			case '1':									//1st message
				for(i = start; i < stop; i++)
				{
					msg.GPGSV1_msg[i - start] = rcv_msg[i];;
				}
				//strcpy(msg.GPGSV1_msg, full_msg);
				msg.GPGSV1_fresh = true;
				break;
			case '2':									//2nd message
				for(i = start; i < stop; i++)
				{
					msg.GPGSV2_msg[i - start] = rcv_msg[i];;
				}
				//strcpy(msg.GPGSV2_msg, full_msg);
				msg.GPGSV2_fresh = true;
				break;
			case '3':									//3rd message
				for(i = start; i < stop; i++)
				{
					msg.GPGSV3_msg[i - start] = rcv_msg[i];;
				}
				//strcpy(msg.GPGSV3_msg, full_msg);
				msg.GPGSV3_fresh = true;
				break;
		}
	}
	return stop;
}

int count_string(char *string, int start, char end_character)        //return position of end character after start in input string
{
	int i,count=0;
	for (i = start; string[i] != end_character; i++)
		count++;
	return (start + count);
}

char compare_two_strings(char *str1, char *str2, int length)		//comparing two strings, returns 1 if equal
{
	int i;
	for(i = 0; i < length; i++)
	{
		if(str1[i] != str2[i])
		break;
	}
	if(i == length)
	return 1;
	else
	return 0;
}


void gps_get_data(char *received_data)
{
	int  i, length = 0, start = 0, stop = 0;
	//char array[550] = "$GPRMC,080423.000,A,4913.6146,N,11634.4190,E,0.30,70.31,271119,,,A*5E\r\n$GPVTG,81.42,T,,M,1.55,N,0.55,K,A*0B\r\n$GPGGA,080424.000,4913.6146,N,11634.4188,E,1,6,2.13,288.2,M,43.5,M,,*5D\r\n$GPGSA,A,3,01,03,23,11,19,17,,,,,,,2.33,2.13,0.94*00\r\n$GPGSV,3,1,12,01,75,146,25,03,64,276,30,11,54,185,29,17,34,303,32*78\r\n$GPGSV,3,2,12,23,28,205,22,31,24,092,22,19,21,319,28,40,17,124,*77\r\n$GPGSV,3,3,12,08,03,182,,09,01,213,,22,,,20,14,,,21*76\r\n";
	//"$GPRMC,235947.799,V,,,,,0.00,0.00,050180,,,N*48\r\n$GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32\r\n$GPGGA,235948.799,,,,,0,0,,,M,,M,,*4E\r\n$GPGSA,A,1,,,,,,,,,,,,,,,*1E\r\n";
	
	//for(i = 0; i<550; i++)
	//	rcv_data[i] = array[i];
	
	
	
	
	//char one_msg[83];
	//sei();
	/*for(int i = 0; i < 600; i++)				//receive all messages
	{
		rcv_data[i] = uart_getc();
		if(rcv_data[i] == UART_NO_DATA || rcv_data[i] == UART_FRAME_ERROR)
			break;
	}
	*/
	cli();
	
	msg.GPRMC_fresh = false;
	msg.GPVTG_fresh = false;
	msg.GPGGA_fresh = false;
	msg.GPGSA_fresh = false;
	msg.GPGSV1_fresh = false;
	msg.GPGSV2_fresh = false;
	msg.GPGSV3_fresh = false;
	
	stop = count_string(received_data, start, '\n');		//stop position of first message out of frame
	while(length < 83)									//NMEA messages are always less than 83 characters
	{
		start = frame_split(received_data, start, stop);	//split message and return start position for new message
		stop = count_string(received_data, ++start, '\n');	//find next end of message
		length = stop - start;							
	}

	//nokia_lcd_set_cursor(0,0);
	//nokia_lcd_write_string(msg.GPRMC_msg,1);
	//nokia_lcd_write_string(msg.GPVTG_msg,1);
	//nokia_lcd_render();
	sei();
	//parse_data();
}

int split_message(char *source, char *target, int start, int stop)	//copy data from source to target array between start and stop index
{
	int i;
	
	for(i = start; i < stop; i++)
	{
		target[i - start] = source[i];
	}
	return stop;
}
void parse_data(T_GPS_data *data)						//analyze data from fresh messages
{
	volatile int start = 7, stop = 0;	//must be volatile for debug

	if(msg.GPRMC_msg[18] == 'V')		//if invalid messages return
	{
		data->valid = false;
		//data.valid = false;
		return;						
	}
	else if(msg.GPRMC_msg[18] == 'A')	//parse data only if messages are valid
		data->valid = true;

	//GPRMC=========================================================================================
	if(msg.GPRMC_fresh == true)												//minimum GPS data
	{
		stop = count_string(msg.GPRMC_msg, start, ',');
		start = split_message(&msg.GPRMC_msg[0], &data->time[0], start, stop - 4);			//time
	
		start = stop + 3;						//jump over validity
		stop = count_string(&msg.GPRMC_msg[0], start, ',');
		start = split_message(&msg.GPRMC_msg[0], &data->latitudeNMEA[0], start, stop);		//latitude
		data->lat_dir = msg.GPRMC_msg[++start];
		data->latitude_deg = NMEAtoDeg(&data->latitudeNMEA[0]);
	
		start += 2;
		stop = count_string(&msg.GPRMC_msg[0], start, ',');
		start = split_message(&msg.GPRMC_msg[0], &data->longitudeNMEA[0], start, stop);	//longitude
		data->lon_dir = msg.GPRMC_msg[++start];
		data->longitude_deg = NMEAtoDeg(&data->longitudeNMEA[0]);
	
		start += 2;
		stop = count_string(&msg.GPRMC_msg[0], start, ',');
		start = split_message(&msg.GPRMC_msg[0], &data->speed_kn[0], start, stop);			//speed in knots
	
		start++;
		stop = count_string(&msg.GPRMC_msg[0], start, ',');
		start = split_message(&msg.GPRMC_msg[0], &data->course[0], start, stop);			//course
	
		start++;
		stop = count_string(&msg.GPRMC_msg[0], start, ',');
		start = split_message(&msg.GPRMC_msg[0], &data->date[0], start, stop);				//date
	}
	//GPVTG==========================================================================================
	start = 7;														//speed and track info
	stop = 0;
	if(msg.GPVTG_fresh == true)
	{
		stop = count_string(&msg.GPVTG_msg[0], start, ',');
		start = split_message(&msg.GPVTG_msg[0], &data->course[0], start, stop);		//course
		
		start += 6;
		stop = count_string(&msg.GPVTG_msg[0], start, ',');
		start = split_message(&msg.GPVTG_msg[0], &data->speed_kn[0], start, stop);		//speed in knots
		
		start += 3;
		stop = count_string(&msg.GPVTG_msg[0], start, ',');
		start = split_message(&msg.GPVTG_msg[0], &data->speed_kmh[0], start, stop);	//speed in knots
		
	}
	//GPGGA==============================================================================================
	start = 7;													//GPS fix info
	stop = 0;
	if(msg.GPGGA_fresh == true)
	{
		stop = count_string(&msg.GPGGA_msg[0], start, ',');
		start = split_message(&msg.GPGGA_msg[0], &data->time[0], start, stop - 4);			//time
		
		start += 5;
		stop = count_string(&msg.GPGGA_msg[0], start, ',');
		start = split_message(&msg.GPGGA_msg[0], &data->latitudeNMEA[0], start, stop);		//latitude
		data->lat_dir = msg.GPGGA_msg[++start];
		data->latitude_deg = NMEAtoDeg(&data->latitudeNMEA[0]);
		
		start += 2;
		stop = count_string(&msg.GPGGA_msg[0], start, ',');
		start = split_message(&msg.GPGGA_msg[0], &data->longitudeNMEA[0], start, stop);	//longitude
		data->lon_dir = msg.GPGGA_msg[++start];
		data->longitude_deg = NMEAtoDeg(&data->longitudeNMEA[0]);
		
		start += 4;
		stop = count_string(&msg.GPGGA_msg[0], start, ',');
		start = split_message(&msg.GPGGA_msg[0], &data->num_of_act_sats[0], start, stop);	//number of active satellites

		start += 6;
		stop = count_string(&msg.GPGGA_msg[0], start, ',');
		start = split_message(&msg.GPGGA_msg[0], &data->altitude[0], start, stop);			//altitude

	}
	//GPGSA==============================================================================================
	/*start = 7;												//IDs of active satellites
	stop = 0;
	if(msg.GPGSA_fresh == true)
	{
		
	}
	*/
	
	//GPGSV==============================================================================================
	start = 11;													//number and IDs of satellites in view
	stop = 0;
	if(msg.GPGSV1_fresh == true)
	{
		stop = count_string(&msg.GPGSV1_msg[0], start, ',');
		start = split_message(&msg.GPGSV1_msg[0], &data->num_of_view_sats[0], start, stop);	//number of satellites in view
	}
	/*
	if(msg.GPGSV2_fresh == true)
	{
		
	}
	if(msg.GPGSV3_fresh == true)
	{
		
	}
	*/
	
}

float NMEAtoDeg(char *NMEA)
{
	char decimal[4] ;
	char fraction[7] ;
	int i,counter;
	float result;

	counter = count_string(NMEA,0,'.');
	if(counter == 4)
	{
		
		decimal[0] = NMEA[0];
		decimal[1] = NMEA[1];
		decimal[2] = 'A';			//for atof function
	}
	else if (counter == 5)
	{
		
		decimal[0] = NMEA[0];
		decimal[1] = NMEA[1];
		decimal[2] = NMEA[2];
		decimal[3] = 'A';			//for atof function
	}

	for(i = 0; i < 7; i++)
	{
		fraction[i] = NMEA[i+counter-2];
	}
	
	result = atof(decimal) + (atof(fraction) / 60);
	return result;
}


