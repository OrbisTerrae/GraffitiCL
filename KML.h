/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   KML.h
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 *
 * Created on March 12, 2018, 10:44 PM
 */

#ifndef KML_H
#define KML_H

#ifndef STOP_H
#include "stop.h"
#endif

#include "GPS.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

int writeKML(file_GPS_struct* file_gps, GPS_atom* GPS, file_tags_struct* file_tag, file_stops_struct* file_stops);

#endif /* KML_H */
