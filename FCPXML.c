/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FCPXML.c
 * Author: Orbis Terrae
 * 
 * Created on February 20, 2018, 3:49 PM
 */

#include "FCPXML.h"
#include "tags.h"
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
#ifndef GPMF_MP4READER_H
#include "GPMF_mp4reader.h"
#endif


int getYear() {
    int year;
    struct tm *current;
    time_t timenow;
    time(&timenow);
    current = localtime(&timenow);
    year = current->tm_year + 1900;
    return year;
}

int writeFCPXML_header(FILE* f) {
    fprintf(f,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!DOCTYPE fcpxml>\n<fcpxml version=\"1.8\">\n"
            "  <resources>\n");

    fprintf(f, "   <!-- Graffiti v%s - Copyright © 2015-%d Orbis Terrae - All rights reserved.-->\n", VERSION, getYear());
    return 0;
}

int writeFCPXML_TL_closer(FILE* f, int duration) {
    fprintf(f, ""
            "    <effect id=\"TextFormatFileId\" name=\"Basic Title\" uid=\".../Titles.localized/Bumper:Opener.localized/Basic Title.localized/Basic Title.moti\"/>\n"
            "    <effect id=\"MountId\" name=\"Placeholder\" uid=\".../Generators.localized/Elements.localized/Placeholder.localized/Placeholder.motn\"/>\n"
            "    <effect id=\"box\" name=\"Shapes\" uid=\".../Generators.localized/Elements.localized/Shapes.localized/Shapes.motn\"/>\n"
            "    <format id=\"VideoFormatId\" name=\"FFVideoFormatRateUndefined\"/>\n"
            "  </resources>\n"
            "  <library>\n"
            "    <event name=\"Orbis Terrae\">\n"
            "      <ref-clip ref=\"TAGS\" name=\"POI Timeline\" lane=\"1\" duration=\"%d\"/>\n"
            "    </event>\n"
            "    <event name=\"Orbis Terrae\">\n"
            "      <ref-clip ref=\"Overlay\" name=\"GPS Overlay\" lane=\"2\" duration=\"%d\"/>\n"
            "    </event>\n"
            "  </library>\n"
            "</fcpxml>\n", duration, duration);
    return 0;
}

int writeFCPXML_TAGS_closer(FILE* f, int duration) {
    fprintf(f, ""
            "    <effect id=\"TextFormatFileId\" name=\"Basic Title\" uid=\".../Titles.localized/Bumper:Opener.localized/Basic Title.localized/Basic Title.moti\"/>\n"
            "    <effect id=\"MountId\" name=\"Placeholder\" uid=\".../Generators.localized/Elements.localized/Placeholder.localized/Placeholder.motn\"/>\n" "     <effect id=\"box\" name=\"Shapes\" uid=\".../Generators.localized/Elements.localized/Shapes.localized/Shapes.motn\"/>\n"
            "    <format id=\"VideoFormatId\" name=\"FFVideoFormatRateUndefined\"/>\n"
            "  </resources>\n"
            "  <library>\n"
            "    <event name=\"Orbis Terrae\">\n"
            "      <ref-clip ref=\"TAGS\" name=\"POI Timeline\" duration=\"%d\"/>\n"
            "    </event>\n"
            "  </library>\n"
            "</fcpxml>\n", duration); 
   return 0;
}

int writeOverlay(FILE* f, int index, int year,int month,int day,int hour,int min,int secondes, 
        int heading, float speed, float alt, 
        float temp, int HR) {

    fprintf(f, "<title name=\"Basic Title: (%d)\" offset=\"%ds\" ref=\"TextFormatFileId\" duration=\"1s\" role=\"titles.Overlay\">\n", index, index);
    fprintf(f, "  <text>\n");
    fprintf(f, "    <text-style ref=\"line%d-1\">%02d:%02d:%04d - %02d:%02d:%02d</text-style>\n", index, day, month, year, hour, min, secondes);
    fprintf(f, "    <text-style ref=\"line%d-2\">\n%d° - %0.1fkm/h - ALT: %0.2fm</text-style>\n", index, heading, speed, alt);
    if (hasTemp || hasHR) {
        if (hasTemp && hasHR) {
            fprintf(f, "    <text-style ref=\"line%d-3\">\nTemp: %0.1f° - Heart rate: %dbpm </text-style>\n", index, temp, HR);
        } else {
            if (hasTemp) {
                fprintf(f, "    <text-style ref=\"line%d-3\">\nTemp: %0.1f°  </text-style>\n", index, temp);
            } else {
                if (hasHR) {
                    fprintf(f, "    <text-style ref=\"line%d-3\">\nHeart rate:% dbpm </text-style>\n", index, HR);
                }
            }
        }
    } else {
        fprintf(f, "    <text-style ref=\"line%d-3\">\nOrbis Terrae %s %s </text-style>\n", index, SW_NAME, VERSION);
    }

    fprintf(f, "  </text>\n");
    fprintf(f, "  <text-style-def id=\"line%d-1\">\n", index);
    fprintf(f, "    <text-style font=\"Avenir Next\" fontSize=\"30\" alignment=\"right\"  bold=\"0\" lineSpacing=\"10\" italic=\"0\"/>\n");
    fprintf(f, "  </text-style-def>\n");
    fprintf(f, "  <text-style-def id=\"line%d-2\">\n", index);
    fprintf(f, "    <text-style font=\"Avenir Next\" fontSize=\"30\" alignment=\"right\"  bold=\"1\" lineSpacing=\"10\" italic=\"0\"/>\n");
    fprintf(f, "  </text-style-def>\n");
    fprintf(f, "  <text-style-def id=\"line%d-3\">\n", index);
    fprintf(f, "    <text-style font=\"Avenir Next\" fontSize=\"30\" alignment=\"right\"  bold=\"0\" lineSpacing=\"10\" italic=\"0\"/>\n");
    fprintf(f, "  </text-style-def>\n");
    fprintf(f, "  <adjust-transform position=\"0 -7.08749\" anchor=\"-83.3333 27.7778\"/>\n");
    fprintf(f, "  <video name=\"Shapes\" lane=\"-1\" offset=\"0s\" ref=\"box\" duration=\"2s\" start=\"3600s\">\n");
    fprintf(f, "    <param name=\"Drop Shadow Opacity\" key=\"9999/988455508/1/208/211\" value=\"0\"/>\n");
    fprintf(f, "    <param name=\"Drop Shadow Distance\" key=\"9999/988455508/1/208/212\" value=\"0\"/>\n");
    fprintf(f, "    <param name=\"Drop Shadow Blur\" key=\"9999/988455508/1/208/213\" value=\"0\"/>\n");
    fprintf(f, "    <param name=\"Drop Shadow Angle\" key=\"9999/988455508/1/208/214\" value=\"0\"/>\n");
    fprintf(f, "    <param name=\"Outline Color\" key=\"9999/988455508/988455699/2/353/108/107\" value=\"1 1 1\"/>\n");
    fprintf(f, "    <param name=\"Fill Color\" key=\"9999/988455508/988455699/2/353/113/111\" value=\"0 0 0\"/>\n");
    fprintf(f, "    <param name=\"Shape\" key=\"9999/988461322/100/988461395/2/100\" value=\"4 (Rectangle)\"/>\n");
    fprintf(f, "    <param name=\"Fill\" key=\"9999/988461322/100/988464517/2/100\" value=\"1\"/>\n");
    fprintf(f, "    <param name=\"Roundness\" key=\"9999/988461322/100/988467054/2/100\" value=\"0.01\"/>\n");
    fprintf(f, "    <param name=\"Outline Width\" key=\"9999/988461322/100/988467855/2/100\" value=\"0\"/>\n");
    fprintf(f, "    <adjust-transform position=\"41.6667 -33.75\" scale=\"0.75 0.96\" anchor=\"-22.2222 4.86111\"/>\n");
    fprintf(f, "    <adjust-blend amount=\"0.60\"/>\n");
    fprintf(f, "  </video>\n");
    fprintf(f, "</title>\n");
    return 0;
}

int writePlaceholder_header(FILE* f, int i, int timeline_nb, int timeline_seq){
    fprintf(f, "\t<!-- TIMELINE %d EVENT %d SEQUENCE %d -->\n",i, timeline_nb, timeline_seq);
    return 0;
}

int writePlaceholder_closer(FILE* f,int timeline_nb, int timeline_seq){
    if(timeline_seq > 0) {
        // event detected but not written
        fprintf(f, "</title>\n");
    }
    return 0;
}

int writePlaceholder(FILE* f, int titleSeq, char* line, int offset, char* text, int videoBG, int timeline_nb, int timeline_seq, int isMarker) {
    if (timeline_nb >1 || timeline_seq >0){ 
        if(debug) printf("WritePlaceholder[%d] with %d elements (sequence %d)\n", titleSeq, timeline_nb, timeline_seq);
    }

    if (timeline_nb == 1) {
        if (!isMarker) {
            // only one element to write to the timeline, no need to nest elements
            fprintf(f, "<title name=\"Basic Title: (%d)\" offset=\"%ds\" ref=\"TextFormatFileId\" duration=\"1s\" role=\"titles.POI\">\n",
                    titleSeq, offset);
            fprintf(f, "  <param name=\"Position\" key=\"9999/999166631/999166633/1/100/101\" value=\"895 %d\"/>\n", -355);
            fprintf(f, "  <param name=\"Alignment\" key=\"9999/999166631/999166633/2/354/999169573/401\" value=\"2 (Right)\"/>\n");
            fprintf(f, "  <text>\n");
            fprintf(f, "    <text-style ref=\"line-%s%d\">", line, titleSeq);
            fprintf(f, "%s", text);
            fprintf(f, "</text-style>\n");
            fprintf(f, "  </text>\n");
            fprintf(f, "  <text-style-def id=\"line-%s%d\">\n", line, titleSeq);
            fprintf(f, "    <text-style font=\"Avenir Next\" fontSize=\"60\" alignment=\"right\"  bold=\"0\" lineSpacing=\"10\" italic=\"0\"/>\n");
            fprintf(f, "  </text-style-def>\n");
            fprintf(f, "    <video name=\"Placeholder\" lane=\"-1\" offset=\"0s\" ref=\"MountId\" duration=\"1s\" start=\"3600s\">\n");
            fprintf(f, "      <param name=\"Sky\" key=\"9999/987171866/100/987175410/2/100\" value=\"5 (None)\"/>\n");

            if (videoBG == 1)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"1 (Suburban)\"/>\n");
            if (videoBG == 2)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"2 (Rural)\"/>\n");
            if (videoBG == 3)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"3 (Mountains)\"/>\n");
            if (videoBG == 4)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"4 (Pastoral)\"/>\n");
            if (videoBG == 5)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"7 (Beach)\"/>\n");
            if (videoBG == 6)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"6 (Distant City)\"/>\n");
            if (videoBG == 7)fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"7 (Beach)\"/>\n");
            fprintf(f, "      <param name=\"People\" key=\"9999/987171866/100/987181336/2/100\" value=\"0 (0)\"/>\n");
            fprintf(f, "      <param name=\"View Notes\" key=\"9999/987171866/100/999117736/2/100\" value=\"1\"/>\n");
            fprintf(f, "      <param name=\"Text\" key=\"9999/987185090/987187752/2/369\" value=\"%s\"/>\n", line);
            fprintf(f, "      <param name=\"Size\" key=\"9999/987185090/987187752/5/987187756/3\" value=\"60\"/>\n");
            fprintf(f, "      <param name=\"Font\" key=\"9999/987185090/987187752/5/987187756/83\" value=\"23 0\"/>\n");
            fprintf(f, "    </video>\n");
        } else {

            /* <marker start='4644/30s' duration='1/48000s' value='beat_at_154.8' completed='0'/> */
            fprintf(f, "         <marker start='%ds' duration='1s' value='%s - %s' completed='0'/>\n",
                    titleSeq,
                    line, text
                    );
        }

    }
    else{
        if(!isMarker){
        // multiple elements to write on the time line, need to embed some xml
        if (timeline_seq == 0) {
            // this is the first instance of the multiple sequence
            fprintf(f, "<title name=\"Basic Title: (%d)\" offset=\"%ds\" ref=\"TextFormatFileId\" duration=\"1s\" role=\"titles.POI\">\n",
                    titleSeq, offset);
            fprintf(f, "  <param name=\"Position\" key=\"9999/999166631/999166633/1/100/101\" value=\"895 %d\"/>\n", -355);
            fprintf(f, "  <param name=\"Alignment\" key=\"9999/999166631/999166633/2/354/999169573/401\" value=\"2 (Right)\"/>\n");
            fprintf(f, "  <text>\n");
            fprintf(f, "    <text-style ref=\"line-%s%d\">", line, titleSeq);
            fprintf(f, "%s: %s", line, text);
            fprintf(f, "</text-style>\n");
            fprintf(f, "  </text>\n");
            fprintf(f, "  <text-style-def id=\"line-%s%d\">\n", line, titleSeq);
            fprintf(f, "    <text-style font=\"Avenir Next\" fontSize=\"60\" alignment=\"right\"  bold=\"0\" lineSpacing=\"10\" italic=\"0\"/>\n");
            fprintf(f, "  </text-style-def>\n");
            fprintf(f, "    <video name=\"Placeholder\" lane=\"-1\" offset=\"0s\" ref=\"MountId\" duration=\"1s\" start=\"3600s\">\n");
            fprintf(f, "      <param name=\"Sky\" key=\"9999/987171866/100/987175410/2/100\" value=\"5 (None)\"/>\n");
            fprintf(f, "      <param name=\"Background\" key=\"9999/987171866/100/987175692/2/100\" value=\"9 (Limbo)\"/>\n");
            fprintf(f, "      <param name=\"People\" key=\"9999/987171866/100/987181336/2/100\" value=\"0 (0)\"/>\n");
            fprintf(f, "      <param name=\"View Notes\" key=\"9999/987171866/100/999117736/2/100\" value=\"1\"/>\n");
            fprintf(f, "      <param name=\"Text\" key=\"9999/987185090/987187752/2/369\" value=\"MULTIPLE\"/>\n");
            fprintf(f, "      <param name=\"Size\" key=\"9999/987185090/987187752/5/987187756/3\" value=\"60\"/>\n");
            fprintf(f, "      <param name=\"Font\" key=\"9999/987185090/987187752/5/987187756/83\" value=\"23 0\"/>\n");
            fprintf(f, "    </video>\n");

        }
        else{
            // this is the additional titles to the timeline
            fprintf(f, "\t<!-- MULTIPLE EVENTS %d SEQUENCE %d -->\n", timeline_nb, timeline_seq);
            fprintf(f, "\t<title name=\"Basic Title: (%d)\" lane=\"%d\" offset=\"0s\" ref=\"TextFormatFileId\" duration=\"1s\" role=\"titles.POI\">\n",
                    titleSeq, timeline_seq +1);
            fprintf(f, "  \t<param name=\"Position\" key=\"9999/999166631/999166633/1/100/101\" value=\"895 %d\"/>\n", -355 + timeline_seq*-80);
            fprintf(f, "  \t<param name=\"Alignment\" key=\"9999/999166631/999166633/2/354/999169573/401\" value=\"2 (Right)\"/>\n");
            fprintf(f, "  \t<text>\n");
            fprintf(f, "    \t<text-style ref=\"line-%s%d\">", line, titleSeq);
            fprintf(f, "%s: %s", line, text);
            fprintf(f, "\t</text-style>\n");
            fprintf(f, "  \t</text>\n");
            fprintf(f, "  \t<text-style-def id=\"line-%s%d\">\n", line, titleSeq);
            fprintf(f, "    \t<text-style font=\"Avenir Next\" fontSize=\"60\" alignment=\"right\"  bold=\"0\" lineSpacing=\"10\" italic=\"0\"/>\n");
            fprintf(f, "  \t</text-style-def>\n");
            fprintf(f, "\t</title>\n");
        }
        }
        else{
            
            /* <marker start='4644/30s' duration='1/48000s' value='beat_at_154.8' completed='0'/> */
            fprintf(f, "         <marker start='%ds' duration='1s' value='%s - %s' completed='0'/>\n",
                    titleSeq,
                    line, text
                    );
        }
    }
    return 0;
}

int writeFCPXML_TL_MARKER_data(
        FILE* f,
        file_GPS_struct* file_gps,
        GPS_atom* GPS,
        file_tags_struct* file_tags,
        file_stops_struct* file_stops) {
    
    
    int i = 0;
    int tag = 0;
    int seq = 0;
    char argument[64];
    char real_path[MAX_PATH];

    realpath(file_tags->file_name, real_path);
    fprintf(f, 
            "   <format id='r1' name='FFVideoFormat1080p30'  frameDuration='1/30s'/>\n"
            "    <asset id=\"r2\" name=\"%s\" src=\"file://%s\" start='0/1000s' duration='%ds'>\n"
            "    </asset>\n"
            "  </resources>\n", file_tags->short_name, real_path, (int) file_tags->duration_sec); 
    fprintf(f, 
            "  <library>\n"
            "   <event name=\"Orbis Terrae\">\n"
            "    <project name=\"TAGS (%s)\">\n"
            "     <sequence duration=\"%lds\" format=\"r1\" tcStart=\"0s\" tcFormat=\"NDF\" keywords=\"Orbis Terrae\">\n"
            "      <spine>\n", file_tags->short_name, file_tags->duration_sec);
    fprintf(f, 
            "       <asset-clip name='%s' lane='0' offset='0/1000s' ref='r2' duration='%ds'>\n",
             file_tags->short_name, (int) file_tags->duration_sec);
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    /* if (file_tags->nb_tags > 0) {
        for (i = 0; i < file_tags->nb_tags; i++) {
            tag = (int) (file_tags->tags[i] / 1000);*/
            /* <marker start='4644/30s' duration='1/48000s' value='beat_at_154.8' completed='0'/> */
    /*        fprintf(f,"         <marker start='%ds' duration='1s' value='TAG[%d] %ds' completed='0'/>\n",
                    tag, i, file_tags->tags[i]);
        }
    }
    */
    
    for (i = 0; i < file_gps->samples; i++) {
        seq = 0;
        if (timeline[i] > 0) {
            // there is at least one event to write to the timeline
            writePlaceholder_header(f, i, timeline[i], seq);
            if (i == 0) {
                writePlaceholder(f, i, "POSITION", i, "Start", 1, timeline[i], seq, 1);
                seq++;
            }
            if (i == file_gps->samples / 2) {
                writePlaceholder(f, i, "POSITION", i, "Mid-point", 1, timeline[i], seq,1);
                seq++;
            }
            if (i == file_gps->samples / 4) {
                writePlaceholder(f, i, "POSITION", i, "1/4", 1, timeline[i], seq,1);
                seq++;
            }
            if (i == file_gps->samples - 1) {
                writePlaceholder(f, i, "POSITION", i, "Finish", 1, timeline[i], seq,1);
                seq++;
            }
            if (i == (int)((file_gps->samples * 3) / 4)) {
                writePlaceholder(f, i, "POSITION", i, "3/4", 1, timeline[i], seq,1);
                seq++;
            }
            if (i == file_gps->max_alt_index || i == file_gps->min_alt_index) {
            // prevent line redefinition in xml if min & max alt are at the same location
                if(i==file_gps->max_alt_index) snprintf(argument, sizeof argument, "%.1fm", file_gps->max_alt);
                if(i==file_gps->min_alt_index) snprintf(argument, sizeof argument, "%.1fm", file_gps->min_alt);
                writePlaceholder(f, i, "ALT", i, argument, 2, timeline[i], seq,1);
                seq++;
            }
            if ((i == file_gps->max_speed_index ) && file_gps->max_speed > 0.0) {
                snprintf(argument, sizeof argument, "%.2fkm/h", file_gps->max_speed);
                writePlaceholder(f, i, "SPEED", i, argument, 6, timeline[i], seq,1);
                seq++;
            }
            if ((i == file_gps->max_temp_index && hasTemp) || (i == file_gps->min_temp_index && hasTemp)){
            // prevent line redefinition in xml if min & max temp are at the same location
                if(i==file_gps->max_temp_index) snprintf(argument, sizeof argument, "%.2f°", file_gps->max_temp);
                if(i==file_gps->min_temp_index) snprintf(argument, sizeof argument, "%.2f°", file_gps->min_temp);
                writePlaceholder(f, i, "TEMP", i, argument, 4, timeline[i], seq,1);
                seq++;
            }
            if ((i == file_gps->max_hr_index && hasHR) || (i == file_gps->min_hr_index && hasHR)) {
            // prevent line redefinition in xml if min & max HR are at the same location
                if(i==file_gps->max_hr_index) snprintf(argument, sizeof argument, "%dbpm", file_gps->max_hr);
                if(i==file_gps->min_hr_index) snprintf(argument, sizeof argument, "%dbpm", file_gps->min_hr);
                writePlaceholder(f, i, "HR", i, argument, 4, timeline[i], seq,1);
                seq++;
            }

            if (file_tags->nb_tags > 0) {
                for (int j = 0; j < file_tags->nb_tags; j++) {
                    tag = (int) file_tags->tags[j] / 1000;
                    if (tag == i) {
                        snprintf(argument, sizeof argument, "%.2fs", ((float) file_tags->tags[j]) / 1000);
                        writePlaceholder(f, i, "TAGS", i, argument, 3, timeline[i], seq,1);
                        seq++;
                    }
                }
            }

            if (file_stops->nb_stops > 0) {
                for (int j = 0; j < file_stops->nb_stops; j++) {
                    if (i == file_stops->stop_duration_index[j]) {
                        snprintf(argument, sizeof argument, "%ds", file_stops->stop_duration[j]);
                        writePlaceholder(f, i, "STOP", i, argument, 7, timeline[i], seq,1);
                        seq++;
                    }
                }
            }
        }
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    
    
    fprintf(f, "       </asset-clip>\n");
    fprintf(f, "      </spine>\n");
    fprintf(f, "     </sequence>\n");
    fprintf(f, "    </project>\n");
    fprintf(f, "   </event>\n");
    fprintf(f, "  </library>\n");
    fprintf(f, "</fcpxml>\n");

    return 0;
    
}


int writeFCPXML_TAGS_MARKER_data(FILE* f, file_tags_struct* file_tags) {
    int i = 0;
    int tag = 0;
    char argument[64];
    char real_path[MAX_PATH];

    realpath(file_tags->file_name, real_path);
    fprintf(f, 
            "   <format id='r1' name='FFVideoFormat1080p30'  frameDuration='1/30s'/>\n"
            "    <asset id=\"r2\" name=\"%s\" src=\"file://%s\" start='0/1000s' duration='%ds'>\n"
            "    </asset>\n"
            "  </resources>\n", file_tags->short_name, real_path, (int) file_tags->duration_sec); 
    fprintf(f, 
            "  <library>\n"
            "   <event name=\"Orbis Terrae\">\n"
            "    <project name=\"TAGS (%s)\">\n"
            "     <sequence duration=\"%lds\" format=\"r1\" tcStart=\"0s\" tcFormat=\"NDF\" keywords=\"Orbis Terrae\">\n"
            "      <spine>\n", file_tags->short_name, file_tags->duration_sec);
    fprintf(f, 
            "       <asset-clip name='%s' lane='0' offset='0/1000s' ref='r2' duration='%ds'>\n",
             file_tags->short_name, (int) file_tags->duration_sec);

    if (file_tags->nb_tags > 0) {
        for (i = 0; i < file_tags->nb_tags; i++) {
            tag = (int) (file_tags->tags[i] / 1000);
            /* <marker start='4644/30s' duration='1/48000s' value='beat_at_154.8' completed='0'/> */
            fprintf(f,"         <marker start='%ds' duration='1s' value='TAG[%d] %ds' completed='0'/>\n",
                    tag, i, file_tags->tags[i]);
        }
    }
    fprintf(f, "       </asset-clip>\n");
    fprintf(f, "      </spine>\n");
    fprintf(f, "     </sequence>\n");
    fprintf(f, "    </project>\n");
    fprintf(f, "   </event>\n");
    fprintf(f, "  </library>\n");
    fprintf(f, "</fcpxml>\n");

    return 0;
}

int writeFCPXML_TAGS_data(FILE* f, file_tags_struct* file_tags) {
    int i = 0;
    int tag = 0;
    char argument[64];

    fprintf(f, "  <media id=\"TAGS\" name=\"Orbis Terrae (%s)\">\n"
            "    <sequence duration=\"%lds\" format=\"VideoFormatId\" tcStart=\"0s\" tcFormat=\"NDF\" keywords=\"Orbis Terrae\">\n"
            "       <spine>\n", file_tags->short_name, file_tags->duration_sec);

    if (file_tags->nb_tags > 0) {
        for (i = 0; i < file_tags->nb_tags; i++) {
            tag = (int) (file_tags->tags[i] / 1000);
            if (tag > 1 && i == 0) {
                fprintf(f, "<gap name=\"Gap\" offset=\"0s\" duration=\"%ds\" start=\"3600s\"/>\n", tag);
            }

            snprintf(argument, sizeof argument, "%.2fs", ((float) file_tags->tags[i]) / 1000);
            // no multiple sequence in tags, max one tag per second.
            writePlaceholder_header(f, i, 1, i + 1);
            writePlaceholder(f, i, "TAGS", tag, argument, 3, 1, i + 1, 0);
            writePlaceholder_closer(f, 1, i + 1);
        }
    }
    fprintf(f, "      </spine>\n");
    fprintf(f, "      <metadata>\n");
    fprintf(f, "        <md key=\"com.apple.proapps.mio.cameraName\" value=\"Orbis Terrae\"/>\n");
    fprintf(f, "      </metadata>\n");
    fprintf(f, "    </sequence>\n");
    fprintf(f, "  </media>\n");

    return 0;
}

int calc_timeline(
        file_GPS_struct* file_gps,
        file_tags_struct* file_tags,
        file_stops_struct* file_stops){
    int i = 0, tag = 0, events = 0, stop_index = 0;
    
        // populate timeline structure
    for (i = 0; i < file_gps->samples; i++) {
        if (i == 0){
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> POSITION event registered\n", i);
        }
        if (i == file_gps->samples / 2){
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> POSITION event registered\n", i);
        }
        if (i == file_gps->samples -1){
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> POSITION event registered\n", i);
        }
        if (i == file_gps->samples / 4){
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> POSITION event registered\n", i);
        }
        if (i == (int)((file_gps->samples * 3) / 4)){
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> POSITION event registered\n", i);
        }
        if (i == file_gps->max_alt_index || i == file_gps->min_alt_index){
            // prevent line redefinition in xml if min & max alt are at the same location
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> ALT event registered\n", i);
        }
        if ((i == file_gps->max_speed_index) && file_gps->max_speed > 0.0){
            // if the max speed is zero - no need to include in the timeline
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> SPEED event registered\n", i);
        }
        if ((i == file_gps->max_hr_index && hasHR) || (i == file_gps->min_hr_index && hasHR) ){
            // prevent line redefinition in xml if min & max alt are at the same location
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> HR event registered\n", i);
        }
        if ((i == file_gps->max_temp_index && hasTemp) || (i == file_gps->min_temp_index && hasTemp) ){
            // prevent line redefinition in xml if min & max alt are at the same location
            timeline[i]+=1;
            events ++;
            if(timeline[i] > 0 && debug) printf("Timeline [%d] -> TEMP event registered\n", i);
        }
        if (file_tags->nb_tags > 0) {
            for (int j = 0; j < file_tags->nb_tags; j++) {
                tag = (int) file_tags->tags[j] / 1000;
                if (tag == i) {
                    timeline[i]+=1;
                    events ++;
                    if(timeline[i] > 0 && debug) printf("Timeline [%d] -> TAGS event registered\n", i);
                }
            }
        }
        
        if (file_stops->nb_stops > 0) {
            for (int j = 0; j < file_stops->nb_stops; j++) {
                stop_index = file_stops->stop_duration_index[j];
                if(stop_index == i){
                /*printf("timeline[%d] stop_index %d ; file_stop->stop_duration_index[%d] %d\n",i,
                        stop_index, j,
                        file_stops->stop_duration_index[j]);*/
                    timeline[i]+=1;
                    events ++;
                    if(timeline[i] > 0 && debug) printf("Timeline [%d] -> STOP event registered\n", i);
                }
            }
        }
        
        if(timeline[i] > 1  && debug) 
            printf("Timeline [%d] -> %d events\n", i, timeline[i]);
    }
    if(debug) printf("Timeline %ds includes %d event(s), made of %d tag(s) and %d stop(s)\n", i, events,file_tags->nb_tags, file_stops->nb_stops);
    return 0;
}


int writeFCPXML_TL_data(
        FILE* f,
        file_GPS_struct* file_gps,
        GPS_atom* GPS,
        file_tags_struct* file_tags,
        file_stops_struct* file_stops) {


    int i = 0;
    int tag = 0;
    char argument[64];
    int seq = 0;

     // ------------------- writing sequence of all noticable events on a timeline  ----

    fprintf(f, "  <media id=\"TAGS\" name=\"POI Timeline (%s)\">\n"
            "    <sequence duration=\"%lds\" format=\"VideoFormatId\" tcStart=\"0s\" tcFormat=\"NDF\" keywords=\"Orbis Terrae\">\n"
            "       <spine>\n", file_tags->short_name, file_tags->duration_sec);
    for (i = 0; i < file_gps->samples; i++) {
        seq = 0;
        if (timeline[i] > 0) {
            // there is at least one event to write to the timeline
            writePlaceholder_header(f, i, timeline[i], seq);
            if (i == 0) {
                writePlaceholder(f, i, "POSITION", i, "Start", 1, timeline[i], seq, 0);
                seq++;
            }
            if (i == file_gps->samples / 2) {
                writePlaceholder(f, i, "POSITION", i, "Mid-point", 1, timeline[i], seq, 0);
                seq++;
            }
            if (i == file_gps->samples / 4) {
                writePlaceholder(f, i, "POSITION", i, "1/4", 1, timeline[i], seq, 0);
                seq++;
            }
            if (i == file_gps->samples - 1) {
                writePlaceholder(f, i, "POSITION", i, "Finish", 1, timeline[i], seq, 0);
                seq++;
            }
            if (i == (int)((file_gps->samples * 3) / 4)) {
                writePlaceholder(f, i, "POSITION", i, "3/4", 1, timeline[i], seq, 0);
                seq++;
            }
            if (i == file_gps->max_alt_index || i == file_gps->min_alt_index) {
            // prevent line redefinition in xml if min & max alt are at the same location
                if(i==file_gps->max_alt_index) snprintf(argument, sizeof argument, "%.1fm", file_gps->max_alt);
                if(i==file_gps->min_alt_index) snprintf(argument, sizeof argument, "%.1fm", file_gps->min_alt);
                writePlaceholder(f, i, "ALT", i, argument, 2, timeline[i], seq, 0);
                seq++;
            }
            if ((i == file_gps->max_speed_index ) && file_gps->max_speed > 0.0) {
                snprintf(argument, sizeof argument, "%.2fkm/h", file_gps->max_speed);
                writePlaceholder(f, i, "SPEED", i, argument, 6, timeline[i], seq, 0);
                seq++;
            }
            if ((i == file_gps->max_temp_index && hasTemp) || (i == file_gps->min_temp_index && hasTemp)){
            // prevent line redefinition in xml if min & max temp are at the same location
                if(i==file_gps->max_temp_index) snprintf(argument, sizeof argument, "%.2f°", file_gps->max_temp);
                if(i==file_gps->min_temp_index) snprintf(argument, sizeof argument, "%.2f°", file_gps->min_temp);
                writePlaceholder(f, i, "TEMP", i, argument, 4, timeline[i], seq, 0);
                seq++;
            }
            if ((i == file_gps->max_hr_index && hasHR) || (i == file_gps->min_hr_index && hasHR)) {
            // prevent line redefinition in xml if min & max HR are at the same location
                if(i==file_gps->max_hr_index) snprintf(argument, sizeof argument, "%dbpm", file_gps->max_hr);
                if(i==file_gps->min_hr_index) snprintf(argument, sizeof argument, "%dbpm", file_gps->min_hr);
                writePlaceholder(f, i, "HR", i, argument, 4, timeline[i], seq, 0);
                seq++;
            }

            if (file_tags->nb_tags > 0) {
                for (int j = 0; j < file_tags->nb_tags; j++) {
                    tag = (int) file_tags->tags[j] / 1000;
                    if (tag == i) {
                        snprintf(argument, sizeof argument, "%.2fs", ((float) file_tags->tags[j]) / 1000);
                        writePlaceholder(f, i, "TAGS", i, argument, 3, timeline[i], seq, 0);
                        seq++;
                    }
                }
            }

            if (file_stops->nb_stops > 0) {
                for (int j = 0; j < file_stops->nb_stops; j++) {
                    if (i == file_stops->stop_duration_index[j]) {
                        snprintf(argument, sizeof argument, "%ds", file_stops->stop_duration[j]);
                        writePlaceholder(f, i, "STOP", i, argument, 7, timeline[i], seq, 0);
                        seq++;
                    }
                }
            }
            writePlaceholder_closer(f, timeline[i], seq);
        }
    }

    fprintf(f, "      </spine>\n");
    fprintf(f, "      <metadata>\n");
    fprintf(f, "        <md key=\"com.apple.proapps.mio.cameraName\" value=\"Orbis Terrae\"/>\n");
    fprintf(f, "      </metadata>\n");
    fprintf(f, "    </sequence>\n");
    fprintf(f, "  </media>\n");

    // ------------------- writing sequence of simple GPS text overlay a la PhantomOverlay ----
    fprintf(f, "  <media id=\"Overlay\" name=\"GPS Overlay (%s)\">\n"
            "    <sequence duration=\"%ds\" format=\"VideoFormatId\" tcStart=\"0s\" tcFormat=\"NDF\" keywords=\"Orbis Terrae\">\n"
            "       <spine>\n", file_gps->short_name, (int) file_gps->samples);

    for (i = 0; i < file_gps->samples; i++) { 
        writeOverlay(f, i, GPS[i].GPS_year, GPS[i].GPS_month, GPS[i].GPS_day, GPS[i].GPS_hour, GPS[i].GPS_min, GPS[i].GPS_sec, 
                (int) GPS[i].heading, GPS[i].curSpeed, GPS[i].ele, 
                GPS[i].temp, GPS[i].HR);
    }

    fprintf(f, "      </spine>\n");
    fprintf(f, "      <metadata>\n");
    fprintf(f, "        <md key=\"com.apple.proapps.mio.cameraName\" value=\"Orbis Terrae\"/>\n");
    fprintf(f, "      </metadata>\n");
    fprintf(f, "    </sequence>\n");
    fprintf(f, "  </media>\n");

    return 0;
}

int init_timeline(int samples){
    timeline = malloc (sizeof(int)*samples);
    for(int i=0; i< samples ; i++){
        timeline[i] = 0;
    }
    return 0;
}

int release_timeline(){
    if(timeline != NULL) free(timeline);
    return 0;
}

int writeFCPXML(file_GPS_struct* file_gps, 
        GPS_atom* GPS, 
        file_tags_struct* file_tags, 
        file_stops_struct * file_stops) {

    FILE* fp;
    int duration = 0;
    char filename[MAX_PATH] = "filename";
    sprintf(filename, "%s.FCPXML", file_tags->file_name);

#ifdef _WINDOWS
    fopen_s(&fp, filename, "wt");
#else
    fp = fopen(filename, "wt");
#endif
    if (file_gps->samples <= 0) {
        // ---------------------------- no gps data, simply write tags ------------------

        if(debug) printf("Write FCPXML NO GPS Samples (%d)\n", file_tags->nb_tags);
        writeFCPXML_header(fp);
        writeFCPXML_TAGS_data(fp, file_tags);
        writeFCPXML_TAGS_closer(fp, file_tags->duration_sec);

    // --------------------- write fcpxml with markers and import file ------------------
    // ---------------------------- no gps data, simply write tags ------------------
        if (debug) printf("Write FCPXML with markers - Samples (%d)\n", file_tags->nb_tags);
        sprintf(filename, "%s.TAGS.FCPXML", file_tags->file_name);
#ifdef _WINDOWS
        fopen_s(&fp, filename, "wt");
#else
        fp = fopen(filename, "wt");
#endif

        printf("Write FCPXML NO GPS Samples (%d)\n", file_gps->samples);
        writeFCPXML_header(fp);
        writeFCPXML_TAGS_MARKER_data(fp, file_tags);
        fclose(fp);
    } else {
        // ---------------------------- GPS populated, write full time line ---------------

        if(debug) printf("Write FCPXML with GPS Samples (%d)\n", file_gps->samples);
        init_timeline(file_gps->samples);
        calc_timeline(file_gps, file_tags, file_stops);
        writeFCPXML_header(fp);
        writeFCPXML_TL_data(fp, file_gps, GPS, file_tags, file_stops);
        writeFCPXML_TL_closer(fp, file_tags->duration_sec);
        
        
        if (debug) printf("Write FCPXML with markers - Samples (%d)\n", file_tags->nb_tags);
        sprintf(filename, "%s.TAGS.FCPXML", file_tags->file_name);
#ifdef _WINDOWS
        fopen_s(&fp, filename, "wt");
#else
        fp = fopen(filename, "wt");
#endif

        writeFCPXML_header(fp);
        writeFCPXML_TL_MARKER_data(fp, file_gps, GPS, file_tags, file_stops);
        fclose(fp);
        
        release_timeline();
    }
    fclose(fp);

    return 0;
}