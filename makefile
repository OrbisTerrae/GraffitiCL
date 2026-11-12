CC ?= cc
CFLAGS ?= -g -Wall -Wextra
CPPFLAGS ?=
LDFLAGS ?=
DEPFLAGS = -MMD -MP

CPPFLAGS += -Isrc -Ithird_party/gpmf-parser -Ithird_party/gpmf-mp4reader

APP_OBJECTS = \
	build/GraffitiCLI.o \
	build/GPS.o \
	build/GPX.o \
	build/KML.o \
	build/FCPXML.o \
	build/SRT.o \
	build/stop.o \
	build/tags.o

GPMF_OBJECTS = \
	build/gpmf-parser/GPMF_parser.o \
	build/gpmf-parser/GPMF_utils.o \
	build/gpmf-mp4reader/GPMF_mp4reader.o \
	build/gpmf-mp4reader/GPMF_print.o

.PHONY: all clean test
all: GraffitiCL

test: GraffitiCL
	python3 tests/timing_regression.py

GraffitiCL: $(APP_OBJECTS) $(GPMF_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJECTS) $(GPMF_OBJECTS)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

build/gpmf-parser/%.o: third_party/gpmf-parser/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

build/gpmf-mp4reader/%.o: third_party/gpmf-mp4reader/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

-include $(APP_OBJECTS:.o=.d) $(GPMF_OBJECTS:.o=.d)

clean:
	rm -rf build GraffitiCL
	rm -f *.o *.d
