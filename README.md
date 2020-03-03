# GraffitiCL ©2019 Orbis Terrae 
https://sites.google.com/site/oterrae/


Command line app to extract HiLights and GPS data from GOPRO Hero 5 video files or from DJI Mavic STR files.

The app is generating FCPXML, GPX & KML files for Final Cut Pro X, Google Earth or Garming Virb Edit.

Way more advanced version compared to HeroineCL.



usage: Usage: ./GraffitiCL [-h] [-v] [-d] [-n] [-g] [-k] [-z] [-f] [-M] [-t Temperature] [-r HearRate] [-o Output] Input_file
- [-h] help - display this usage
- [-v] verbose - display verbose information to be used in GUI
- [-d] debug - display debug information
- [-n] no output - simlation only\n[-g] generate GPX output
- [-k] generate KML output
- [-z] generate KMLZ output including pictures
- [-f] generate FCPXML output\n[-t Temperature] initial temperature in °C - varies with altitude
- [-r HearRate] Heart Rate in BPM - cycle around central point
- [-M] process successive GOPRO MP4 files as one
- [-o Output] optional output file - if not specified, default to the input path
- input file: currently supports GoPro Hero 5 MP4 and DJI STR files

Tested on macOS 10.14 with GOPRO Hero 5 and Mavic Platinum

Questions, remarks, feedback: oterrae@gmail.com
