/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FCPXML.h
 * Author: Orbis Terrae
 *
 * Created on February 20, 2018, 3:49 PM
 */

#ifndef FCPXML_H
#define FCPXML_H

#include <stdio.h>
#ifndef TAGS_H
#include "tags.h"
#endif

#ifndef GPS_H
#include "GPS.h"
#endif

#ifndef STOP_H
#include "stop.h"
#endif

int* timeline;

int init_timeline(int sequence);
int release_timeline();

int getYear();
int writeFCPXML_header(FILE* f);
int writeFCPXML_TAGS_closer(FILE* f, int duration);
int writeFCPXML_TAGS_data(FILE* f, file_tags_struct* file_tags);
int writeFCPXML_TL_header(FILE* f);
int writeFCPXML_TL_closer(FILE* f, int duration);
int writeFCPXML_TL_data(FILE* f, file_GPS_struct* file_gps, GPS_atom* GPS, file_tags_struct* file_tag, file_stops_struct* file_stops);
int writeFCPXML(file_GPS_struct* file_gps, GPS_atom* GPS, file_tags_struct* file_tag, file_stops_struct* file_stops);

#endif /* FCPXML_H */
