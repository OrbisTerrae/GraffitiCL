/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SRT.c
 * Author: Orbis Terrae <oterrae@gmail.com>
 * 
 * Created on June 27, 2018, 7:22 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include "SRT.h"
#include "GPMF_mp4reader.h"

int init_SRT_list(struct SRT_list* SRT, int nb_samples){
    // SRT = malloc(sizeof (struct SRT_list));
    for(int i=0 ; i<nb_samples ; i++){
        SRT[i].SRT = malloc(sizeof(struct SRT_atom));
        SRT[i].SRT->index = 1;
    }
    return 0;
}
int release_SRT_list(struct SRT_list* SRT){
    for(int i=0 ; i< SRT[i].nb_data ; i++){
        if(SRT[i].SRT != NULL) free(SRT[i].SRT);
    }
    //free(SRT_list);
    return 0;
}
/*
1
00:00:01,000 --> 00:00:02,000
HOME(4.5954,46.1735) 2018.04.21 19:10:44
GPS(4.5954,46.1735,13) BAROMETER:0.0
ISO:186 Shutter:100 EV: 0 Fnum:2.2 
*/
int parseSRT(file_GPS_struct* file_gps, struct SRT_atom* SRT){
    FILE* f;
    int line = 0;
    int index = 0;
    int SRT_line = 1;
    int ret =0;
    int time_not_used[8];
    char EVStr[25];
    char EV2Str[10];
    int EVInt = 0;
    int EV2Int = 0;
    int sat_fix;
    
    f = fopen(file_gps->file_name,"r+");
    
    while (!feof(f)){
        ret = fscanf(f,"%d\n",&SRT[line].index);
        if (ret != 1){
            printf("[%s] Invalid SRT data at line %d/%d - index expected.\n",file_gps->short_name, SRT_line, ret);
            SRT[line].index = 0;
            return -1;
        }
        SRT_line++;
        
        ret = fscanf(f,"%d:%d:%d,%d --> %d:%d:%d,%d\n",
                &time_not_used[0],
                &time_not_used[1],
                &time_not_used[2],
                &time_not_used[3],
                &time_not_used[4],
                &time_not_used[5],
                &time_not_used[6],
                &time_not_used[7]);
        if (ret != 8){
            printf("[%s] Invalid SRT data at line %d (fail scan after argument %d) - 'time int --> out' expected.\n",file_gps->short_name, SRT_line, ret);
            SRT[line].index = 0;
            return -1;
        }
        SRT_line++;
        
        ret = fscanf(f,"HOME(%f,%f) %d.%d.%d %d:%d:%d\n",
                &SRT[line].home_lon,
                &SRT[line].home_lat,
                &SRT[line].date_year,
                &SRT[line].date_month,
                &SRT[line].date_day,
                &SRT[line].time_hour,
                &SRT[line].time_min,
                &SRT[line].time_sec);
        if (ret != 8){
            printf("[%s] Invalid SRT data at line %d (fail scan after argument %d) - 'HOME(lon,lat) date time' expected.\n",file_gps->short_name, SRT_line, ret);
            SRT[line].index = 0;
            return -1;
        }
        SRT_line++;
        
        ret = fscanf(f,"GPS(%f,%f,%d) BAROMETER:%f\n",
                &SRT[line].gps_lon,
                &SRT[line].gps_lat,
                &sat_fix,
                &SRT[line].ele);
        if (ret != 4){
            printf("[%s] Invalid SRT data at line %d (fail scan after argument %d) - 'GPS(lon,lat, fix) BAROMETER: ele' expected.\n",file_gps->short_name, SRT_line, ret);
            SRT[line].index = 0;
            return -1;
        }
        SRT_line++;
        
        ret = fscanf(f,"ISO:%f Shutter:%f EV: %s Fnum:%f\n",
                &SRT[line].ISO,
                &SRT[line].shutter,
                EVStr,
                &SRT[line].Fnum);

        if (ret != 4) {
            if(debug) printf("[%s] Invalid SRT data at line %d (fail scan after argument %d) - 'ISO:x Shutter:x EV: x Fnum:x' expected [EVStr:%s].\n", file_gps->short_name, SRT_line, ret, EVStr);
            if (ret == 3) {
                //printf("ret = 3 EVStr = %s\n", EVStr);
                // likely to be in the format of 
                // ISO:100 Shutter:4000 EV:-2 1/3 Fnum:2.2 
                ret = fscanf(f, "%s Fnum:%f\n",
                        EV2Str, &SRT[line].Fnum);

                //printf("ret = 3/%d EVStr = %s EV2Str = %s\n", ret, EVStr, EV2Str);
                if (ret == 2) {
                    sscanf(EVStr, "%d", &EVInt);
                    SRT[line].EV = EVInt;
                    sscanf(EV2Str, "%d/%d", &EVInt, &EV2Int);
                    if (SRT[line].EV > 0) {
                        SRT[line].EV += (float) EVInt / (float) EV2Int;
                    } else {
                        SRT[line].EV -= (float) EVInt / (float) EV2Int;
                    }
                    //if(debug) printf("ret = 3/2 EVStr = %s EV2Str = %s EVInt=%d EV2Int=%d EV=%f\n", EVStr, EV2Str, EVInt, EV2Int, SRT[line].EV);
                } else {
                    // not sure of the format then ?
                    //if (debug) printf("ret = 2/%d EV2Str = %s\n", ret, EV2Str);
                    printf("[%s] Invalid SRT data at line %d (fail scan after argument %d) - 'ISO:x Shutter:x EV: x Fnum:x' expected [EVStr:%s/EV2Str:%s].\n", file_gps->short_name, SRT_line, ret, EVStr, EV2Str);
                    SRT[line].EV =0;
                }
            }
            SRT[line].index = 0;
            //return -1;
        }
        else{
            // found all 4 parameter, but EV may be in 1/3 format
            ret = sscanf(EVStr, "%d/%d", &EVInt, &EV2Int);
            if (ret == 1) {
                // EV = 0 for example
                //if (debug) printf("ret = 4/1 EVStr = %s EV=%d\n", EVStr, EVInt);
                SRT[line].EV = EVInt;
            } else {
                if (ret == 2) {
                    // EV = -1/3 for example
                    //if (debug) printf("ret = 4/2 EVStr = %s EV1=%d / EV2=%d\n", EVStr, EVInt, EV2Int);
                    SRT[line].EV = (float) EVInt /(float) EV2Int;
                } else {
                    // not sure of the format
                    //if (debug) printf("ret = 4/%d EVStr = %s unable to recognized, defaulting to 0\n", ret, EVStr);
                    printf("[%s] Invalid SRT data at line %d (fail scan after argument %d) - 'ISO:x Shutter:x EV: x Fnum:x' expected [EVStr:%s/EV2Str:%s].\n", file_gps->short_name, SRT_line, ret, EVStr, EV2Str);
                    SRT[line].EV = 0;
                }
            }
        }
        if (debug) printf("[%s] SRT data at line %d -----> %f\n",file_gps->short_name, SRT_line ,SRT[line].EV);
        SRT_line+=2;
        
        if (debug) printf("SRT %d data over %d SRT line processed \n",line, SRT_line);
        
        line++;
    }
    file_gps->samples = line;
    fclose(f);
    return line;
}

int convertSRTtoGPS(struct SRT_atom* SRT, struct GPS_atom* GPS, int nb_data){
   
    for(int i = 0 ; i < nb_data ; i++){
        GPS[i].lat = SRT[i].gps_lat; 
        GPS[i].lon = SRT[i].gps_lon; 
        GPS[i].ele = SRT[i].ele; 
        
        GPS[i].GPS_year = SRT[i].date_year;
        GPS[i].GPS_month = SRT[i].date_month;
        GPS[i].GPS_day = SRT[i].date_day; 
        GPS[i].GPS_hour = SRT[i].time_hour;
        GPS[i].GPS_min = SRT[i].time_min;
        GPS[i].GPS_sec = SRT[i].time_sec; 
    }
    
    if (debug) printf("%d SRT data converted to GPS\n",nb_data);
        
    return 0;
}