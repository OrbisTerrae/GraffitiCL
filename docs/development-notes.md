HISTORY

v0.4.0
Updated GPMF parser to version 1.2.2
// GMPF parser updated
// GPMF mp4 reader on going - file source imported and merged
// GPX import done - now need to add graffiti logic
// tag not done at all


//support gopro hero 7


multiples bug fixes: 
fixed multiple successive GoPro files support with TAGS.
fixed stop algo.
fixed SRT temp & HR.
Generates file with markers including GPS


v0.3.5 -> 0.3.6
Added multiple successive GoPro files support
Added the direct import of the GoPro video files with markers on top
Fixed SRT import with EV: -1 2/3

v0.3.4 -> 0.3.5
Added SRT support (DJI drones) as input

v0.3.3 -> 0.3.4
added working FCPXML with GPS Overlay & POI Timeline

V0.3.2 -> 0.3.3
Added kml stop detection
Added fcpxml tags

current TODO & issues of the release -> 
- ML - seems that first data (ele, hr, temp) = 0; // only on test data, not on real ML files
- markers -> fix markers
- get the exact duration of the video file (precision ms instead of s) GPMF different from video ?
- review heading & speed algo 
- Motion overlay, including rolling barel
- fix GPX / KML data for successive files 
- use same method to get the file creation time in KML & GPX -> it impacts successive files

next releases ideas ->
integrate GPX files import from SkiTracks/BikeTracks
update GPMF from latest release on git hub
develop front end UI
develop license / payement system
develop auto update
add GoPro Hero7 support


KML
calculate distance for all files 
calculate max speed for all files  / Fix speed calculation
biketrack kml with preview
speed: max / average 
distance: distance climb / distance Downhill
Vertical total (ascent + descent)
Slope ?
Analysis per KM and per hour
runs ?

-o test doesn't seem to work

tour POI
only if > 1k or 10m ?
detect pause - done
detect long turns
fix tilt
camera - far away from the current point

FCPXML
to import tags as markers into fcpxml, we need to get the timing of the full video first..
Frame rate - how to caculate for 80fps ???

http://www.fcp.co/final-cut-pro/tutorials/1912-demystifying-final-cut-pro-xmls-by-philip-hodgetts-and-gregory-clarke



https://stackoverflow.com/questions/13528588/retrieving-resolution-duration-of-a-video-file-before-upload
On browsers support HTML5, you can use an MP4 parser (such as mp4.js) to parse the file headers.

    To get the file duration, you will need to find the moov atom of the file, and inside it the mvhd atom. The duration, in seconds, can be calculated by dividing the mvhd's Duration field by the TimeScale field.
    To get the resolution, you will need to find inside the moov atom, the trak atom for the video track (ignore the audio track, or other tracks if you encounter them; usually the file has only one video track), and in it the tkhd atom, which has width and height fields.

successive gopro files
GOPR1226
GP011226
GP021226
GP031226
GP041226
GP051226

TAG[0:1] GOPR2819.MP4
TAG[0:1]: timescale:90000 29.97fps 
TAG[0:1]: 0 highlights within 3s duration
Frame rate                               : 29.970 (30000/1001) FPS

TAG[1:2] GOPR2820.MP4
TAG[1:2]: timescale:60000 59.94fps 
TAG[1:2]: 0 highlights within 11s duration
Frame rate                               : 59.940 (60000/1001) FPS

TAG[2:3] GOPR2821.MP4
TAG[2:3]: timescale:48000 30.00fps 
TAG[2:3]: 0 highlights within 11s duration
Frame rate                               : 47.952 (48000/1001) FPS


TAG[3:4] GOPR2822.MP4
TAG[3:4]: timescale:90000 29.97fps 
TAG[3:4]: 0 highlights within 16s duration
Frame rate                               : 29.970 (30000/1001) FPS

TAG[4:5] GOPR2823.MP4
TAG[4:5]: timescale:24000 30.00fps 
TAG[4:5]: 0 highlights within 17s duration
Frame rate                               : 23.976 (24000/1001) FPS


TAG[5:6] GOPR2824.MP4
TAG[5:6]: timescale:90000 29.97fps 
TAG[5:6]: 5 highlights within 709s duration
Frame rate                               : 29.970 (30000/1001) FPS

TAG[6:7] GOPR2825.MP4
TAG[6:7]: timescale:120000 30.00fps 
TAG[6:7]: 0 highlights within 50s duration
Frame rate                               : 119.880 (120000/1001) FPS

TAG[7:8] GOPR2826.MP4
TAG[7:8]: timescale:90000 29.97fps 
TAG[7:8]: 0 highlights within 30s duration
Frame rate                               : 90.000 FPS

TAG[8:9] GOPR2827.MP4
TAG[8:9]: timescale:90000 29.97fps 
TAG[8:9]: 2 highlights within 30s duration
Frame rate                               : 80.000 FPS

TAG[9:10] GOPR2828.MP4
TAG[9:10]: timescale:90000 29.97fps 
TAG[9:10]: 1 highlights within 47s duration
Frame rate                               : 29.970 (30000/1001) FPS

TAG[10:11] GOPR2829.MP4
TAG[10:11]: timescale:90000 29.97fps 
TAG[10:11]: 80 highlights within 294s duration
Frame rate                               : 29.970 (30000/1001) FPS