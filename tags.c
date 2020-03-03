/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tags.c
 * Author: Orbis Terrae
 * 
 * Created on February 19, 2018, 11:09 AM
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include "tags.h"
#include "GPMF_mp4reader.h"
#include "GPMF_parser.h"
#ifndef GPS_H
#include "GPS.h"
#endif


const char *strs_vmode[] = {"Video", "Timelapse Video", "Looping", "Video/Photo"};
const char *strs_fov[] = {"Wide", "Medium", "Narrow"};

const char *strs_orient[] = {"Up", "Down"};

const char *strs_OnOff[] = {"On", "Off"};
const char *strs_OffOn[] = {"Off", "On"};

const char *strs_pt_wb[] = {"Auto*", "3000K", "5500K", "6500K", "Native"};
const char *strs_pt_color[] = {"Flat", "GoPro Color*"};
const char *strs_pt_iso_video[] = {"6400*", "3200", "1600", "800", "400"};
const char *strs_pt_sharpness[] = {"Low", "Medium", "High*"};
const char *strs_pt_ev[] = {"+2", "+1.5", "+1", "+0.5", "0", "-0.5", "-1", "-1.5", "-2"};


int set_file_tags_name_duration(file_tags_struct* file_tags, char* new_name, int nb_data){
    file_tags->duration_sec = nb_data;
    sprintf(file_tags->file_name, "%s", new_name);
    strncpy(file_tags->short_name, mybasename(new_name), MAX_SHORT);
    return 0;
}

int copy_tags(file_tags_struct* dest_tags, int* tags, int nb_tags){
    dest_tags ->nb_tags = nb_tags;
    if (nb_tags>0){
        dest_tags->tags = malloc(sizeof(int)*nb_tags);
        if(dest_tags->tags == NULL){
            perror("malloc failed");
        }
        else{
            if(debug) printf("memory allocated for %d tags\n", nb_tags);
        }
    }
    for(int i = 0; i<nb_tags; i++){
        dest_tags->tags[i] = tags[i];
    }
    return 0;
}

int init_file_tags(file_tags_struct* file_tags, char* filename, int index) {
    file_tags->frame_rate = 30;
    file_tags->index = index;
    file_tags->nb_tags = 0;
    file_tags->x_resolution = 1920;
    file_tags->y_resolution = 1080;
    file_tags->duration_frame = 0;
    file_tags->duration_sec = 0;
    strncpy(file_tags->file_name, filename, sizeof(file_tags->file_name));
    file_tags->time_scale = 0;
    file_tags->short_name = malloc(MAX_SHORT*sizeof(char));
    strncpy(file_tags->short_name, mybasename(filename),MAX_SHORT);
    file_tags->tags = malloc(sizeof (int)*MAX_TAGS);
    
    return 0;
}

int release_file_tags(file_tags_struct* file_tags) {
    if(file_tags->nb_tags >0){
        if(file_tags->tags != NULL) free(file_tags->tags);
        if(debug) printf("memory released for %d tags\n", file_tags->nb_tags);
    }
    if(file_tags->short_name != NULL) free(file_tags->short_name);
    return 0;
}

void print_tags(file_tags_struct* tags) {
    for (int i = 0; i < nb_files; i++) {
        if(debug) printf("TAG[%d:%d] %s\n", i, tags[i].index, tags[i].short_name);
        //        printf("TAG[%d]: file %s\n",tags[i].index, tags[i].file_name);
        //        printf("TAG[%d:%d]: dir \"%s\"  name \"%s\"\n",i,tags[i].index, tags[i].dir_name, tags[i].short_name);
        if(debug) 
            printf("TAG[%d:%d]: timescale:%d %.2ffps \n", i, tags[i].index, tags[i].time_scale, tags[i].frame_rate);
        if(debug) 
            printf("TAG[%d:%d]: %d highlights within %lds duration\n\n", i, tags[i].index, tags[i].nb_tags, tags[i].duration_sec);
    }
    printf("TAG %s\t%d HiLigth\n",tags->short_name, tags->nb_tags);
    printf("\tframe rate:%.2ffps duration:%lds\n", tags->frame_rate,  tags->duration_sec);
    printf("\tresolution: %dx%d pixels\n\n", tags->x_resolution,  tags->y_resolution);
}
  

int parseMP4_tags(file_tags_struct* file_tags) {
    HMMT_struct hmmt;
    SETT_struct sett;
    int ret;
    int local_nb_data = 0;
    int local_tags[MAX_TAGS];

    OpenGPMFSource(file_tags->file_name);
    CloseGPMFSource();
    file_tags->duration_sec = getGPMF_videolength();
    file_tags->time_scale = getMDTimeScale();
    if (file_tags->time_scale == 90000)
        file_tags->frame_rate = (float)(30000.0/1001.0); //29.970
    if (file_tags->time_scale == 120000)
        file_tags->frame_rate = (float)(120000.0/1001.0);  //119.880
    if (file_tags->time_scale == 60000)
        file_tags->frame_rate = (float)(60000.0/1001.0);  //59.940
    if (file_tags->time_scale == 48000)
        file_tags->frame_rate = (float)(48000.0/1001.0);  //47.952
    if (file_tags->time_scale == 24000)
        file_tags->frame_rate = (float)(24000.0/1001.0);  //23.976
   if (debug){
       printf("-- GMPF Duration %f\n",getGPMF_videolength());
       printf("-- GMPF MetaSize Count %d - MetaDataLength %f\n", getMetasize_count(), getMetadatalength());
       printf("-- MD Duration %ld\n",getMDDuration());
       printf("-- MD time_scale %ld\n",getMDTimeScale());
       printf("-- Frame rate %f (or %f) opposite %f\n",((float)getMDDuration()/(float)getMDTimeScale()),100*(float)getGPMF_videolength()/(float)getMDTimeScale(), ((float)getMDTimeScale()/(float)getMDDuration()));        
       printf("-- Frame rate %f \n",(float)getMetadatalength() / ((float)getMDDuration() / (float)getMDTimeScale()));
       printf("-- indexcount %d\n",getIndexcount());
       printf("-- ClockDemon %ld\n",getClockdemon());
       printf("-- ClockCount %ld\n",getClockcount());
       printf("-- TrakClockDemon %ld\n",getTrakClockdemon());
       printf("-- TrakClockCount %ld\n",getTrakClockcount());
       printf("-- MetaClockDemon %ld\n",getMetaClockdemon());
       printf("-- MetaClockCount %ld\n",getMetaClockcount());
   }

    OpenGPMFSourceUDTA_HMMT(file_tags->file_name, &hmmt);
    if (hmmt.nb > 0) {
        local_nb_data = hmmt.nb;
        //file_tags->nb_tags = hmmt.nb;
        for (int i = 0; i < hmmt.nb; i++) {
            local_tags[i] = hmmt.samples[i];
        }
        copy_tags(file_tags, local_tags, local_nb_data);
    }
    CloseGPMFSource();
    OpenGPMFSourceUDTA_SETT(file_tags->file_name, &sett);
    CloseGPMFSource();
    //    OpenGPMFSourceUDTA_FIRM(file_tags->file_name);
    //    CloseGPMFSource();


    unsigned int mode = sett.set1 & 0xf;
    unsigned int submode = (sett.set1 >> 4) & ((1 << 4) - 1);
    unsigned int timelapse_rate = (sett.set1 >> 8) & ((1 << 4) - 1);
    unsigned int orientation = (sett.set1 >> 16) & ((1 << 1) - 1);
    unsigned int spotmeter = (sett.set1 >> 17) & ((1 << 1) - 1);
    unsigned int protune = (sett.set1 >> 30) & ((1 << 1) - 1);
    //unsigned int white_bal = 0;

    // setting 2
    unsigned int fov = (sett.set2 >> 1) & ((1 << 2) - 1);
    unsigned int lowlight = (sett.set2 >> 4) & ((1 << 1) - 1);
    unsigned int superview = (sett.set2 >> 5) & ((1 << 1) - 1);
    unsigned int protune_sharpness = (sett.set2 >> 6) & ((1 << 2) - 1);
    unsigned int protune_color = (sett.set2 >> 8) & ((1 << 1) - 1);
    unsigned int protune_iso = (sett.set2 >> 9) & ((1 << 3) - 1);
    unsigned int protune_ev = (sett.set2 >> 0xc) & ((1 << 4) - 1);
    unsigned int protune_wb = (sett.set2 >> 0x10) & ((1 << 2) - 1);
    unsigned int broadcast_privacy = (sett.set2 >> 0x12) & ((1 << 2) - 1);

    if (debug) {
        // setting 3
        unsigned int media_type = sett.set3;
        printf("(0x%x 0x%x 0x%x)\n\n", sett.set1, sett.set2, sett.set3);

        // 0 video
        if (mode) {
            printf("\tmode:\t\t\t%d\n", mode);
            printf("\tsubmode:\t\t%d\n", submode);
        } else {
            printf("\tmode:\t\t\t%s\n", DecipherValue(strs_vmode, arraysize(strs_vmode), submode));
        }


        printf("\torientation:\t\t%s\n", DecipherValue(strs_orient, arraysize(strs_orient), orientation));
        printf("\tspotmeter:\t\t%s\n", DecipherValue(strs_OffOn, arraysize(strs_OffOn), spotmeter));

        //printf("white_bal (Indicates Color Correction Matrix not applied): %d\n",white_bal);
        printf("\tfov:\t\t\t%s\n", DecipherValue(strs_fov, arraysize(strs_fov), fov));

        printf("\tlowlight:\t\t%s\n", DecipherValue(strs_OffOn, arraysize(strs_OffOn), lowlight));
        printf("\tsuperview:\t\t%s\n", DecipherValue(strs_OffOn, arraysize(strs_OffOn), superview));

        printf("\tprotune:\t\t%s\n", DecipherValue(strs_OffOn, arraysize(strs_OffOn), protune));
        if (protune) {
            printf("\tprotune_wb:\t\t%s\n", DecipherValue(strs_pt_wb, arraysize(strs_pt_wb), protune_wb));
            printf("\tprotune_color:\t\t%s\n", DecipherValue(strs_pt_color, arraysize(strs_pt_color), protune_color));
            printf("\tprotune_iso:\t\t%s\n", DecipherValue(strs_pt_iso_video, arraysize(strs_pt_iso_video), protune_iso));
            if (!protune_iso)
                printf("\tWARNING: ISO can also be set to 800 or 3200. It's not set in firmware for these values!\n");
            printf("\tprotune_sharpness:\t%s\n", DecipherValue(strs_pt_sharpness, arraysize(strs_pt_sharpness), protune_sharpness));
            printf("\tprotune_ev:\t\t%s\n", DecipherValue(strs_pt_ev, arraysize(strs_pt_ev), protune_ev));
        }
        printf("-----------------------------------------------\n\n");
    }

    return 0;
}