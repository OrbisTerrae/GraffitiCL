/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GPS.c
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 * 
 * Created on March 12, 2018, 10:27 AM
 */

#include "GPS.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"


float getRadians(float n) {
  return n * (M_PI / 180);
}
float getDegrees(float n) {
  return n * (180 / M_PI);
}

float getBearing(float startLat, float startLong, float endLat, float endLong){
  
  startLat = getRadians(startLat);
  startLong = getRadians(startLong);
  endLat = getRadians(endLat);
  endLong= getRadians(endLong);

  float dLong = endLong - startLong;

  float dPhi = log(tan(endLat/2.0+ M_PI/4.0)/tan(startLat/2.0+M_PI/4.0));
  
  if (fabsf(dLong) > M_PI){
    if (dLong > 0.0)
       dLong = -(2.0 * M_PI - dLong);
    else
       dLong = (2.0 * M_PI + dLong);
  }

  return fmod((getDegrees(atan2(dLong, dPhi)) + 360.0), 360.0);
}

float getDistance(float startLat, float startLong, float endLat, float endLong) {
    float R = 6371e3; // metres
    float phi1 = getRadians(startLat);
    float phi2 = getRadians(endLat);
    float delptaphi = getRadians(endLat - startLat);
    float delpalambda = getRadians(endLong - startLong);

    float a = sin(delptaphi / 2) * sin(delptaphi / 2) +
            cos(phi1) * cos(phi2) *
            sin(delpalambda / 2) * sin(delpalambda / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

float getSpeed(float dist, float distprev, float distprevprev){
    
    return ((dist + distprev + distprevprev)/3) * 3.6;
   
}

int set_file_gps_name_samples(file_GPS_struct* file_gps,char * new_name, int nb_data){
    sprintf(file_gps->file_name, "%s", new_name);
    strncpy(file_gps->short_name, mybasename(new_name), MAX_SHORT);
    file_gps->samples = nb_data;
    return 0;
}

int calc_GPS_init(GPS_atom* gps, int nb_samples){
    for (int i = 0; i < nb_samples; i++) {
        gps[i].temp = init_temp - ((gps[i].ele - gps[0].ele) / 100) + ((rand() % 10) / 10);
        gps[i].HR = hr_base + sin(i * PI / 180) * 10 + rand() % 5;
    }
    return 0;
}

int calc_GPS_stats(file_GPS_struct* file_gps, GPS_atom* gps, int convertTime) {
    struct tm* local_time;
    char buffer[80];

    for (int i = 0; i < file_gps->samples; i++) {
        if (gps[i].ele > file_gps->max_alt) {
            file_gps->max_alt = gps[i].ele;
            file_gps->max_alt_index = i;
        }
        if (gps[i].ele < file_gps->min_alt) {
            file_gps->min_alt = gps[i].ele;
            file_gps->min_alt_index = i;
            //if(debug) printf("--%d-- new min alt: %f (was %f)\n", i, gps[i].ele, file_gps->min_alt);
        }
        if (gps[i].temp > file_gps->max_temp) {
            file_gps->max_temp = gps[i].temp;
            file_gps->max_temp_index = i;
        }
        if (gps[i].temp < file_gps->min_temp) {
            file_gps->min_temp = gps[i].temp;
            file_gps->min_temp_index = i;
        }
        if (gps[i].HR > file_gps->max_hr) {
            file_gps->max_hr = gps[i].HR;
            file_gps->max_hr_index = i;
        }
        if (gps[i].HR < file_gps->min_hr) {
            file_gps->min_hr = gps[i].HR;
            file_gps->min_hr_index = i;
        }
        if (gps[i].lon > file_gps->max_lon) {
            file_gps->max_lon = gps[i].lon;
        }
        if (gps[i].lon < file_gps->min_lon) {
            file_gps->min_lon = gps[i].lon;
        }

        if (gps[i].lat > file_gps->max_lat) {
            file_gps->max_lat = gps[i].lat;
        }
        if (gps[i].lat < file_gps->min_lat) {
            file_gps->min_lat = gps[i].lat;
        }

        if (i < file_gps->samples - 1) {
            gps[i].heading = getBearing(gps[i].lat, gps[i].lon, gps[i + 1].lat, gps[i + 1].lon);
            gps[i].tilt = 90 + (gps[i + 1].ele - gps[i].ele);
        } else {
            gps[i].heading = gps[i - 1].heading;
            gps[i].tilt = 90;
        }

        if (i > 1) {
            gps[i].distFromPrev = getDistance(gps[i - 1].lat, gps[i - 1].lon, gps[i].lat, gps[i].lon);
            gps[i].curSpeed = getSpeed(gps[i].distFromPrev, gps[i - 1].distFromPrev, gps[i - 2].distFromPrev);
            if (gps[i].ele > gps[i - 1].ele) {
                file_gps->ascent += (gps[i].ele - gps[i - 1].ele);
            }
            if (gps[i].ele < gps[i - 1].ele) {
                file_gps->descent += (gps[i - 1].ele - gps[i].ele);
            }
        } else {
            if (i == 1) {
                gps[i].distFromPrev = getDistance(gps[i - 1].lat, gps[i - 1].lon, gps[i].lat, gps[i].lon);
                gps[i].curSpeed = getSpeed(gps[i].distFromPrev, gps[i - 1].distFromPrev, 0);
            } else { // i == 0       
                gps[i].distFromPrev = 0;
                gps[i].curSpeed = 0;
            }
        }

        file_gps->distance += gps[i].distFromPrev;

        if (gps[i].curSpeed > file_gps->max_speed) {
            //if(debug) printf("--%d-- new max speed: %f (was %f)\n", i, gps[i].curSpeed, file_gps->max_speed);

            file_gps->max_speed = gps[i].curSpeed;
            file_gps->max_speed_index = i;
        }

        if (convertTime) {
            // need to copy file_gps-> time_file into the local struct and add the second per samples.
            local_time = &file_gps->time_file;

            strftime(buffer, 80, "%H:%M:%S.", local_time);
            //if (debug) printf("********** calc_GPS_stats local_date before mktime %s\n", buffer);

            local_time->tm_sec += 1;
            mktime(local_time);

            strftime(buffer, 80, "%H:%M:%S.", local_time);
            //if (debug) printf("********** calc_GPS_stats local_date after mktime %s\n", buffer);

            gps[i].GPS_hour = local_time->tm_hour;
            gps[i].GPS_min = local_time->tm_min;
            gps[i].GPS_sec = local_time->tm_sec;
            gps[i].GPS_year = local_time->tm_year + 1900;
            gps[i].GPS_month = local_time->tm_mon + 1;
            gps[i].GPS_day = local_time->tm_mday;

            /*if (debug) printf("********** calc_GPS_stats local_time:%02d:%02d:%02d GPS_time:%02d:%02d:%02d\n",
                    local_time->tm_hour, local_time->tm_min, local_time->tm_sec,
                    gps[i].GPS_hour, gps[i].GPS_min, gps[i].GPS_sec);
             */
        }
    }
    return 0;
}

char* mybasename(char* path){
    int i= strlen(path);
    
    while(i > 0){
        if(path[i] == '/'){
            return &path[i+1];
        }
        else{
        }
        i--;
    }
    return "";
}

int init_file_gps(file_GPS_struct* file_gps, char* filename, int index) {
    struct stat st;
    struct tm* tm_info;
    
    int ret = 0 ,error =0;
    file_gps->file_nb = index;
    error = snprintf(file_gps->file_name, strlen(filename)+1,"%s", filename);
    if(debug) printf("return:%d - name original:%s (size: %zd) copied %s\n",
            error, filename,
            strlen(filename)+1,
            file_gps->file_name);
    
    file_gps->short_name = malloc(MAX_SHORT*sizeof(char));
    strncpy(file_gps->short_name, mybasename(filename),MAX_SHORT);

    file_gps->max_alt = -9999.0;
    file_gps->max_alt_index = 0;
    file_gps->min_alt = 9999.0;
    file_gps->min_alt_index = 0;
    file_gps->distance = 0.0;
    file_gps->max_speed = 0.0;
    file_gps->max_speed_index = 0;
    file_gps->max_lon = -9999.0;
    file_gps->min_lon = 9999.0;
    file_gps->max_lat = -9999.0;
    file_gps->min_lat = 9999.0;
    file_gps->max_temp = -9999.0;
    file_gps->max_temp_index = 0;
    file_gps->min_temp = 9999.0;
    file_gps->min_temp_index = 0;
    file_gps->max_hr = 0;
    file_gps->max_hr_index = 0;
    file_gps->min_hr = 9999;
    file_gps->min_hr_index = 0;
    file_gps->ascent = 0;
    file_gps->descent = 0;
    
    if (stat(filename, &st) != 0){
        perror("stat failed");
        printf("ERROR: UNABLE TO GET TIME FROM %s\n",filename);
    }
    else{
        if(debug) printf("stats \"%s\" aquired\n",filename);
    }
    
#ifdef GPX_ORIGINAL_DATE 
#ifdef _DARWIN_FEATURE_64_BIT_INODE
     tm_info = localtime(&st.st_birthtimespec);
#else
    tm_info = localtime(&st.st_ctime);
#endif
#else    
    // using date of the execution of the app to generate the GPX file
    time_t timer;
    time(&timer);
    tm_info = localtime(&timer);
#endif
    file_gps->time_file = *tm_info;
    
    if (debug) printf("init_file_gps [%d/%d] done\n",index, file_gps->file_nb);
    return ret;
}

void print_gps_file(file_GPS_struct * file_gps) {
    char buffer[80];
    printf("GPS[%d] %s\t%d samples\n", file_gps->file_nb, file_gps->short_name, file_gps->samples);
    strftime (buffer,80,"%F - %H:%M:%S.",&file_gps->time_file);
    printf("\t file stat: %s\n",buffer);
    printf("\t %.04f\t< altitude  <\t%.4f\n", file_gps->min_alt, file_gps->max_alt);
    printf("\t %.04f\t< latitude  <\t%.4f\n", file_gps->min_lat, file_gps->max_lat);
    printf("\t %.04f\t< longitude <\t%.4f\n", file_gps->min_lon, file_gps->max_lon);
    if(hasTemp) printf("\t %.04f\t< temperat. <\t%.4f\n", file_gps->min_temp, file_gps->max_temp);
    if(hasHR) printf("\t %.d\t\t< HeartRate <\t%d\n", file_gps->min_hr, file_gps->max_hr);
    printf("\t distance: %.03fm - max speed[%d]: %.03fkm/h - duration: %ds\n\n", 
            file_gps->distance, file_gps->max_speed_index, file_gps->max_speed, file_gps->samples);
}

int release_file_gps(file_GPS_struct * file_gps) {
    if(file_gps->short_name != NULL) free(file_gps->short_name);
    return 0;
}

int init_GPS(GPS_atom * gps, int nb_samples) {
    if(debug) printf("init_GPS with %d samples\n",nb_samples);
    for (int i = 0; i < nb_samples; i++) {
        gps[i].lat = 0;
        gps[i].lon = 0;
        gps[i].ele = 0;
        gps[i].ms = 0;
        gps[i].temp = INIT_TEMP;
        gps[i].HR = HR_BASE;
        gps[i].GPS_index = 0;
        gps[i].heading = 0;
        gps[i].tilt = 90;
        gps[i].distFromPrev = 0.0;
        gps[i].curSpeed = 0.0;
        gps[i].prevSpeed = 0.0;
        gps[i].GPS_year = 0;
        gps[i].GPS_month = 0;
        gps[i].GPS_day = 0;
        gps[i].GPS_hour = 0;
        gps[i].GPS_min = 0;
        gps[i].GPS_sec = 0;
    }
    return 0;
}


int copy_GPS(GPS_atom* dest, GPS_atom* source, int nb_samples) {
    //if(debug) printf("copy_GPS %d samples to copy from %d to %d\n",nb_samples, source, dest);
    for (int i = 0; i < nb_samples; i++) {
        //if(debug) printf("copy_GPS %d\n",i);
        snprintf(dest[i].time_GPX, strlen(source[i].time_GPX +1),"%s",source[i].time_GPX);
        dest[i].GPS_year = source[i].GPS_year;
        dest[i].GPS_month = source[i].GPS_month;
        dest[i].GPS_day = source[i].GPS_day;
        dest[i].GPS_hour = source[i].GPS_hour;
        dest[i].GPS_min = source[i].GPS_min;
        dest[i].GPS_sec = source[i].GPS_sec;
        dest[i].lat = source[i].lat;
        dest[i].lon = source[i].lon;
        dest[i].ele = source[i].ele;
        dest[i].ms = source[i].ms;
        dest[i].temp = source[i].temp;
        dest[i].HR = source[i].HR;
        dest[i].GPS_index = source[i].GPS_index;
        dest[i].heading = source[i].heading;
        dest[i].tilt = source[i].tilt;
        dest[i].distFromPrev = source[i].distFromPrev;
        dest[i].curSpeed = source[i].curSpeed;
        dest[i].prevSpeed = source[i].prevSpeed;
    }
    return 0;
}


int release_GPS(GPS_atom * gps) {
    return 0;
}

void print_GPS(GPS_atom* gps, int samples) {
    for (int i = 0; i < samples; i++) {
        printf("###GPS[%d] lat:%.6f - lon:%.6f - ele:%.6f \n", i, gps[i].lat, gps[i].lon, gps[i].ele);
    }
}

void double2Ints(double f, int p, int *i, int *d) {
    // f = float, p=decimal precision, i=integer, d=decimal
    int li;
    int prec = 1;

    for (int x = p; x > 0; x--) {
        prec *= 10;
    }; // same as power(10,p)

    li = (int) f; // get integer part
    *d = (int) ((f - li) * prec); // get decimal part
    *i = li;
}


int parseMP4_GPS(file_GPS_struct* file_gps, GPS_atom * GPS) {

    int32_t ret = GPMF_OK;
    GPMF_stream metadata_stream, *ms = &metadata_stream;
    double metadatalength;
    uint32_t *payload = NULL; //buffer to store GPMF samples from the MP4.
    time_t timer;
    struct tm* tm_info;
    double GPSrate = GPS_RATE;
    char TimeStr[26];
    int min = 0;
    int sec = 0;
    double msec = 0;
    int hour = 0;
    int fractpart, intpart;
    float tempc = INIT_TEMP;
    int hr = HR_BASE;
    int AltInit = 0;
    int c;
    FILE *fp = NULL;
    
    struct stat st;

    
    time(&timer);
    tm_info = localtime(&timer);
    
    strftime(TimeStr, 26, "%Y-%m-%dT%H:%M:%SZ", tm_info);

    GPS[0].GPS_hour = tm_info->tm_hour;
    GPS[0].GPS_min = tm_info->tm_min;
    GPS[0].GPS_sec = tm_info->tm_sec;
    GPS[0].GPS_year = tm_info->tm_year + 1900;
    GPS[0].GPS_month = tm_info->tm_mon +1;
    GPS[0].GPS_day = tm_info->tm_mday;

    metadatalength = OpenGPMFSource(file_gps->file_name);
    if (debug) printf("OpenGPMFSource: %s\n", file_gps->file_name);

    if (metadatalength > 0.0) {
        uint32_t index, payloads = GetNumberGPMFPayloads();

        file_gps->samples = payloads;
//        if (debug) printf("GetNumberGPMFPayloads: %u\n", payloads);

        for (index = 0; index < payloads; index++) {

            //if (debug) printf("index: %u < %u payload\n", index, payloads);

            uint32_t payloadsize = GetGPMFPayloadSize(index);
            //if (debug) printf("GetGPMFPayloadSize: %u\n", payloadsize);
            double in = 0.0, out = 0.0; //times

            payload = GetGPMFPayload(payload, index);
            if (payload == NULL) {
                goto cleanup;
            } else {
                //if (debug) printf("GetGPMFPayload: %u\n", payloads);
            }

            ret = GetGPMFPayloadTime(index, &in, &out);

            //if (debug) printf("GetGPMFPayloadTime: in:%f out:%f ret:%d\n", in, out, ret);
            if (ret != GPMF_OK)
                goto cleanup;

            ret = GPMF_Init(ms, payload, payloadsize);

            //if (debug) printf("GPMF_Init\n");
            if (ret != GPMF_OK)
                goto cleanup;

            // Find all the available Streams and the data carrying FourCC
            if (index == 0) // show first payload 
            {
                while (GPMF_OK == GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS)) {
                    if (GPMF_OK == GPMF_SeekToSamples(ms)) //find the last FOURCC within the stream
                    {
                        uint32_t key = GPMF_Key(ms);
                        GPMF_SampleType type = GPMF_Type(ms);
                        uint32_t elements = GPMF_ElementsInStruct(ms);
                        //uint32_t samples = GPMF_Repeat(ms);
                        uint32_t samples = GPMF_PayloadSampleCount(ms);

                        if (samples) {
                            //if (debug) printf("STRM of %c%c%c%c found\n", PRINTF_4CC(key));

                            if (type == GPMF_TYPE_COMPLEX) {
                                GPMF_stream find_stream;
                                GPMF_CopyState(ms, &find_stream);

                                if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, GPMF_CURRENT_LEVEL)) {
                                    char tmp[64];
                                    char *data = (char *) GPMF_RawData(&find_stream);
                                    int size = GPMF_RawDataSize(&find_stream);

                                    if (size < sizeof (tmp)) {
                                        memcpy(tmp, data, size);
                                        tmp[size] = 0;
                                    }
                                }

                            } else {
                            }
                        }
                    }
                }
                GPMF_ResetState(ms);
            }

            //            fprintf(fp, "<!--                                frame %09D                          -->\n", index);
            //if (debug) printf("frame -- %06us --\n", index);

            if (GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPS5"), GPMF_RECURSE_LEVELS) || //GoPro Hero5 GPS
                    GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPRI"), GPMF_RECURSE_LEVELS) || //GoPro Karma
                    GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("TMPC"), GPMF_RECURSE_LEVELS)) //GoPro temp
            {
                uint32_t key = GPMF_Key(ms);
                uint32_t samples = GPMF_Repeat(ms);
                uint32_t elements = GPMF_ElementsInStruct(ms);
                uint32_t buffersize = samples * elements * sizeof (double);
                GPMF_stream find_stream;
                double *ptr, *tmpbuffer = malloc(buffersize);
                char units[10][6] = {""};
                uint32_t unit_samples = 1;

                if (tmpbuffer && samples) {
                    uint32_t i, j;

                    //Search for any units to display
                    GPMF_CopyState(ms, &find_stream);
                    if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL) ||
                            GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL)) {
                        char *data = (char *) GPMF_RawData(&find_stream);
                        int ssize = GPMF_StructSize(&find_stream);
                        unit_samples = GPMF_Repeat(&find_stream);

                        for (i = 0; i < unit_samples; i++) {
                            memcpy(units[i], data, ssize);
                            units[i][ssize] = 0;
                            data += ssize;
                        }
                    }

                    //GPMF_FormattedData(ms, tmpbuffer, buffersize, 0, samples); // Output data in LittleEnd, but no scale
                    GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE); //Output scaled data as floats


                    ptr = tmpbuffer;
                    if (key == STR2FOURCC("TMPC")) {
                        printf("<!--     TMPC Temperature detected   -->\n");
                        tempc = ptr[0];
                        //not tested, likely to break
                    }
                    if (index == 0) {
                        AltInit = ptr[2];
                    }
                    /* There are 18 samples (18.177936Hz) per 1s data     */
                    strftime(TimeStr, 26, "%Y-%m-%dT", tm_info);
                    

                    //if (debug) printf("time: %s\n", TimeStr);

                    GPS[index].lat = ptr[0];
                    GPS[index].lon = ptr[1];
                    GPS[index].ele = ptr[2];
                    GPS[index].ms = in;
                    GPS[index].GPS_index = index;

                    GPS[index].temp = init_temp - ((ptr[2] - AltInit) / 100) + ((rand() % 10) / 10);
                    GPS[index].HR = hr_base + sin(index * PI / 180) * 10 + rand() % 5;

                    msec = (in)*1000;
                    sec = (int) (msec / 1000) % 60;
                    min = (int) ((int) (msec / (1000 * 60)) % 60);
                    hour = (int) ((int) (msec / (1000 * 60 * 60)) % 24);
                    double2Ints(msec / 1000, 3, &intpart, &fractpart);
                    snprintf(GPS[index].time_GPX, MAX_TIME_STR, "%s%02d:%02d:%02d.%03dZ", TimeStr, hour, min, sec, fractpart);
                    
                    
                    //if (debug) printf("time: %s\n", GPS[index].time_GPX);
                    free(tmpbuffer);
                }
            }
            GPMF_ResetState(ms);
        }
        file_gps->max_alt = AltInit;


        if (debug) printf("%ds parsed\n", file_gps->samples);

cleanup:
        if (payload) FreeGPMFPayload(payload);
        payload = NULL;
        CloseGPMFSource();
    }


    return -1;
}