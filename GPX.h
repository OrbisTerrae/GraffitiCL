/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GPX.h
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 *
 * Created on March 12, 2018, 10:43 PM
 */

#ifndef GPX_H
#define GPX_H

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

int writeGPX(file_GPS_struct* file_gps, GPS_atom* GPS);


#endif /* GPX_H */
