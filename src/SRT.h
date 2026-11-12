/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SRT.h
 * Author: Orbis Terrae <oterrae@gmail.com>
 *
 * Created on June 27, 2018, 7:22 PM
 */

#ifndef SRT_H
#define SRT_H
#include "GPS.h"


typedef struct SRT_atom {
    int index;
    char time_start[12];
    char time_stop[12];
    float home_lat;
    float home_lon;
    int date_year;
    int date_month;
    int date_day;
    int time_hour;
    int time_min;
    int time_sec;
    float gps_lat;
    float gps_lon;
    float ele;
    float ISO;
    float shutter;
    float EV;
    float Fnum;
} SRT_atom;


typedef struct SRT_list {
    int nb_data;
    SRT_atom* SRT;
} SRT_list;

/*
1
00:00:01,000 --> 00:00:02,000
HOME(4.5954,46.1735) 2018.04.21 19:10:44
GPS(4.5954,46.1735,13) BAROMETER:0.0
ISO:186 Shutter:100 EV: 0 Fnum:2.2 
*/
int release_SRT_list(struct SRT_list* SRT);

int init_SRT_list(struct SRT_list* SRT, int nb_samples);

int parseSRT(file_GPS_struct* file_gps, struct SRT_atom* SRT);

int convertSRTtoGPS(struct SRT_atom* SRT, struct GPS_atom* GPS, int nb_data);
#endif /* SRT_H */
