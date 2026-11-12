# Implementation roadmap

This roadmap groups the proposed improvements by the work they share and identifies the recommended starting point. The order is intentional: output accuracy and analysis depend on a reliable timeline.

## 1. Establish reliable clip timing

Start with timing because marker placement, frame rates, GPX timestamps, analysis, and merged clips all depend on it.

Work in this group:

- Verify GoPro highlight counts and exact marker times against tagged clips and a trusted reference.
- Read video duration and frame rate from authoritative MP4 timing metadata, including unusual rates such as 80 fps.
- Use one documented capture-date policy for GPX and KML, preferring embedded capture time when available and using a consistent fallback otherwise.
- Represent clip offsets and sample times precisely, then use the same timeline for successive clips and all outputs.
- Preserve each clip's metadata when building a merged session.

### First milestone

The repeatable checks are in `tests/timing_regression.py`. The current test clips provide useful cases: `GOPR-HL1.MP4` reports one highlight, `GOPR-HL5.MP4` reports five, and `GOPR2884.MP4`, `GP012884.MP4`, and `GP022884.MP4` form a successive clip sequence. The marker offsets in the test are read from the clips' HMMT records; they still need comparison with an independent trusted reference.

Introduce a shared timestamped clip/session model. Give every GPS point an explicit time, and represent frame rate and clip duration without rounding them to whole seconds. Update highlight, duration, frame-rate, date, and merge handling to use that model.

### Implemented baseline (2026-11-12)

- MP4 parsing now retains fractional clip duration and reads the video track's sample count. Frame rate is derived from frame count divided by duration, instead of a small hard-coded time-scale lookup.
- GPS sample offsets are kept when producing GPX timestamps. GPX and KML share a UTC capture-time base: use the embedded GoPro GPSU value when present, then fall back to file birth time on macOS (modification time elsewhere), and finally the current time if the file timestamp cannot be read.
- Successive clips retain their original point offsets, and merged GPS points and highlight markers are offset by the preceding clips' measured durations.
- Fixed the multi-file GPS copy to use the current clip's parsed buffer rather than reusing points from the first clip.
- Existing clips were run through the CLI. Highlight counts remained 1 for `GOPR-HL1.MP4` and 5 for `GOPR-HL5.MP4`; the `GOPR2884` / `GP012884` / `GP022884` merged GPX had 84 points with nondecreasing timestamps.
- A repeatable check script now validates those known marker offsets and counts, checks duration and frame rate against `ffprobe` where available, checks merged GPX/FCPXML timing, and asserts the one-second overlay cadence.

FCPXML's GPS telemetry overlay intentionally stays at a one-second cadence with one-second title durations. This keeps noisy drone telemetry readable; the clip and sequence duration and GoPro highlight marker start times use fractional timing.

Still needed before calling timing fully verified: compare exact highlight offsets with a trusted GoPro reference; validate GPSU parsing across camera generations and establish a stronger fallback policy; and test actual 60/80/90 fps sample videos (the checked-in examples currently contain notes, not video fixtures).

## 2. Correct track calculations

Once timestamps are reliable, update the calculations to use elapsed time and clearly defined units:

- Compute speed from distance and elapsed time, and validate distance and ascent/descent against reference tracks.
- Define heading behavior for sparse, noisy, or stationary points.
- Decide whether `tilt` describes terrain slope or camera orientation. Terrain slope can use adjacent GPS points; camera orientation requires motion/orientation telemetry.
- Rework stop detection to use distance and elapsed duration rather than a per-sample threshold alone.
- Add turn and point-of-interest detection only after the underlying calculations are validated.

Use the same analysis results for every output format.

## 3. Improve output presentation

Keep GPX, KML, and FCPXML writers focused on formatting the shared session and analysis results. Avoid recalculating time or statistics inside each writer.

- Make timestamps, duration, and metadata consistent across GPX, KML, and FCPXML.
- Use calculated route bounds and statistics for KML views and map previews rather than fixed camera positions and sample-index spacing.
- Explore motion overlays after confirming which GPMF motion streams are available and how they align with video frames.

## 4. Expand inputs and user interface

Treat these as later features built on the stable session model:

- Add GPX import for activity tracks, then define how imported points align with video time and GPS telemetry.
- Add telemetry extraction for first-generation DJI Mavic Mini recordings. DJI SRT support currently targets Phantom-format recordings; Mavic Mini telemetry needs its own input parsing and timestamp validation.
- Add a graphical interface after the command-line pipeline, data model, and output behavior are stable.

## Suggested order of work

1. Complete timing validation against independent marker references and high-frame-rate clips.
2. Introduce a shared clip/session/time model.
3. Parse embedded capture dates where available and document the fallback policy.
4. Correct GPS calculations and stop detection using the shared timestamps.
5. Improve KML previews and validate output metadata across formats.
6. Add GPX import, DJI Mavic Mini telemetry, advanced detection, motion overlays, and a GUI as separate follow-up work.

This order keeps the first implementation focused on correctness and provides a stable base for the larger features.
