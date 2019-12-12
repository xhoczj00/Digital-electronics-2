/*
 * pa6h_gps.h
 *
 * Created: 28.11.2019 20:36:38
 *  Author: Martin Košút
 */ 


#ifndef PA6H_GPS_H_
#define PA6H_GPS_H_

#include <stdbool.h>

char rcv_data[500];


typedef struct GPS_data
{
	bool valid;
	char time[7];
	char date[7];
	char latitudeNMEA[10];
	char lat_dir;
	float latitude_deg;
	char longitudeNMEA[11];
	char lon_dir;
	float longitude_deg;
	char speed_kn[8];
	char speed_kmh[8];
	char course[7];
	char num_of_act_sats[3];
	char num_of_view_sats[3];
	char altitude[8];
	//float alt_accuracy;
	//char active_sats[11];
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
	char GPGSV4_msg[83];
	bool GPRMC_fresh;
	bool GPVTG_fresh;
	bool GPGGA_fresh;
	bool GPGSA_fresh;
	bool GPGSV1_fresh;
	bool GPGSV2_fresh;
	bool GPGSV3_fresh;
	bool GPGSV4_fresh;
}T_GPS_msgs;



void gps_get_data(char *received_data,  T_GPS_data *data);

/**
 *  @brief   Splitting received data into messages and storing them in separate structure
 *
 *  @param   rcv_msg is input packet
			 start is index of position where it start storing data according name
			 stop is index of position where it stops 
 *  @return  stop index
 */
int frame_split(char *rcv_msg,  T_GPS_msgs *msg, int start, int stop);

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


/**
 *  @brief   Calculate checksum value and compare it with received checksum
 *
 *  @param   message is string in NMEA format
 *  @return  0 if calculated and received checksum are different
			 1 if calculated and received checksum are equal
 */
char check_checksum(char *message);


/**
 *  @brief   Analyze data from fresh messages and copy them into data structure
 */
void parse_data(T_GPS_data *data, T_GPS_msgs *msg);

/**
 *  @brief   Copy data from source to target array between start and stop index
 *
 *  @param   source, target is input strings
 *  @return  stop index
 */
int split_message(char *source, char *target, int start, int stop);

/**
 *  @brief   Converting NMEA latitude and longitude format to degrees
 *
 *  @param   NMEA is input string in NMEA format
 *  @return  converted float
 */
float NMEAtoDeg(char *NMEA);




#endif /* PA6H_GPS_H_ */