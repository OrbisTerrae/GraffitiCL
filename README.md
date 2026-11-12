# GraffitiCL

GraffitiCL is a command-line tool for turning action-camera and drone telemetry into files that are useful in a video editor or on a map. Its original goal was to make GoPro highlights and GPS tracks easy to review in Final Cut Pro, Google Earth, and Garmin Virb Edit. The project also accepts DJI subtitle telemetry in SRT format.

## What it reads and writes

**Inputs**

- GoPro MP4 files with an embedded GPMF telemetry track. The GPS extraction code looks for `GPS5` and `GPRI` records. The project began with Hero 5 footage; later camera models should be validated against representative files before relying on every field.
- DJI SRT subtitle files containing timestamped GPS and barometer data.
- A directory of input clips. Successive GoPro files can be combined with `-M`; the expected GoPro naming sequence uses `GOPRxxxx.MP4` followed by `GP01xxxx.MP4`, `GP02xxxx.MP4`, and so on.

**Outputs**

- **FCPXML** for Final Cut Pro, including clip metadata, highlight markers, GPS-related points of interest, and timeline information.
- **GPX** tracks for GPS applications.
- **KML/KMZ** tracks and map annotations for Google Earth and compatible mapping tools.

Select one or more output flags. `-a` is a convenience option that enables GPX, KML, FCPXML, verbose reporting, simulated temperature and heart-rate values, and successive-file processing.

## How it works

1. The MP4 reader locates the GPMF metadata track and reads its timed payloads. For DJI input, the SRT reader parses the subtitle records instead.
2. The GoPro GPMF parser decodes the payload structure and scaled telemetry values. GPS records are converted into the application's common GPS point structure. The program calculates track statistics such as distance, speed, altitude range, and detected stops.
3. GoPro highlight metadata and clip timing are read separately from the telemetry points. For a directory of successive GoPro clips, the program can combine points and markers into a single timeline.
4. The selected writers create FCPXML, GPX, and/or KML output files next to the input. `-n` runs the parsing and reporting path without writing output files.

The GPMF decoder is vendored from [GoPro's gpmf-parser](https://github.com/gopro/gpmf-parser). The exact upstream commit is recorded in [`GPMF-parser-UPSTREAM-COMMIT`](third_party/gpmf-parser/GPMF-parser-UPSTREAM-COMMIT), and the upstream license files are included. MP4 container extraction and DJI SRT parsing remain separate parts of GraffitiCL.

## Build

A C compiler and `make` are required. Application code is in `src/`, the pinned GoPro parser and MP4 support code are in `third_party/`, sample XML and frame-rate notes are in `examples/`, and historical alternatives are in `archive/legacy/`.

```sh
make
make clean
```

To build with AddressSanitizer on a supported Clang or GCC toolchain:

```sh
make clean
make CFLAGS='-g -Wall -Wextra -fsanitize=address' LDFLAGS='-fsanitize=address'
```

## Usage

```text
./GraffitiCL [options] Input_file_or_directory
```

Examples:

```sh
# Create a GPX track from one GoPro clip
./GraffitiCL -g GOPR0001.MP4

# Create Final Cut XML and KML outputs
./GraffitiCL -f -k GOPR0001.MP4

# Process a directory, combine successive GoPro clips, and create all outputs
./GraffitiCL -a -M /path/to/clips/

# Parse and report without writing files
./GraffitiCL -n -g GOPR0001.MP4
```

Options:

| Option | Meaning |
| --- | --- |
| `-h` | Show help. |
| `-v` | Print verbose progress. |
| `-d` | Print debugging information. |
| `-n` | Parse without writing output files. |
| `-g` | Write GPX. |
| `-k` | Write KML. |
| `-z` | Request KMLZ output. |
| `-f` | Write FCPXML. |
| `-M` | Combine successive GoPro MP4 clips when processing a directory. |
| `-a` | Enable verbose mode, GPX, KML, FCPXML, simulated temperature and heart rate, and `-M`. |
| `-t Celsius` | Set the initial temperature used by the temperature estimate. |
| `-r BPM` | Set the baseline heart rate used by the heart-rate estimate. |
| `-o Output` | Specify an output path. This option is present in the CLI, but output-path behavior is not currently reliable. |

File extensions are currently matched in uppercase (`.MP4` and `.SRT`). Directory processing scans the directory's immediate entries.

## Timing regression checks

After building, run `make test` (or `python3 tests/timing_regression.py`). The script uses the sibling `../test` folder by default, writes generated files only to a temporary directory, and checks known highlight records, embedded GoPro capture time, MP4 duration/frame rate against `ffprobe` when available, one-second FCPXML overlay cadence, and merged GPX/FCPXML timing. Pass `--clips-dir PATH` to use another fixture directory. The source clips are not included in this repository.

## Known limitations and follow-up work

The timing baseline is implemented; open validation and follow-up work includes:

- Verify marker and highlight timing against clips with known GoPro highlight tags.
- Test actual high and unusual frame rates such as 60, 80, and 90 fps; current fixtures contain no such video files.
- Review heading, tilt, speed, and distance calculations; improve KML statistics and map previews.
- Validate merged GPX/KML timing and metadata across successive clips.
- Use a consistent source for capture dates in GPX and KML.
- Improve stop, turn, and point-of-interest detection, and explore motion overlays.
- Consider importing GPX tracks from activity apps and adding a graphical interface.
- Add telemetry extraction for first-generation DJI Mavic Mini footage. Existing DJI SRT parsing targets Phantom-format recordings.

The grouped implementation plan and recommended starting milestone are in [`docs/implementation-roadmap.md`](docs/implementation-roadmap.md). The project history and open ideas are preserved in [`docs/development-notes.md`](docs/development-notes.md). Earlier work includes DJI SRT support, successive GoPro clips, stop detection, FCPXML tags, GPS overlays, and KML output. The original README stated that the application was tested on macOS 10.14 with GoPro Hero 5 and DJI Mavic Platinum footage.
