/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   stop.c
 * Author: Orbis Terrae <https://sites.google.com/site/oterrae/home>
 * 
 * Created on May 3, 2018, 1:41 PM
 */

#include <stdio.h>
#include "stop.h"
#include "tags.h"

int init_stops(file_stops_struct* file_stops, int index){
    for(int i  = 0 ; i < MAX_STOPS ; i++){
        file_stops->stop_duration[i] = 0;
        file_stops->stop_duration_index[i] = 0;
    }
    file_stops->index = index;
    file_stops->nb_stops = 0 ;
    return 0;
}

int release_stops(file_stops_struct* file_stops){
    return 0;
}
void print_stops(file_stops_struct* file_stops){
    if(file_stops->nb_stops > 0){
        printf ("STOP [%d]: %d stops\n", file_stops->index, file_stops->nb_stops);
        for (int i = 0; i < file_stops->nb_stops; i++) {
            printf("\t Start at frame [%d] for %ds\n", file_stops->stop_duration_index[i], file_stops->stop_duration[i]);
        }
        printf("\n");
    }
}

int calc_stops(file_stops_struct* file_stops, GPS_atom* gps, int samples){
    int stop_count = 0;
    int count = 0;
    int index_start = 0;
    file_stops->nb_stops = 0;
    
    for (int i = 0; i < samples; i++) {
        if (gps[i].distFromPrev < 0.0999) {
            if (count == 0) {
                index_start = i;
            }
            count += 1;
            //printf("STOP: GPS [%d| distance %.14f count %d index_start %d\n", i, gps[i].distFromPrev, count, index_start);
        } else {
            if (count > 7) {
                file_stops->stop_duration[file_stops->nb_stops] = count;
                file_stops->stop_duration_index[file_stops->nb_stops] = index_start;
                /*
                printf("STOP detected [file index %d] nb_stop %d / stop_duration %ds / stop_duration_index %d (stop_count %d; index_start %d)\n",
                        file_stops->index,
                        file_stops->nb_stops,
                        file_stops->stop_duration[file_stops->nb_stops],
                        file_stops->stop_duration_index[file_stops->nb_stops],
                        stop_count,
                        index_start);
                */
                file_stops->nb_stops++;
                 
            }
            count = 0;
        }
    }
    
    return 0;
}