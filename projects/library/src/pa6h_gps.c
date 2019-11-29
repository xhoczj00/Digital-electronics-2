/*
 * pa6h_gps.c
 *
 * Created: 28.11.2019 20:36:17
 *  Author: Martin Košút
 */ 


#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pa6h_gps.h"
#include "uart.h"
//
char rcv_data[550] = "$GPRMC,080423.000,A,4913.6146,N,01634.4190,E,0.30,70.31,271119,,,A*5E\r\n$GPVTG,70.31,T,,M,0.30,N,0.55,K,A*0B\r\n$GPGGA,080424.000,4913.6146,N,01634.4188,E,1,6,2.13,288.2,M,43.5,M,,*5D\r\n$GPGSA,A,3,01,03,23,11,19,17,,,,,,,2.33,2.13,0.94*00\r\n$GPGSV,3,1,12,01,75,146,25,03,64,276,30,11,54,185,29,17,34,303,32*78\r\n$GPGSV,3,2,12,23,28,205,22,31,24,092,22,19,21,319,28,40,17,124,*77\r\n$GPGSV,3,3,12,08,03,182,,09,01,213,,22,,,20,14,,,21*76\r\n";
T_GPS_msgs msg;

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

int msg_split(char *rcv_msg, int start, int stop)	//splitting messages and adding them in GPS_msgs structure
{
	char name[6];
	char GPRMCname[6] = "$GPRMC";
	char GPVTGname[6] = "$GPVTG";
	char GPGGAname[6] = "$GPGGA";
	char GPGSAname[6] = "$GPGSA";
	char GPGSVname[6] = "$GPGSV";
	char GPGSVmsg_number = 0, num_of_GPGSVmsgs = 0;
	int i = 0;
	
	for(i = 0; i < 6; i++)							//get name of current message
	{
		name[i] = rcv_msg[start + i];
	}

													//copy message into right place in structure
	if (compare_two_strings(&name,&GPRMCname,6) == 1)
	{
		for(i = start; i < stop; i++)
		{
			msg.GPRMC_msg[i - start] = rcv_msg[i];
		}
		//strcpy(msg.GPRMC_msg, full_msg);
	}
	else if (compare_two_strings(&name,&GPVTGname,6) == 1)
	{
		for(i = start; i < stop; i++)
		{
			msg.GPVTG_msg[i - start] = rcv_msg[i];
		}
		//strcpy(msg.GPVTG_msg, full_msg);
	}
	else if (compare_two_strings(&name,&GPGGAname,6) == 1)
	{
		for(i = start; i < stop; i++)
		{
			msg.GPGGA_msg[i - start] = rcv_msg[i];
		}
		//strcpy(msg.GPGGA_msg, full_msg);
	}
	else if (compare_two_strings(&name,&GPGSAname,6) == 1)
	{
		for(i = start; i < stop; i++)
		{
			msg.GPGSA_msg[i - start] = rcv_msg[i];
		}
		//strcpy(msg.GPGGA_msg, full_msg);
	}
	else if (compare_two_strings(&name,&GPGSVname,6) == 1)	//max 3 messages in row
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
				break;
			case '2':									//2nd message
				for(i = start; i < stop; i++)
				{
					msg.GPGSV2_msg[i - start] = rcv_msg[i];;
				}
				//strcpy(msg.GPGSV2_msg, full_msg);
				break;
			case '3':									//3rd message
				for(i = start; i < stop; i++)
				{
					msg.GPGSV3_msg[i - start] = rcv_msg[i];;
				}
				//strcpy(msg.GPGSV3_msg, full_msg);
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

void gps_get_data(void)
{
	int  length = 0, start = 0, stop = 0;
	//char one_msg[83];
	sei();
	/*for(int i = 0; i < 600; i++)				//receive all messages
	{
		rcv_data[i] = uart_getc();
		if(rcv_data[i] == UART_NO_DATA)
			break;
	}
	*/
	cli();
	
	stop = count_string(&rcv_data, start, '\n');		//stop position of first message out of frame
	while(length < 83)									//NMEA messages are always less than 83 characters
	{
		start = msg_split(&rcv_data, start, stop);		//split message and return start position for new message
		stop = count_string(&rcv_data, ++start, '\n');	//find next end of message
		length = stop - start;							
	}
}

