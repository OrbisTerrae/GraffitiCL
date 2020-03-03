/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tags.h
 * Author: Orbis Terrae
 *
 * Created on February 19, 2018, 11:09 AM
 */

#ifndef TAGS_H
#define TAGS_H


#define MAX_FILE 512
#ifndef MAX_PATH
#define MAX_PATH 1080
#endif
#ifndef MAX_SHORT
#define MAX_SHORT 128
#endif
#define MAX_TAGS 80
#define GPS_RATE 18.177936
// uncomment to generate GPX file based on the date of the MP4 creation
#define GPX_ORIGINAL_DATE
// comment to generate GPX file based on the date of the day.

#define VERSION "0.4.0"
#define SW_NAME "GraffitiGPS"


typedef struct file_tags_struct
{
	int index;
	char file_name[MAX_PATH];
	char* short_name;
	char* dir_name;
        long duration_frame;
        long duration_sec;
        float frame_rate;
        int time_scale;
        int x_resolution;
        int y_resolution;
        int nb_tags;
        int* tags;
} file_tags_struct;

int nb_files;
int GPX;
int KML;
int KMLZ;
int FCPXML;
int hasTemp;
int hasHR;
int ML;

int init_file_tags(file_tags_struct* file_tags, char* filename, int index);
int release_file_tags(file_tags_struct* file_tags);
int copy_tags(file_tags_struct* dest_tags,int* tags, int nb_tags);
int set_file_tags_name_duration(file_tags_struct* file_tags, char* new_name, int nb_data);
int parseMP4_tags(file_tags_struct* file_tags);
void print_tags(file_tags_struct* file_tags);


#endif /* TAGS_H */
