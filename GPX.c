/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GPX.c
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 * 
 * Created on March 12, 2018, 10:43 PM
 */

#include "GPX.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tags.h"
#include "GPS.h"
#include "GPMF_mp4reader.h"


int writeGPX(file_GPS_struct* file_gps, GPS_atom* GPS) {

    FILE* fp;
    int duration = 0;
    char filename[MAX_PATH];
    char TimeStr[MAX_TIME_STR];
    char* basec;
    int min = 0;
    int sec = 0;
    double msec = 0;
    int hour = 0;
    int error;
    
    error = snprintf(filename, MAX_PATH, "%s.GPX", file_gps->file_name);
    fp = fopen(filename, "wt");
    if(fp == NULL) fprintf(stderr, "open file GPX %s error",filename);
#ifdef __CYGWIN__
#endif
/*
 * 
    struct tm* tm_info;
  // using the original date creation of the MP4 file in the GPX data
    struct stat st;
    if (stat(file_gps->file_name, &st) != 0)
        perror("stat failed");
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
 */
    
    
    if(debug) printf("Writing GPX file %s - %lu (%s - %lu) [%d]\n",filename, strlen(filename), 
            file_gps->file_name, strlen(file_gps->file_name), error);
    
    strftime(TimeStr, 26, "%Y-%m-%dT%H:%M:%SZ", &file_gps->time_file);
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n");
    fprintf(fp, "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\"\n");
    
    fprintf(fp, "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprintf(fp, "     xmlns:trp=\"http://www.garmin.com/xmlschemas/TripExtensions/v1\"\n");
    fprintf(fp, "     xmlns:adv=\"http://www.garmin.com/xmlschemas/AdventuresExtensions/v1\"\n");
    fprintf(fp, "     xmlns:prs=\"http://www.garmin.com/xmlschemas/PressureExtension/v1\"\n");
    fprintf(fp, "     xmlns:tmd=\"http://www.garmin.com/xmlschemas/TripMetaDataExtensions/v1\"\n");
    fprintf(fp, "     xmlns:vptm=\"http://www.garmin.com/xmlschemas/ViaPointTransportationModeExtensions/v1\"\n");
    fprintf(fp, "     xmlns:ctx=\"http://www.garmin.com/xmlschemas/CreationTimeExtension/v1\"\n");
    fprintf(fp, "     xmlns:vidx1=\"http://www.garmin.com/xmlschemas/VideoExtension/v1\"\n");
    fprintf(fp, "     xmlns:gpxx=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\"\n");
    fprintf(fp, "     xmlns:gpxtrkx=\"http://www.garmin.com/xmlschemas/TrackStatsExtension/v1\"\n");
    fprintf(fp, "     xmlns:gpxtrkoffx=\"http://www.garmin.com/xmlschemas/TrackMovieOffsetExtension/v1\"\n");
    fprintf(fp, "     xmlns:wptx1=\"http://www.garmin.com/xmlschemas/WaypointExtension/v1\"\n");
    fprintf(fp, "     xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\"\n");
    fprintf(fp, "     xmlns:gpxpx=\"http://www.garmin.com/xmlschemas/PowerExtension/v1\"\n");
    fprintf(fp, "     xmlns:gpxacc=\"http://www.garmin.com/xmlschemas/AccelerationExtension/v1\"\n");
    fprintf(fp, "     creator=\"VIRB Elite\"\n");
    fprintf(fp, "     version=\"1.1\"\n");
    fprintf(fp, "     xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/WaypointExtension/v1 http://www8.garmin.com/xmlschemas/WaypointExtensionv1.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www8.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/ActivityExtension/v1 http://www8.garmin.com/xmlschemas/ActivityExtensionv1.xsd http://www.garmin.com/xmlschemas/AdventuresExtensions/v1 http://www8.garmin.com/xmlschemas/AdventuresExtensionv1.xsd http://www.garmin.com/xmlschemas/PressureExtension/v1 http://www.garmin.com/xmlschemas/PressureExtensionv1.xsd http://www.garmin.com/xmlschemas/TripExtensions/v1 http://www.garmin.com/xmlschemas/TripExtensionsv1.xsd http://www.garmin.com/xmlschemas/TripMetaDataExtensions/v1 http://www.garmin.com/xmlschemas/TripMetaDataExtensionsv1.xsd http://www.garmin.com/xmlschemas/ViaPointTransportationModeExtensions/v1 http://www.garmin.com/xmlschemas/ViaPointTransportationModeExtensionsv1.xsd http://www.garmin.com/xmlschemas/CreationTimeExtension/v1 http://www.garmin.com/xmlschemas/CreationTimeExtensionsv1.xsd http://www.garmin.com/xmlschemas/AccelerationExtension/v1 http://www.garmin.com/xmlschemas/AccelerationExtensionv1.xsd http://www.garmin.com/xmlschemas/PowerExtension/v1 http://www.garmin.com/xmlschemas/PowerExtensionv1.xsd http://www.garmin.com/xmlschemas/VideoExtension/v1 http://www.garmin.com/xmlschemas/VideoExtensionv1.xsd\">\n");
    fprintf(fp, "     <metadata>\n");
    fprintf(fp, "       <link href=\"https://sites.google.com/site/oterrae/\">\n");
    fprintf(fp, "          <text>Orbis Terrae</text>\n");
    fprintf(fp, "       </link>\n");
    fprintf(fp, "       <time>%s</time>\n", TimeStr);
    fprintf(fp, "     </metadata>\n");
    fprintf(fp, "<trk>\n");
    fprintf(fp, "  <name>%s</name>\n", file_gps->short_name);
    fprintf(fp, "       <extensions>\n");
    fprintf(fp, "           <gpxx:TrackExtension>\n");
    fprintf(fp, "              <gpxx:DisplayColor>Cyan</gpxx:DisplayColor>\n");
    fprintf(fp, "           </gpxx:TrackExtension>\n");
    fprintf(fp, "           <gpxtrkoffx:TrackMovieOffsetExtension>\n");
    fprintf(fp, "              <gpxtrkoffx:StartOffsetSecs>0</gpxtrkoffx:StartOffsetSecs>\n");
    fprintf(fp, "           </gpxtrkoffx:TrackMovieOffsetExtension>\n");
    fprintf(fp, "       </extensions>\n");
    fprintf(fp, "  <trkseg>\n");

    for (int i = 0; i < file_gps->samples; i++) {
        strftime(TimeStr, 26, "%Y-%m-%dT", &file_gps->time_file);
        fprintf(fp, "    <trkpt lat=\"%.14f\" lon=\"%.14f\">\n", GPS[i].lat, GPS[i].lon);
        fprintf(fp, "        <ele>%.2f</ele>\n", GPS[i].ele);
        
#ifdef GPX_ORIGINAL_DATE
        msec = (GPS[i].ms)*1000 + file_gps->time_file.tm_sec * 1000 + file_gps->time_file.tm_min * 60 * 1000 + file_gps->time_file.tm_hour * 60 * 60 * 1000;
#else
        msec = i * 1000;
#endif
        sec = (int) ((msec / 1000)) % 60;
        min = (int) ((int) (msec / (1000 * 60)) % 60);
        hour = (int) ((int) (msec / (1000 * 60 * 60)) % 24);
        fprintf(fp, "        <time>%s%02d:%02d:%02dZ</time>\n", TimeStr, hour, min, sec);
        if (hasTemp || hasHR) {
            fprintf(fp, "     <extensions>\n");
            fprintf(fp, "      <gpxtpx:TrackPointExtension>\n");
            if (hasTemp) fprintf(fp, "           <gpxtpx:atemp>%.2f</gpxtpx:atemp>\n", GPS[i].temp);
            if (hasHR) fprintf(fp, "           <gpxtpx:hr>%d</gpxtpx:hr>\n", GPS[i].HR);
            fprintf(fp, "       </gpxtpx:TrackPointExtension>\n");
            fprintf(fp, "     </extensions>\n");
        }
        fprintf(fp, "    </trkpt>\n");
    }
    fprintf(fp, "    </trkseg>\n"
            "  </trk>\n"
            "</gpx>\n");

    if(debug) printf("GPX file written\n");
    
    fclose(fp);
    return -1;
}