/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   KML.c
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 * 
 * Created on March 12, 2018, 10:44 PM
 */

#include "KML.h"
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


int writeKML(file_GPS_struct* file_gps, 
        GPS_atom* GPS, 
        file_tags_struct* file_tag, 
        file_stops_struct* file_stops) {

    FILE* fp_KML;
    char filename[MAX_PATH] = "filename";
    char TimeStr[MAX_TIME_STR];
    int min = 0;
    int sec = 0;
    double msec = 0;
    int hour = 0;

    sprintf(filename, "%s.KML", file_gps->file_name);

    fp_KML = fopen(filename, "wt");
    if (fp_KML == NULL) fprintf(stderr, "open file KML %s error", filename);
/*
#ifdef  __CYGWIN__
#endif


    struct tm* tm_info;
    struct stat st;
 * 
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
    strftime(TimeStr, 26, "%d-%m-%Y %H:%M:%S", &file_gps->time_file);
    fprintf(fp_KML, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp_KML, "<kml  xmlns=\"http://earth.google.com/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:trails=\"http://www.google.com/kml/trails/1.0\">\n");
    fprintf(fp_KML, " <Document>\n");

    fprintf(fp_KML, "    <Style id=\"OrbisTerrae\">\n");
    fprintf(fp_KML, "        <BalloonStyle>\n");
    fprintf(fp_KML, "        <bgColor>DF8DB2D8</bgColor>\n");
    fprintf(fp_KML, "        <textColor>FF134E96</textColor>\n");
    fprintf(fp_KML, "        <text>File: %s<br/>", file_gps->short_name);
    fprintf(fp_KML, "              Date: %02d-%02d-%d<br/>", file_gps->time_file.tm_mday, file_gps->time_file.tm_mon + 1, file_gps->time_file.tm_year + 1900);
    fprintf(fp_KML, "              Distance: %.2fm - Duration: %ds<br/>", file_gps->distance, file_gps->samples);
    fprintf(fp_KML, "              Altitude: %.2fm - %.2fm<br/>", file_gps->min_alt, file_gps->max_alt);
    fprintf(fp_KML, "              Ascent: %dm - Descent: %dm<br/>", file_gps->ascent, file_gps->descent);
    if (hasTemp) fprintf(fp_KML, "Temp: %.1f°C<br/>", GPS[0].temp);
    if (hasHR) fprintf(fp_KML, "Heart: %dbps<br/>", GPS[0].HR);

    fprintf(fp_KML, "              https://sites.google.com/site/oterrae/<br/>");
    fprintf(fp_KML, "              %s v%s<br/>", SW_NAME, VERSION);
    fprintf(fp_KML, "              Copyright © 2017-%d Orbis Terrae<br/>", file_gps->time_file.tm_year + 1900);
    fprintf(fp_KML, "              All rights reserved.</text>\n");
    fprintf(fp_KML, "        </BalloonStyle>\n");

    fprintf(fp_KML, "     <LineStyle>\n");
    fprintf(fp_KML, "      <color>DF8DB2D8</color>\n");
    fprintf(fp_KML, "      <width>5</width>\n");
    fprintf(fp_KML, "     </LineStyle>\n");
    fprintf(fp_KML, "     <PolyStyle>\n");
    fprintf(fp_KML, "        <color>0f00ff00</color>\n");
    fprintf(fp_KML, "     </PolyStyle>\n");
    fprintf(fp_KML, "    <LabelStyle>\n");
    fprintf(fp_KML, "        <color>EF134E96</color>\n");
    fprintf(fp_KML, "     </LabelStyle>\n");
    fprintf(fp_KML, "     <IconStyle>\n");
    fprintf(fp_KML, "        <color>EF134E96</color>\n");
    fprintf(fp_KML, "        <scale>1</scale>\n");
    fprintf(fp_KML, "        <Icon>\n");
    fprintf(fp_KML, "          <href>http://maps.google.com/mapfiles/kml/shapes/arrow.png</href>\n");
    fprintf(fp_KML, "        </Icon>\n");
    fprintf(fp_KML, "      </IconStyle>\n");
    fprintf(fp_KML, "    </Style>\n");

    fprintf(fp_KML, "    <Style id=\"OrbisTerraeBei\">\n");
    fprintf(fp_KML, "     <LineStyle>\n");
    fprintf(fp_KML, "      <color>DF8DB2D8</color>\n");
    fprintf(fp_KML, "      <width>5</width>\n");
    fprintf(fp_KML, "     </LineStyle>\n");
    fprintf(fp_KML, "     <PolyStyle>\n");
    fprintf(fp_KML, "        <color>0f00ff00</color>\n");
    fprintf(fp_KML, "     </PolyStyle>\n");
    fprintf(fp_KML, "    <LabelStyle>\n");
    fprintf(fp_KML, "        <color>EF134E96</color>\n");
    fprintf(fp_KML, "     </LabelStyle>\n");
    fprintf(fp_KML, "     <IconStyle>\n");
    fprintf(fp_KML, "        <color>EF134E96</color>\n");
    fprintf(fp_KML, "        <scale>1</scale>\n");
    fprintf(fp_KML, "        <Icon>\n");
    fprintf(fp_KML, "          <href>http://maps.google.com/mapfiles/kml/shapes/arrow.png</href>\n");
    fprintf(fp_KML, "        </Icon>\n");
    fprintf(fp_KML, "      </IconStyle>\n");
    fprintf(fp_KML, "    </Style>\n");

    fprintf(fp_KML, "    <Style id=\"OrbisTerraeMat\">\n");
    fprintf(fp_KML, "     <LineStyle>\n");
    fprintf(fp_KML, "      <color>EF134E96</color>\n");
    fprintf(fp_KML, "      <width>5</width>\n");
    fprintf(fp_KML, "     </LineStyle>\n");
    fprintf(fp_KML, "     <PolyStyle>\n");
    fprintf(fp_KML, "        <color>0f00ff00</color>\n");
    fprintf(fp_KML, "     </PolyStyle>\n");
    fprintf(fp_KML, "    <LabelStyle>\n");
    fprintf(fp_KML, "        <color>DF8DB2D8</color>\n");
    fprintf(fp_KML, "     </LabelStyle>\n");
    fprintf(fp_KML, "     <IconStyle>\n");
    fprintf(fp_KML, "        <color>DF8DB2D8</color>\n");
    fprintf(fp_KML, "        <scale>1</scale>\n");
    fprintf(fp_KML, "        <Icon>\n");
    fprintf(fp_KML, "          <href>http://maps.google.com/mapfiles/kml/shapes/arrow.png</href>\n");
    fprintf(fp_KML, "        </Icon>\n");
    fprintf(fp_KML, "      </IconStyle>\n");
    fprintf(fp_KML, "    </Style>\n");

    fprintf(fp_KML, "<open>1</open>\n");
    fprintf(fp_KML, "<Camera>\n");
    fprintf(fp_KML, "  <longitude>%.6f</longitude>\n", GPS[file_gps->samples / 2].lon);
    fprintf(fp_KML, "  <latitude>%.6f</latitude>\n", GPS[file_gps->samples / 2].lat);
    fprintf(fp_KML, "  <altitude>2500</altitude>\n");
    fprintf(fp_KML, "  <heading>0</heading>\n");
    fprintf(fp_KML, "  <tilt>0</tilt>\n");
    fprintf(fp_KML, "</Camera>\n");

    /*--------------------- Tour Preview -----------------------------------------*/
    fprintf(fp_KML, "      <gx:Tour>\n");
    fprintf(fp_KML, "           <name>%s Tour</name>\n", SW_NAME);
    fprintf(fp_KML, "            <gx:Playlist>\n");
    for (int i = 0; i < file_gps->samples; i = i + 30) {
        fprintf(fp_KML, "                 <gx:FlyTo>\n");
        fprintf(fp_KML, "                      <gx:flyToMode>smooth</gx:flyToMode>\n");
        fprintf(fp_KML, "                      <gx:duration>1</gx:duration>\n");
        fprintf(fp_KML, "                      <Camera>\n");
        fprintf(fp_KML, "                           <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>25.000000</altitude>\n", GPS[i].lon, GPS[i].lat);
        fprintf(fp_KML, "                           <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[i].heading, (int) GPS[i].tilt);
        fprintf(fp_KML, "                      </Camera>\n");
        fprintf(fp_KML, "                 </gx:FlyTo>\n");
    }
    fprintf(fp_KML, "            </gx:Playlist>\n");
    fprintf(fp_KML, "      </gx:Tour>\n");

    fprintf(fp_KML, " \n");

    int j =0;
    fprintf(fp_KML, "      <gx:Tour>\n");
    fprintf(fp_KML, "           <name>%s POI</name>\n", SW_NAME);
    fprintf(fp_KML, "            <gx:Playlist>\n");
    for (int i = 0; i < file_gps->samples; i++) {
        if (i == 0 ||
                i == file_gps->samples / 4 ||
                i == file_gps->samples / 2 ||
                i == file_gps->samples * 3 / 4 ||
                i == file_gps->samples - 1 ||
                i == file_gps->max_alt_index ||
                i == file_gps->min_alt_index ||
                i == file_gps->max_speed_index ||
                i == file_gps->max_temp_index ||
                i == file_gps->min_temp_index ||
                i == file_gps->max_hr_index ||
                i == file_gps->max_hr_index) {
            
            fprintf(fp_KML, "                 <gx:FlyTo>\n");
            fprintf(fp_KML, "                      <gx:flyToMode>smooth</gx:flyToMode>\n");
            fprintf(fp_KML, "                      <gx:duration>10</gx:duration>\n");
            fprintf(fp_KML, "                      <Camera>\n");
            fprintf(fp_KML, "                           <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>%d</altitude>\n", 
                    GPS[i].lon, GPS[i].lat, 25);
            fprintf(fp_KML, "                           <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", 
                    (int) GPS[i].heading + 90*(j%2 -1), 
                    (int) GPS[i].tilt);
            fprintf(fp_KML, "                      </Camera>\n");
            fprintf(fp_KML, "                 </gx:FlyTo>\n");
        }
        if (file_gps->min_alt_index == i) {
        }
        if (file_gps->max_speed_index == i) {
        }
        if (file_gps->max_temp_index == i) {
        }
        if (file_gps->min_temp_index == i) {
        }
        if (file_gps->max_hr_index == i) {
        }
        if (file_gps->min_hr_index == i) {
        }
        j++;
    }
    fprintf(fp_KML, "            </gx:Playlist>\n");
    fprintf(fp_KML, "      </gx:Tour>\n");

    fprintf(fp_KML, " \n");


    /*--------------------- Tracks  ----------------------------------------------*/
    fprintf(fp_KML, "  <Folder>\n");
    fprintf(fp_KML, "  <name>%s</name>\n", file_gps->short_name);
    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "     <styleUrl>#OrbisTerrae</styleUrl>\n");
    fprintf(fp_KML, "   <name>%s</name>\n", TimeStr);
    fprintf(fp_KML, "    <MultiGeometry>\n");
    fprintf(fp_KML, "     <visibility>1</visibility>\n");

    for (int i = 0; i < file_gps->samples; i++) {
        if (i == 0) {
            //fprintf(fp_KML, "     <Point><coordinates>%.14f,%.14f,%.14f</coordinates></Point>\n", GPS[i].lon, GPS[i].lat, GPS[i].ele);
            fprintf(fp_KML, "     <LineString>\n");
            fprintf(fp_KML, "      <tessellate>1</tessellate>\n");
            fprintf(fp_KML, "      <altitudeMode>clampToGround</altitudeMode>\n");
            fprintf(fp_KML, "       <coordinates>\n");
        } else {
            fprintf(fp_KML, "%.14f,%.14f,%.14f ", GPS[i].lon, GPS[i].lat, GPS[i].ele);
        }
    }
    fprintf(fp_KML,
            "       </coordinates>\n"
            "      </LineString>\n"
            "     </MultiGeometry>\n"
            "   </Placemark>\n");


    /*--------------------- Point of Interest ------------------------------------*/
    fprintf(fp_KML, "  <Folder>\n");
    fprintf(fp_KML, "  <name>POI</name>\n");
    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>Start</name>\n");
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>250.00000</altitude>\n", GPS[0].lon, GPS[0].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[0].heading, 15);//(int) GPS[0].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", GPS[0].lon, GPS[0].lat, GPS[0].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");
    int quart2 = file_gps->samples / 4;
    int quart3 = file_gps->samples / 2;
    int quart4 = file_gps->samples * 3 / 4;

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>1/4</name>\n");
    fprintf(fp_KML, "    <visibility>0</visibility>\n");
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>50.00000</altitude>\n", GPS[quart2].lon, GPS[quart2].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[quart2].heading+90, 20); //(int) GPS[quart2].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", GPS[quart2].lon, GPS[quart2].lat, GPS[quart2].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>Mid Point</name>\n");
    fprintf(fp_KML, "    <visibility>0</visibility>\n");
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>125.00000</altitude>\n", GPS[quart3].lon, GPS[quart3].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[quart3].heading-90,  05);//(int) GPS[quart3].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", GPS[quart3].lon, GPS[quart3].lat, GPS[quart3].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>3/4</name>\n");
    fprintf(fp_KML, "    <visibility>0</visibility>\n");
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>50.00000</altitude>\n", GPS[quart4].lon, GPS[quart4].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[quart4].heading+90, 20);//(int) GPS[quart4].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", GPS[quart4].lon, GPS[quart4].lat, GPS[quart4].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>Finish</name>\n");
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>500.00000</altitude>\n", GPS[quart3].lon, GPS[quart3].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[quart4].heading+180, 15);//(int) GPS[quart4].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", GPS[file_gps->samples - 1].lon, GPS[file_gps->samples - 1].lat, GPS[file_gps->samples - 1].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>Min Alt %.2fm</name>\n", file_gps->min_alt);
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeBei</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>25.00000</altitude>\n", GPS[file_gps->min_alt_index].lon,
            GPS[file_gps->min_alt_index].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[file_gps->min_alt_index].heading,(int) GPS[file_gps->min_alt_index].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n",
            GPS[file_gps->min_alt_index].lon,
            GPS[file_gps->min_alt_index].lat,
            GPS[file_gps->min_alt_index].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>Max Alt %.2fm</name>\n", file_gps->max_alt);
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeBei</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>25.00000</altitude>\n", GPS[file_gps->max_alt_index].lon,
            GPS[file_gps->max_alt_index].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[file_gps->max_alt_index].heading+180,(int) GPS[file_gps->max_alt_index].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n",
            GPS[file_gps->max_alt_index].lon,
            GPS[file_gps->max_alt_index].lat,
            GPS[file_gps->max_alt_index].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");

    fprintf(fp_KML, "  <Placemark>\n");
    fprintf(fp_KML, "    <name>Max Speed %.2f</name>\n", file_gps->max_speed);
    fprintf(fp_KML, "    <styleUrl>#OrbisTerraeBei</styleUrl>\n");
    fprintf(fp_KML, "       <Camera>\n");
    fprintf(fp_KML, "         <longitude>%.6f</longitude><latitude>%.6f</latitude><altitude>25.00000</altitude>\n", GPS[file_gps->max_speed_index].lon,
            GPS[file_gps->max_speed_index].lat);
    fprintf(fp_KML, "         <heading>%d</heading><tilt>%d</tilt><roll>0</roll>\n", (int) GPS[file_gps->max_speed_index].heading,(int) GPS[file_gps->max_speed_index].tilt);
    fprintf(fp_KML, "       </Camera>\n");
    fprintf(fp_KML, "    <Point>\n");
    fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n",
            GPS[file_gps->max_speed_index].lon,
            GPS[file_gps->max_speed_index].lat,
            GPS[file_gps->max_speed_index].ele);
    fprintf(fp_KML, "    </Point>\n");
    fprintf(fp_KML, "  </Placemark>\n");


    if (hasTemp) {
        fprintf(fp_KML, "  <Placemark>\n");
        fprintf(fp_KML, "    <name>Min Temp %.2f°C</name>\n", file_gps->min_temp);
        fprintf(fp_KML, "    <styleUrl>#OrbisTerraeBei</styleUrl>\n");
        fprintf(fp_KML, "    <Point>\n");
        fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n",
                GPS[file_gps->min_temp_index].lon,
                GPS[file_gps->min_temp_index].lat,
                GPS[file_gps->min_temp_index].ele);
        fprintf(fp_KML, "    </Point>\n");
        fprintf(fp_KML, "  </Placemark>\n");
        fprintf(fp_KML, "  <Placemark>\n");
        fprintf(fp_KML, "    <name>Max Temp %.2f°C</name>\n", file_gps->max_temp);
        fprintf(fp_KML, "    <styleUrl>#OrbisTerraeBei</styleUrl>\n");
        fprintf(fp_KML, "    <Point>\n");
        fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n",
                GPS[file_gps->max_temp_index].lon,
                GPS[file_gps->max_temp_index].lat,
                GPS[file_gps->max_temp_index].ele);
        fprintf(fp_KML, "    </Point>\n");
        fprintf(fp_KML, "  </Placemark>\n");
    }

    if (hasHR) {
        fprintf(fp_KML, "  <Placemark>\n");
        fprintf(fp_KML, "    <name>Max HR %d</name>\n", file_gps->max_hr);
        fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
        fprintf(fp_KML, "    <Point>\n");
        fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n",
                GPS[file_gps->max_hr_index].lon,
                GPS[file_gps->max_hr_index].lat,
                GPS[file_gps->max_hr_index].ele);
        fprintf(fp_KML, "    </Point>\n");
        fprintf(fp_KML, "  </Placemark>\n");
    }
    fprintf(fp_KML, "  </Folder>\n");


    /*--------------------- GO PRO TAGS & HILGIHTS -------------------------------*/
    if(FCPXML) {
        fprintf(fp_KML, "  <Folder>\n");
        fprintf(fp_KML, "  <name>TAG</name>\n");
        for (int i = 0; i < file_tag->nb_tags; i++) {
            fprintf(fp_KML, "  <Placemark>\n");
            fprintf(fp_KML, "    <name>GOPRO HILIGHT%d</name>\n",i+1);
            fprintf(fp_KML, "    <visibility>0</visibility>\n");
            fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
            fprintf(fp_KML, "    <Point>\n");
            fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", 
                    GPS[(int)(file_tag->tags[i]/1000)].lon, 
                    GPS[(int)(file_tag->tags[i]/1000)].lat, 
                    GPS[(int)(file_tag->tags[i]/1000)].ele);
            fprintf(fp_KML, "    </Point>\n");
            fprintf(fp_KML, "  </Placemark>\n");
        }

        fprintf(fp_KML, "  </Folder>\n");
    }
    
    
    /*--------------------- STOPS -------------------------------*/
    int g=0;
    
    if(file_stops->nb_stops > 0) {
        fprintf(fp_KML, "  <Folder>\n");
        fprintf(fp_KML, "  <name>STOP</name>\n");
        for (int i = 0; i < file_stops->nb_stops; i++) {
            g= file_stops->stop_duration_index[i];
            fprintf(fp_KML, "  <Placemark>\n");
            fprintf(fp_KML, "    <name>STOP%d %ds</name>\n",i+1, file_stops->stop_duration[i]);
            fprintf(fp_KML, "    <visibility>0</visibility>\n");
            fprintf(fp_KML, "    <styleUrl>#OrbisTerraeMat</styleUrl>\n");
            fprintf(fp_KML, "    <Point>\n");
            fprintf(fp_KML, "      <coordinates>%.6f,%.6f,%.6f</coordinates>\n", 
                    GPS[g].lon, 
                    GPS[g].lat, 
                    GPS[g].ele);
            fprintf(fp_KML, "    </Point>\n");
            fprintf(fp_KML, "  </Placemark>\n");
        }

        fprintf(fp_KML, "  </Folder>\n");
    }
    
    
    fprintf(fp_KML,
            "  </Folder>\n"
            " </Document>\n"
            "</kml>");


    fclose(fp_KML);

    return -1;

}