/*
 * pa6h_gps.h
 *
 * Created: 28.11.2019 20:36:38
 *  Author: Martin Košút
 */ 


#ifndef PA6H_GPS_H_
#define PA6H_GPS_H_

#include <stdbool.h>

typedef struct GPS_data
{
	bool valid;
	char time[6];
	char date[6];
	char latitude[9];
	char lat_dir;
	char longtitude[10];
	char lon_dir;
	char speed_kn[7];
	char speed_kmh[7];
	char course[6];
	char num_of_act_sats;
	//char num_of_view_sats;
	float altitude;
	float alt_accuracy;
	char active_sats[11];
}T_GPS_data;

typedef struct GPS_msgs
{
	char GPRMC_msg[83];
	char GPVTG_msg[83];
	char GPGGA_msg[83];
	char GPGSA_msg[83];
	char GPGSV1_msg[83];
	char GPGSV2_msg[83];
	char GPGSV3_msg[83];
	bool GPRMC_fresh;
	bool GPVTG_fresh;
	bool GPGGA_fresh;
	bool GPGSA_fresh;
	bool GPGSV1_fresh;
	bool GPGSV2_fresh;
	bool GPGSV3_fresh;
}T_GPS_msgs;



void gps_get_data(void);

/**
 *  @brief   Splitting received data into messages and storing them in separate structure
 *
 *  @param   rcv_msg is input packet
			 start is index of position where it start storing data according name
			 stop is index of position where it stops 
 *  @return  stop index
 */
int frame_split(char *rcv_msg, int start, int stop);

/**
 *  @brief   Count number of character in string until end character
 *
 *  @param   string is input string 
			 start is index of position where it start counting in string
			 end_character is character in string where it stops counting
 *  @return  start position + count		 
 */
int count_string(char *string, int start, char end_character);


/**
 *  @brief   Compare two strings
 *
 *  @param   str1, str2 are input strings
 *  @return  0 if str1 and str2 are not the same
			 1 if str1 and str2 are the same
 */
char compare_two_strings(char *str1, char *str2, int length);

int parsing_data(void);




#endif /* PA6H_GPS_H_ */