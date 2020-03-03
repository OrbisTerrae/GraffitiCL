/*! @file GraffitiCLI.c
 *
 *  @brief Extract GoPro TAGS from an MP4
 *
 *  @version 0.1
 *
 *  (C) Copyright 2018 OrbisTerrae
 *	
 *  Licensed under either:
 *  - Apache License, Version 2.0, http://www.apache.org/licenses/LICENSE-2.0  
 *  - MIT license, http://opensource.org/licenses/MIT
 *  at your option.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
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
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "tags.h"
#include "FCPXML.h"
#include "GPS.h"
#include "GPX.h"
#include "KML.h"
#include "stop.h"
#include "SRT.h"

#define USAGE "Usage: %s [-h] [-v] [-d] [-n] [-g] [-k] [-z] [-f] [-M] [-t Temperature] [-r HearRate] [-o Output] Input_file\n[-h] help - display this usage\n[-v] verbose - display verbose information to be used in GUI\n[-d] debug - display debug information\n[-n] no output - simlation only\n[-g] generate GPX output\n[-k] generate KML output\n[-z] generate KMLZ output including pictures\n[-f] generate FCPXML output\n[-t Temperature] initial temperature in °C - varies with altitude\n[-r HearRate] Heart Rate in BPM - cycle around central point\n[-M] process successive GOPRO MP4 files as one\n[-o Output] optional output file - if not specified, default to the input path\ninput file: currently support GoPro Hero 5 MP4 and DJI STR files\n"
// Version defined in FCPXML.h

extern void PrintGPMF(GPMF_stream *ms);


// You must free the result if result is non-NULL.

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins; // the next insert point
    char *tmp; // varies
    int len_rep; // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count; // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

int isSuccessiveGoProFile(char* name) {
    char* word = "GP0";
    char* result;
    int ret;

    result = strstr(name, word);
    if (result != NULL) {
        ret = strcmp(result, word);
        if (ret != 0) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

int isMP4File(char* name) {
    char* word = ".MP4";
    char* result;

    result = strstr(name, word);
    if (result != NULL) {
        if (strcmp(result, word) != 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int isSRTFile(char* name) {
    char* word = ".SRT";
    char* result;

    result = strstr(name, word);
    if (result != NULL) {
        if (strcmp(result, word) != 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int calc_succ_file(successive_gps_file* succ_gps, file_GPS_struct* file_gps, int nb_files) {
    int index = 0;
    int nb_fsucc = 0;

    for (int i = 1; i < nb_files; i++) {
        succ_gps->index_files[i] = 0;
        if (isSuccessiveGoProFile(file_gps[i].file_name) && isMP4File(file_gps[i].file_name)) {
            if (nb_fsucc == 0) {
                index = i;
            }
            nb_fsucc += 1;
            succ_gps->index_files[nb_fsucc] = i;
        }
    }
    if (nb_fsucc > 0) {
        succ_gps->nb_files = nb_fsucc + 1;
        succ_gps->first_file = str_replace(file_gps[index].file_name, "GP01", "GOPR");
        int ret = 0;
        for (int j = 0; j < nb_files; j++) {
            ret = strcmp(file_gps[j].file_name, succ_gps->first_file);
            if (ret == 0) {
                succ_gps->index_files[0] = j;
            }
        }
        if (debug) {
            for (int k = 0; k <= nb_fsucc; k++) {
                printf("succ file [%d] index %d \n", k, succ_gps->index_files[k]);
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {

    char* f_input;
    char* version = VERSION;
    int c;
    file_tags_struct file_tags[MAX_FILE];
    file_stops_struct file_stops[MAX_FILE];
    file_GPS_struct file_gps[MAX_FILE];
    successive_gps_file succ_gps;
    GPS_atom GPS[MAX_GPS_POINTS];
    SRT_atom SRT[MAX_GPS_POINTS];
    GPS_table GPS_matrix;
    char* f_output = NULL;
    DIR *dir;
    char filename[MAX_PATH];
    nb_files = 0;
    int MP4files = 0;
    int SRTfiles = 0;

    verbose = 0;
    debug = 0;
    nooutput = 0;
    GPX = 0;
    KML = 0;
    KMLZ = 0;
    FCPXML = 0;
    ML = 0;
    init_temp = INIT_TEMP;
    hr_base = HR_BASE;
    hasTemp = 0;
    hasHR = 0;

    /* ------- processing the command line ------- */

    while ((c = getopt(argc, argv, "advngkfzhMt:r:o:")) != -1)
        switch (c) {
            case 'd':
                debug = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'g':
                GPX = 1;
                break;
            case 'k':
                KML = 1;
                break;
            case 'z':
                KMLZ = 1;
                break;
            case 'f':
                FCPXML = 1;
                break;
            case 'M':
                ML = 1;
                break;
            case 'a':
                verbose = 1;
                GPX = 1;
                KML = 1;
                FCPXML = 1;
                hasTemp = 1;
                hasHR = 1;
                ML = 1;
                break;
            case 't':
                hasTemp = 1;
                init_temp = atoi(optarg);
                break;
            case 'r':
                hasHR = 1;
                hr_base = atoi(optarg);
                break;
            case 'o':
                f_output = optarg;
                break;
            case 'n':
                nooutput = 1;
                break;
            case 'h':
                fprintf(stderr, USAGE, argv[0]);
                return 1;
            case '?':
                if (optopt == 't' || optopt == 'r' || optopt == 'o')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
                return 1;
            default:
                abort();
        }
    if (optind < argc) {
        f_input = argv[argc - 1];

    } else {
        fprintf(stderr, USAGE, argv[0]);
        return 1;
    }

    if (verbose) printf("Copyright © 2017-2018 Orbis Terrae %s v%s\n\n", SW_NAME, version);

    /* ------- opening MP4 files and populating the structure with data from video files ------- */
    if ((dir = opendir(f_input)) != NULL) {
        closedir(dir); // not needed for scandir ?

        /* processing all the files and directories within directory */
        struct dirent **namelist;
        int n;
        int i = 0;
        int nb_succ = 0;
        int error;

        n = scandir(f_input, &namelist, NULL, alphasort);
        if (n < 0) {
            perror("scandir");
            exit(EXIT_FAILURE);
        }

        while (i < n) { // detect how much files we need to process to allocate memory
            if (isMP4File(namelist[i]->d_name)) {
                MP4files++;
                if (isSuccessiveGoProFile(namelist[i]->d_name)) {
                    nb_succ++;
                }
            }
            if (isSRTFile(namelist[i]->d_name)) {
                SRTfiles++;
            }
            ++i;
        }

        if (nb_succ > 0) {
            succ_gps.nb_files = nb_succ + 1;
            if (debug) printf("Successive GoPro Files %d\n", succ_gps.nb_files);
        }

        if (MP4files + SRTfiles > 0) { // more than one file, let's allocate the memory to store the file struct

            GPS_matrix.nb_file = MP4files + SRTfiles;
            GPS_matrix.GPS = malloc(sizeof (GPS_list) * (MP4files + SRTfiles));
            if (GPS_matrix.GPS == NULL) {
                perror("malloc matrix/file");
            } else
                if (debug) printf("memory for %d MP4 & %d SRT files allocated ------------\n", MP4files, SRTfiles);
        }
        
        
        if(debug) printf("################################################ entering parsing input file section ######################### \n");
        i = 0;
        while (i < n) { // parse the files, get the data into the initial table.
            if (isMP4File(namelist[i]->d_name) || isSRTFile(namelist[i]->d_name)) {

                if (f_input[strlen(f_input) - 1] == '/') {
                    error = snprintf(filename, strlen(f_input) + strlen(namelist[i]->d_name) + 1, "%s%s", f_input, namelist[i]->d_name);
                } else {
                    error = snprintf(filename, strlen(f_input) + strlen(namelist[i]->d_name) + 2, "%s/%s", f_input, namelist[i]->d_name);
                }
                if (debug) printf("[%d] filename %s(%zd)  f_input %s(%zd) namelist %s(%zd) \n", error,
                        filename, strlen(filename),
                        f_input, strlen(f_input),
                        namelist[i]->d_name, strlen(namelist[i]->d_name));

                if (GPX || KML) {
                    //init_file_gps(&file_gps[nb_files], filename, nb_files + 1);
                    init_file_gps(&file_gps[nb_files], filename, nb_files);

                    if (isMP4File(namelist[i]->d_name)) {
                        // it's a GOPRO file, parse it to get gps data
                        init_GPS(&GPS[nb_files], file_gps[nb_files].samples);

                        parseMP4_GPS(&file_gps[nb_files], &GPS[nb_files]);
                        calc_GPS_stats(&file_gps[nb_files], &GPS[nb_files], 1);

                        print_gps_file(&file_gps[nb_files]);
                    }
                    if (isSRTFile(namelist[i]->d_name)) {
                        // DJI file, parse it and convert to GPS struct
                        if (parseSRT(&file_gps[nb_files], &SRT[nb_files]) > 0) {
                            // unlikely to be a DJI file.
                            convertSRTtoGPS(&SRT[nb_files], &GPS[nb_files], file_gps[nb_files].samples);
                            calc_GPS_init(&GPS[nb_files], file_gps[nb_files].samples);
                            calc_GPS_stats(&file_gps[nb_files], &GPS[nb_files], 0);
                            print_gps_file(&file_gps[nb_files]);
                        }
                    }

                    // we know how how much data to process. Let's allocate memory and copy data so we can dump them later.
                    GPS_matrix.GPS[nb_files].nb_data = file_gps[nb_files].samples;
                    GPS_matrix.GPS[nb_files].GPS = malloc(sizeof (GPS_atom) * file_gps[nb_files].samples);

                    if (GPS_matrix.GPS[nb_files].GPS == NULL) {
                        perror("malloc unit/list");
                    } else {
                        if (debug) printf("memory for %d GPS data (s) allocated---------------------\n", 
                                GPS_matrix.GPS[nb_files].nb_data);
                    }
                    //copy_GPS(GPS_matrix.GPS[nb_files].GPS, &GPS[nb_files], file_gps[nb_files].samples);
                    copy_GPS(GPS_matrix.GPS[nb_files].GPS, GPS, file_gps[nb_files].samples);
                    
                    if (debug) printf("GPS copied into GPS struct [%d] ---------------------\n", nb_files);

                    init_stops(&file_stops[nb_files], nb_files);
                    calc_stops(&file_stops[nb_files], &GPS[nb_files], file_gps[nb_files].samples);
                    print_stops(&file_stops[nb_files]);
                }
                
                if (FCPXML) {
                    // need to process even if the input file is SRT (ie, no TAGS), as we need to populate the tags structure
                    init_file_tags(&file_tags[nb_files], filename, nb_files + 1);
                    if (isMP4File(filename)) {
                        parseMP4_tags(&file_tags[nb_files]);
                    } else if (isSRTFile(filename)) {
                        file_tags[nb_files].duration_sec = file_gps[nb_files].samples;
                    }
                    print_tags(&file_tags[nb_files]);
                }
                nb_files++;

            } else {
                if (debug) printf("Discard: %s\n", namelist[i]->d_name);
            }
            free(namelist[i]);
            ++i;
        }
        free(namelist);
    } else {
        /* ------- only one file to process ------- */
        if (isMP4File(f_input) || isSRTFile(f_input)) {
            
            GPS_matrix.nb_file = 1;
            GPS_matrix.GPS = malloc(sizeof (GPS_list) * GPS_matrix.nb_file);
            
            if (GPX || KML) { // no need to copy data, let's work with the initial table directly.
                init_file_gps(&file_gps[0], f_input, nb_files + 1);
                if (isMP4File(f_input)) {
                    // it's a GOPRO file, parse it to get gps data
                    init_GPS(&GPS[0], file_gps[0].samples);
                    parseMP4_GPS(&file_gps[0], &GPS[0]);
                    calc_GPS_stats(&file_gps[0], &GPS[0], 1);
                    print_gps_file(&file_gps[0]);
                }
                if (isSRTFile(f_input)) {
                    // DJI file, parse it and convert to GPS struct
                    parseSRT(&file_gps[0], &SRT[nb_files]);
                    convertSRTtoGPS(&SRT[nb_files], &GPS[0], file_gps[0].samples);
                    calc_GPS_init(&GPS[0], file_gps[0].samples);
                    calc_GPS_stats(&file_gps[0], &GPS[0], 0);
                    print_gps_file(&file_gps[nb_files]);
                }

                GPS_matrix.GPS[0].nb_data = file_gps[0].samples;
                GPS_matrix.GPS[0].GPS = malloc(sizeof (GPS_atom) * file_gps[0].samples);

                if (GPS_matrix.GPS[0].GPS == NULL) {
                    perror("malloc unit/list");
                } else {
                    if (debug) printf("memory for %d GPS data (s) allocated---------------------\n", 
                            GPS_matrix.GPS[0].nb_data);
                }
                /*
                printf("debug GPS[0].lat %f (@ &GPS[0] %d / GPS[0] %d / GPS %d)\n", GPS[0].lat, &GPS[0], GPS[0], GPS);
                printf("debug GPS_matrix. nb_file %d GPS_matrix.GPS[0].nb_data %d\n", 
                        GPS_matrix.nb_file, GPS_matrix.GPS[0].nb_data); 
                printf("debug (@ &GPS_matrix.GPS[0].GPS[0] %d\n", 
                        &GPS_matrix.GPS[0].GPS[0]);
                printf("debug GPS_matrix.GPS[0].GPS %d)\n",
                        GPS_matrix.GPS[0].GPS); 
                printf("debug GPS_matrix.GPS[0].GPS[0].lat %f)\n",
                        GPS_matrix.GPS[0].GPS[0].lat);
                //copy_GPS(&GPS_matrix.GPS[0].GPS[0], &GPS[0], file_gps[0].samples);
                */
                copy_GPS(GPS_matrix.GPS[0].GPS, GPS, file_gps[0].samples);
                if (debug) printf("GPS copied into GPS struct [%d] ---------------------\n", 0);

                init_stops(&file_stops[0], nb_files + 1);
                calc_stops(&file_stops[0], &GPS[0], file_gps[0].samples);
                print_stops(&file_stops[0]);
            }
            if (FCPXML) {
                // need to process even if the input file is SRT (ie, no TAGS), as we need to populate the tags structure
                init_file_tags(&file_tags[0], f_input, nb_files + 1);
                if (isMP4File(f_input)) {
                    parseMP4_tags(&file_tags[0]);
                } else if (isSRTFile(f_input)) {
                    file_tags[0].duration_sec = file_gps[0].samples;
                }
                print_tags(&file_tags[0]);
            }
            nb_files = 1;
        } else {
            fprintf(stderr, USAGE, argv[0]);
            return EXIT_FAILURE;
        }
    }

    /* ------- We know have all the data in the structures - let's dump into FCPXML/GPX/KML ------- */
        
    if(debug) printf("################################################ entering writing output file section ######################### \n");

    if (debug) {
        printf("Transforming GPMF into selected options\n");
        for (int i = 0; i < nb_files; i++) {
            printf("#%d [%d] %s\n", i, file_gps[i].file_nb, file_gps[i].file_name);
            print_gps_file(&file_gps[i]);
        }
    }

    for (int i = 0; i < nb_files; i++) {
        if (!nooutput) {
            if (FCPXML) {
                if (debug) printf("before writeFCPXML %s------------------------------\n", file_tags[i].file_name);
                writeFCPXML(&file_gps[i], GPS_matrix.GPS[i].GPS, &file_tags[i], &file_stops[i]);
            }
            if (GPX) {
                if (debug) printf("before writeGPX %s------------------------------\n", file_gps[i].file_name);
                writeGPX(&file_gps[i], GPS_matrix.GPS[i].GPS);
            }
            if (KML) {
                if (debug) printf("before writeKML %s ; %s------------------------------\n", file_gps[i].file_name, file_gps[i].short_name);
                writeKML(&file_gps[i], GPS_matrix.GPS[i].GPS, &file_tags[i], &file_stops[i]);
            }
        }
    }
    // --------------------------------------------------------------------------------------------------------------------------------------
    /* ------- Processing multiple and successive gopro files GOPR0XX.MP4 GP01XX.MP4 ------------------------------------------ */
    if (nb_files > 1 && ML) {
        if(debug) printf("################################################ entering Multiple GoPro file section ######################### \n");
        if (GPX || KML) {
            calc_succ_file(&succ_gps, file_gps, nb_files);
        }
        // successive GOPRO files - new tables with all data as one big file
        // using these newly created tables with combined data for the output routines
        if (succ_gps.nb_files > 0) {
            GPS_list GPS_succ;
            file_GPS_struct file_gps_succ;
            file_tags_struct succ_tags;
            file_stops_struct succ_stops;
            int file_index = 0;
            char* new_name;

            // happen ML to the end of the name
            new_name = str_replace(succ_gps.first_file, ".MP4", "_ML.MP4");
            succ_gps.new_name = new_name;
            GPS_succ.nb_data = 0;

            for (int i = 0; i < succ_gps.nb_files; i++) {
                file_index = succ_gps.index_files[i];
                GPS_succ.nb_data += GPS_matrix.GPS[file_index].nb_data;
                if(debug) printf("file %d has %d gps samples - total %d atoms\n", 
                        file_index, 
                        GPS_matrix.GPS[succ_gps.index_files[i]].nb_data, 
                        GPS_succ.nb_data);
            }
            if (GPS_succ.nb_data > 0) {
                GPS_succ.GPS = malloc(sizeof (GPS_atom) * (GPS_succ.nb_data));
                if (GPS_succ.GPS == NULL) {
                    perror("malloc matrix successive file");
                } else
                    if (debug) printf("memory for %d successive GPS data allocated----------------------------\n", GPS_succ.nb_data);
            }

            // copy all the GPS data into one big table
            if (debug) printf("copying %d data to the main matrix file ----------------------------\n", GPS_succ.nb_data);
            int succ_nb_data =0;
            for (int i = 0; i < succ_gps.nb_files; i++) {
                file_index = succ_gps.index_files[i];
                if (debug) printf("copying %d GPS data from file %d at position %d-%d\n", 
                        GPS_matrix.GPS[file_index].nb_data, 
                        file_index, 
                        succ_nb_data,
                        succ_nb_data + GPS_matrix.GPS[file_index].nb_data -1);
                copy_GPS(&GPS_succ.GPS[succ_nb_data], &GPS_matrix.GPS[file_index].GPS[0], GPS_matrix.GPS[file_index].nb_data);
                succ_nb_data += GPS_matrix.GPS[file_index].nb_data;
            }
            // for some unknown reason, first element of the table is always zero. 
            // assigning second element to element 1
            //copy_GPS(&GPS_succ.GPS[0], &GPS_succ.GPS[1],1);
            
            //print_GPS(GPS_succ.GPS, GPS_succ.nb_data);
            init_file_gps(&file_gps_succ, succ_gps.first_file, 0);
            set_file_gps_name_samples(&file_gps_succ, new_name, GPS_succ.nb_data);
            calc_GPS_stats(&file_gps_succ, GPS_succ.GPS, 1);
  
            // copy all the stops into one big table ---------------------------------------------------------
            if (debug) printf("calculating stops on %d data ----------------------------\n", GPS_succ.nb_data);
            init_stops(&succ_stops, 0);
            for (int i = 0; i < succ_gps.nb_files; i++) {
                file_index = succ_gps.index_files[i];
                succ_stops.nb_stops += file_stops[file_index].nb_stops;
                if (debug) printf("file %d has %d stops - total %d stops\n", 
                        file_index, 
                        file_stops[file_index].nb_stops,  
                        succ_stops.nb_stops);
            }
            // recalculating stops based the unified ML GPS data  - no need to recopy stops from each files, simply recalc
            calc_stops(&succ_stops, GPS_succ.GPS, GPS_succ.nb_data);
            if (verbose) print_stops(&succ_stops);

            /* // - no need to recopy stops from each files, simply recalculate all of the them
            int total_stops = 0;
            int base_stops = 0;
            for (int i = 0; i < succ_gps.nb_files; i++) {
                // algo to compose all tags with the right timing into the big table
                file_index = succ_gps.index_files[i];
                for (int nb_data = 0; nb_data < file_stops[file_index].nb_stops; nb_data++) {
                    if (debug) printf("adding file_stops[%d].stops[%d] %d to succ_stops[%d]\n", file_index, nb_data, 
                            file_stops[file_index].stop_duration[nb_data] + base_stops, total_stops);
                    
                    succ_stops.stop_duration_index[total_stops] = file_stops[file_index].stop_duration_index[nb_data] + base_stops;
                    succ_stops.stop_duration[total_stops] = file_stops[file_index].stop_duration[nb_data];
                    total_stops++;
                }
            }
            */
            
            // copy all the tags into one big table ---------------------------------------------------------
            if (debug) printf("calculating tags on %d data ----------------------------\n", GPS_succ.nb_data);
            init_file_tags(&succ_tags, succ_gps.first_file, 0);
            set_file_tags_name_duration(&succ_tags, new_name, GPS_succ.nb_data);

            for (int i = 0; i < succ_gps.nb_files; i++) {
                file_index = succ_gps.index_files[i];
                succ_tags.nb_tags += file_tags[file_index].nb_tags;
                if (debug) printf("file %d has %d tags (%lds) - total %d tags\n", file_index, file_tags[file_index].nb_tags, file_tags[file_index].duration_sec, succ_tags.nb_tags);
            }
            if (succ_tags.nb_tags > MAX_TAGS) {
                if (succ_tags.tags != NULL) {
                    free(succ_tags.tags);
                }
                succ_tags.tags = malloc(sizeof (int)*succ_tags.nb_tags);
                if (debug) printf("allocating memory for %lu tags\n",sizeof (int)*succ_tags.nb_tags);
                //perror("too many tags in the successive files");
            }
            succ_tags.time_scale = file_tags[succ_gps.index_files[0]].time_scale;
            succ_tags.x_resolution = file_tags[succ_gps.index_files[0]].x_resolution;
            succ_tags.y_resolution = file_tags[succ_gps.index_files[0]].y_resolution;
            int total_data = 0;
            int base_tags = 0;
            for (int i = 0; i < succ_gps.nb_files; i++) {
                // algo to compose all tags with the right timing into the big table
                file_index = succ_gps.index_files[i];
                for (int nb_data = 0; nb_data < file_tags[file_index].nb_tags; nb_data++) {
                    if (debug) printf("adding file_tag[%d].tags[%d] %d to succ_tags[%d]\n", file_index, nb_data, 
                            file_tags[file_index].tags[nb_data] + base_tags, total_data);
                    
                    succ_tags.tags[total_data] = file_tags[file_index].tags[nb_data] + base_tags;
                    total_data++;
                }
                base_tags += file_tags[file_index].duration_sec * 1000;
            }
            // write output for the big tables of successive files  ---------------------------------------------------------
            if (!nooutput) {
                if (FCPXML) {
                    if (debug) printf("before writeFCPXML_ML %s------------------------------\n", succ_gps.new_name);
                    writeFCPXML(&file_gps_succ, GPS_succ.GPS, &succ_tags, &succ_stops);
                }
                if (GPX) {
                    if (debug) printf("before writeGPX_ML %s------------------------------\n", succ_gps.new_name);
                    writeGPX(&file_gps_succ, GPS_succ.GPS);
                }
                if (KML) {
                    if (debug) printf("before writeKML_ML %s ------------------------------\n", succ_gps.new_name);
                    writeKML(&file_gps_succ, GPS_succ.GPS, &succ_tags, &succ_stops);
                }
            }
            // cleanup memory related to successives files
            release_file_tags(&succ_tags);
            release_file_gps(&file_gps_succ);
            if (debug) printf("memory for %d GPS data free up-------------------------\n", GPS_succ.nb_data);
            free(GPS_succ.GPS);
        }
    }

    // --------------------------------------------------------------------------------------------------------------------------------------
    /* ------- Closing files and cleanup ------- */

    if (succ_gps.first_file != NULL) {
        free(succ_gps.first_file);
    }
    if(debug) printf("################################################ entering cleanup section ######################### \n");
    if (debug) printf("before cleanup------------------------------\n");
    if (FCPXML) {
        for (int i = 0; i < nb_files; i++) {
            if (debug) printf("memory for %d TAGS data free up %s-------------------------\n", file_tags[i].nb_tags, file_tags[i].short_name);
            release_file_tags(&file_tags[i]);
        }
    }
    if (GPX || KML) {
        for (int i = 0; i < nb_files; i++) {
            release_file_gps(&file_gps[i]);
            release_stops(&file_stops[i]);
        }
    }
    for (int i = 0; i < GPS_matrix.nb_file; i++) {
        if ((GPS_matrix.GPS[i].GPS) != NULL) {
            if (debug) printf("memory for %d GPS data free up-------------------------\n", GPS_matrix.GPS[i].nb_data);
            free(GPS_matrix.GPS[i].GPS);
        }
    }
    if (GPS_matrix.GPS != NULL) {
        if (debug) printf("memory for %d files free up------------------------------\n", GPS_matrix.nb_file);
        free(GPS_matrix.GPS);
    }
    return 0;
}
