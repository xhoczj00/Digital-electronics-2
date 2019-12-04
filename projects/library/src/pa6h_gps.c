/*
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

char check_checksum(char *message)			//calculate checksum from all characters between $ and * compare it to received checksum after * in NMEA message
{
	volatile int start = 0, stop = 0, i;
	volatile char calc_checksum, rcv_checksum[2], calc_checksum_hex[2];
	
	stop = count_string(message, start, '*');
	rcv_checksum[0] = message[stop + 1];
	rcv_checksum[1] = message[stop + 2];
	
	calc_checksum = message[1];
	for(i = 2; i < stop; i++)				//calculate checksum
	{
		calc_checksum ^= message[i];
	}
	if(calc_checksum < 16)					//convert calculated checksum to hex string
	{
		char temp[2];
		sprintf(temp, "%x", 0);	
		calc_checksum_hex[0] = temp[0];
		sprintf(temp, "%x", calc_checksum);	
		calc_checksum_hex[1] = temp[0];
	}
	else
	{
		sprintf(calc_checksum_hex, "%x", calc_checksum);
	}
	
	if(compare_two_strings(&rcv_checksum[0], &calc_checksum_hex[0], 2) == 1)	//compare calculated and received checksum, return 1 if they are equal, else 0
		return 1;
	else
		return 0;
		
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


	//volatile char ak = check_validity(msg.GPRMC_msg);



	//GPRMC=========================================================================================
	if(msg.GPRMC_fresh == true)												//minimum GPS data
	{
		if(check_checksum(msg.GPRMC_msg) == 1)		//parse only if received and calculated checksum are same
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
			start = split_message(&msg.GPRMC_msg[0], &data->longitudeNMEA[0], start, stop);		//longitude
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
	}
	//GPVTG==========================================================================================
	start = 7;														//speed and track info
	stop = 0;
	if(check_checksum(msg.GPVTG_msg) == 1)		//parse only if received and calculated checksum are same
	{
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
	}
	//GPGGA==============================================================================================
	start = 7;													//GPS fix info
	stop = 0;
	if(check_checksum(msg.GPGGA_msg) == 1)		//parse only if received and calculated checksum are same
	{
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
			start = split_message(&msg.GPGGA_msg[0], &data->longitudeNMEA[0], start, stop);		//longitude
			data->lon_dir = msg.GPGGA_msg[++start];
			data->longitude_deg = NMEAtoDeg(&data->longitudeNMEA[0]);
		
			start += 4;
			stop = count_string(&msg.GPGGA_msg[0], start, ',');
			start = split_message(&msg.GPGGA_msg[0], &data->num_of_act_sats[0], start, stop);	//number of active satellites

			start += 6;
			stop = count_string(&msg.GPGGA_msg[0], start, ',');
			start = split_message(&msg.GPGGA_msg[0], &data->altitude[0], start, stop);			//altitude

		}
	}
	//GPGSA==============================================================================================
	/*start = 7;												//IDs of active satellites
	stop = 0;
	if(check_checksum(msg.GPGSA_msg) == 1)		//parse only if received and calculated checksum are same
	{
		if(msg.GPGSA_fresh == true)
		{
			
		}
	}
	*/
	
	//GPGSV==============================================================================================
	start = 11;													//number and IDs of satellites in view
	stop = 0;
	if(msg.GPGSV1_fresh == true)
	{
		if(check_checksum(msg.GPGSV1_msg) == 1)		//parse only if received and calculated checksum are same
		{
			stop = count_string(&msg.GPGSV1_msg[0], start, ',');
			start = split_message(&msg.GPGSV1_msg[0], &data->num_of_view_sats[0], start, stop);	//number of satellites in view
		}
	}
	/*
	if(msg.GPGSV2_fresh == true)
	{
		if(check_checksum(msg.GPGSV2_msg) == 1)	
		{
		}
		
	}
	if(msg.GPGSV3_fresh == true)
	{
		if(check_checksum(msg.GPGSV3_msg) == 1)	
		{
		}
	}
	*/
	
}

float NMEAtoDeg(char *NMEA)					//convert NMEA latitude and longitude format to degrees
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
		decimal[2] = 'A';			//character to stop atof function
	}
	else if (counter == 5)
	{
		
		decimal[0] = NMEA[0];
		decimal[1] = NMEA[1];
		decimal[2] = NMEA[2];
		decimal[3] = 'A';			//character to stop atof function
	}

	for(i = 0; i < 7; i++)
	{
		fraction[i] = NMEA[i+counter-2];
	}
	
	result = atof(decimal) + (atof(fraction) / 60);
	return result;
}



