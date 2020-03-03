GraffitiCL : GraffitiCLI.o GPMF_parser.o SRT.o GPMF_mp4reader.o GPMF_print.o FCPXML.o GPS.o GPX.o KML.o stop.o tags.o 
		gcc -fsanitize=address -g -o GraffitiCL GraffitiCLI.o SRT.o GPMF_parser.o GPMF_mp4reader.o GPMF_print.o  FCPXML.o GPS.o GPX.o KML.o stop.o tags.o

GraffitiCLI.o : GraffitiCLI.c GPMF_parser.h tags.h
		gcc -fsanitize=address -g -c GraffitiCLI.c
GPMF_mp4reader.o : GPMF_mp4reader.c GPMF_parser.h
		gcc -fsanitize=address -g -c GPMF_mp4reader.c
GPMF_print.o : GPMF_print.c GPMF_parser.h
		gcc -fsanitize=address -g -c GPMF_print.c
GPMF_parser.o : GPMF_parser.c GPMF_parser.h
		gcc -fsanitize=address -g -c GPMF_parser.c
tags.o : tags.c tags.h
		gcc -fsanitize=address -g -c tags.c
GPS.o : GPS.c GPS.h tags.h
		gcc -fsanitize=address -g -c GPS.c
SRT.o : SRT.c SRT.h tags.h GPS.h
		gcc -fsanitize=address -g -c SRT.c
GPX.o : GPX.c GPX.h tags.h
		gcc -fsanitize=address -g -c GPX.c
KML.o : KML.c KML.h tags.h
		gcc -fsanitize=address -g -c KML.c
FCPXML.o : FCPXML.c FCPXML.h tags.h
		gcc -fsanitize=address -g -c FCPXML.c
stop.o : stop.c stop.h tags.h gps.h
		gcc -fsanitize=address -g -c stop.c
clean_test_files:
		rm test/*.FCPXML
		rm test/*.GPX
		rm test/*.KML
	
run: GraffitiCL
	./GraffitiCL -a -M test/
	
run_debug: GraffitiCL
	./GraffitiCL -a -M -d test/
    
clean :
		rm GraffitiCL GraffitiCLI.o SRT.o GPS.o GPMF_parser.o GPMF_mp4reader.o tags.o GPMF_print.o FCPXML.o GPX.o KML.o stop.o