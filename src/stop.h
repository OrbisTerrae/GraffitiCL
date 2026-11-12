/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   stop.h
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 *
 * Created on May 3, 2018, 1:41 PM
 */

#ifndef STOP_H
#define STOP_H

#ifndef GPS_H
#include "GPS.h"
#endif

#define MAX_STOPS 100

typedef struct file_stops_struct
{
	int index;
        int nb_stops;
        int stop_duration [MAX_STOPS];
        int stop_duration_index [MAX_STOPS];
} file_stops_struct;

int init_stops(file_stops_struct* file_stops, int index);
int release_stops(file_stops_struct* file_stops);
void print_stops(file_stops_struct* file_stops);
int calc_stops(file_stops_struct* file_stops, GPS_atom* gps, int samples);

#endif /* STOP_H */
