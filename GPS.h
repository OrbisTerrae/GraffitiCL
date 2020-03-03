/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GPS.h
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 *
 * Created on March 12, 2018, 10:27 AM
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>

#ifndef GPS_H
#define GPS_H

#ifndef TAGS_H
#include "tags.h"
#endif

#define INIT_TEMP 20
#define HR_BASE 65
#define PI 3.14159265
// max file duration : 17m 42s
#define MAX_GPS_POINTS 1080
#define MAX_TIME_STR 26

typedef struct GPS_atom {
    int GPS_index;
    char time_GPX[MAX_TIME_STR];
    float lon;
    float lat;
    float ele;
    float temp;
    float ms;
    int HR;
    float heading;
    float tilt;
    float distFromPrev;
    float curSpeed;
    float prevSpeed;
    int GPS_year;
    int GPS_month;
    int GPS_day;
    int GPS_hour;
    int GPS_min;
    int GPS_sec;
} GPS_atom;


typedef struct GPS_list {
    int nb_data;
    GPS_atom* GPS;
} GPS_list;

typedef struct GPS_table {
    int nb_file;
    GPS_list* GPS;
} GPS_table;

typedef struct file_GPS_struct {
    int file_nb;
    int samples;
    struct tm time_file;
    char file_name[MAX_PATH];
    char* short_name;
    float max_alt;
    int max_alt_index;
    float min_alt;
    int min_alt_index;
    float distance;
    float max_speed;
    int max_speed_index;
    float max_lon;
    float min_lon;
    float max_lat;
    float min_lat;
    float max_temp;
    int max_temp_index;
    float min_temp;
    int min_temp_index;
    int max_hr;
    int max_hr_index;
    int min_hr;
    int min_hr_index;
    int ascent;
    int descent;
} file_GPS_struct;


typedef struct successive_gps_file {
    char* first_file;
    char* new_name;
    int nb_files;
    int index_files[MAX_FILE];
} successive_gps_file;

int init_temp;
int hr_base;

int init_file_gps(file_GPS_struct* file_gps, char* filename, int index);
int release_file_gps(file_GPS_struct* file_gps);
void print_gps_file(file_GPS_struct* file_gps);
int set_file_gps_name_samples(file_GPS_struct* file_gps,char * new_name, int nb_data);
int calc_GPS_stats(file_GPS_struct* file_gps, GPS_atom* gps, int converTime);
int calc_GPS_init(GPS_atom * GPS, int nb_samples);

int init_GPS(GPS_atom* gps, int nb_samples);
int release_GPS(GPS_atom* gps);
void print_GPS(GPS_atom* gps, int samples);

int copy_GPS(GPS_atom* dest, GPS_atom* source, int samples);

void double2Ints(double f, int p, int *i, int *d);
char* concat(const char *s1, const char *s2);
int parseMP4_GPS(file_GPS_struct* file_gps, GPS_atom* GPS);

char* mybasename(char* path);


#endif /* GPS_H */
