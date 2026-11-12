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

static time_t gpsu_to_epoch(const char gpsu[17], int *milliseconds) {
    int year, month, day, hour, minute, second, millisecond = 0;
    if (sscanf(gpsu, "%2d%2d%2d%2d%2d%2d.%3d", &year, &month, &day,
               &hour, &minute, &second, &millisecond) != 7 ||
        month < 1 || month > 12 || day < 1 || day > 31 ||
        hour > 23 || minute > 59 || second > 60 || millisecond > 999) {
        return (time_t)0;
    }
    if (milliseconds != NULL) *milliseconds = millisecond;

    year += 2000; /* GPMF GPSU years are encoded as 20xx. */
    year -= month <= 2;
    int era = (year >= 0 ? year : year - 399) / 400;
    unsigned year_of_era = (unsigned)(year - era * 400);
    unsigned adjusted_month = (unsigned)(month + (month > 2 ? -3 : 9));
    unsigned day_of_year = (153 * adjusted_month + 2) / 5 + (unsigned)day - 1;
    unsigned day_of_era = year_of_era * 365 + year_of_era / 4 -
                           year_of_era / 100 + day_of_year;
    long long days = (long long)era * 146097 + (long long)day_of_era - 719468;
    return (time_t)(days * 86400 + hour * 3600 + minute * 60 + second);
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
    (void)convertTime; /* GPS timestamps are derived from each sample's media offset. */

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

        if (file_gps->capture_time != (time_t)0) {
            time_t sample_time = file_gps->capture_time +
                    (time_t)(file_gps->capture_time_fraction + gps[i].ms);
            struct tm sample_tm;
            if (gmtime_r(&sample_time, &sample_tm) != NULL) {
                gps[i].GPS_hour = sample_tm.tm_hour;
                gps[i].GPS_min = sample_tm.tm_min;
                gps[i].GPS_sec = sample_tm.tm_sec;
                gps[i].GPS_year = sample_tm.tm_year + 1900;
                gps[i].GPS_month = sample_tm.tm_mon + 1;
                gps[i].GPS_day = sample_tm.tm_mday;
            }
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
    int stat_ok;
    int ret = 0 ,error =0;
    file_gps->file_nb = index;
    file_gps->samples = 0;
    file_gps->capture_time = 0;
    file_gps->capture_time_fraction = 0.0;
    file_gps->duration_seconds = 0.0;
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
    
    stat_ok = stat(filename, &st) == 0;
    if (!stat_ok){
        perror("stat failed");
        printf("ERROR: UNABLE TO GET TIME FROM %s\n",filename);
        memset(&st, 0, sizeof(st));
    }
    else{
        if(debug) printf("stats \"%s\" aquired\n",filename);
    }
    
#ifdef GPX_ORIGINAL_DATE
#ifdef __APPLE__
    file_gps->capture_time = stat_ok ? st.st_birthtimespec.tv_sec : time(NULL);
#else
    file_gps->capture_time = stat_ok ? st.st_mtime : time(NULL);
#endif
#else
    file_gps->capture_time = time(NULL);
#endif
    tm_info = gmtime(&file_gps->capture_time);
    if (tm_info != NULL) file_gps->time_file = *tm_info;
    
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
        gps[i].time_GPX[0] = '\0';
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
    if (nb_samples <= 0) return 0;
    if (dest == NULL || source == NULL) return -1;
    memcpy(dest, source, sizeof(*dest) * (size_t)nb_samples);
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
    int embedded_capture_time_found = 0;
    
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
        file_gps->duration_seconds = getGPMF_videolength();
        file_gps->samples = GetNumberGPMFPayloads();
        uint32_t index, payloads = GetNumberGPMFPayloads();

        if (payloads > MAX_GPS_POINTS) {
            fprintf(stderr, "Input has %u GPMF payloads; this build supports at most %d GPS points.\n",
                    payloads, MAX_GPS_POINTS);
            file_gps->samples = 0;
            goto cleanup;
        }

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

            if (!embedded_capture_time_found &&
                GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPSU"), GPMF_RECURSE_LEVELS)) {
                const char *raw_time = (const char *)GPMF_RawData(ms);
                int raw_size = GPMF_RawDataSize(ms);
                if (raw_time != NULL && raw_size >= 4) { /* RawDataSize is measured in 32-bit words. */
                    char gpsu[17];
                    memcpy(gpsu, raw_time, 16);
                    gpsu[16] = '\0';
                    int embedded_milliseconds = 0;
                    time_t embedded_time = gpsu_to_epoch(gpsu, &embedded_milliseconds);
                    if (embedded_time != (time_t)0) {
                        double clip_start = (double)embedded_time +
                                (double)embedded_milliseconds / 1000.0 - in;
                        file_gps->capture_time = (time_t)floor(clip_start);
                        file_gps->capture_time_fraction = clip_start - floor(clip_start);
                        embedded_capture_time_found = 1;
                        if (debug) printf("using embedded GoPro GPSU capture time %s\n", gpsu);
                        struct tm capture_tm;
                        if (gmtime_r(&file_gps->capture_time, &capture_tm) != NULL) {
                            file_gps->time_file = capture_tm;
                        }
                    }
                }
            }
            GPMF_ResetState(ms);

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
