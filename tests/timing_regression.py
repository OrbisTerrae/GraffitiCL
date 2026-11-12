#!/usr/bin/env python3
"""Regression checks for GoPro timing using the sample clips in ../test."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from fractions import Fraction
from pathlib import Path


KNOWN_MARKERS_MS = {
    "GOPR-HL1.MP4": [3169],
    "GOPR-HL5.MP4": [2702, 4337, 8641, 12312, 17517],
}
SUCCESSIVE_CLIPS = ["GOPR2884.MP4", "GP012884.MP4", "GP022884.MP4"]
GPX_NS = {"gpx": "http://www.topografix.com/GPX/1/1"}


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, check=True, text=True, capture_output=True)


def duration_seconds(value: str) -> float:
    if value.endswith("s"):
        value = value[:-1]
    return float(Fraction(value))


def check_highlight_markers(folder: Path) -> None:
    for filename, expected in KNOWN_MARKERS_MS.items():
        xml_path = folder / f"{filename}.TAGS.FCPXML"
        root = ET.parse(xml_path).getroot()
        markers = [node for node in root.iter("marker") if node.attrib.get("value", "").startswith("TAGS - ")]
        actual = [round(duration_seconds(node.attrib["start"]) * 1000) for node in markers]
        assert actual == expected, f"{filename}: expected markers {expected}, got {actual}"


def check_clip_timing(folder: Path, clips: list[Path]) -> None:
    ffprobe = shutil.which("ffprobe")
    if not ffprobe:
        print("SKIP ffprobe comparisons (ffprobe unavailable)")
        return

    for clip in clips:
        probe = json.loads(run([
            ffprobe, "-v", "error", "-select_streams", "v:0",
            "-show_entries", "stream=avg_frame_rate,duration",
            "-of", "json", str(clip),
        ]).stdout)
        stream = probe["streams"][0]
        root = ET.parse(folder / f"{clip.name}.TAGS.FCPXML").getroot()
        asset = next(node for node in root.iter("asset") if node.attrib.get("name") == clip.name)
        fmt = next(node for node in root.iter("format") if node.attrib.get("id") == "r1")
        actual_duration = duration_seconds(asset.attrib["duration"])
        probed_duration = float(stream["duration"])
        assert abs(actual_duration - probed_duration) <= 0.002, (
            f"{clip.name}: FCPXML duration {actual_duration} != ffprobe {probed_duration}"
        )
        frame_duration = duration_seconds(fmt.attrib["frameDuration"])
        actual_rate = 1.0 / frame_duration
        probed_rate = float(Fraction(stream["avg_frame_rate"]))
        assert abs(actual_rate - probed_rate) <= 0.03, (
            f"{clip.name}: FCPXML frame rate {actual_rate} != ffprobe {probed_rate}"
        )
        main_root = ET.parse(folder / f"{clip.name}.FCPXML").getroot()
        sequences = list(main_root.iter("sequence"))
        assert sequences and all(
            abs(duration_seconds(node.attrib["duration"]) - probed_duration) <= 0.002
            for node in sequences
        ), f"{clip.name}: FCPXML sequence duration does not match the video"


def check_successive_gpx(folder: Path) -> None:
    gpx_path = folder / "GOPR2884_ML.MP4.GPX"
    root = ET.parse(gpx_path).getroot()
    times = [node.text for node in root.findall(".//gpx:trkpt/gpx:time", GPX_NS)]
    assert len(times) == 84, f"merged GPX expected 84 points, got {len(times)}"
    assert all(first <= second for first, second in zip(times, times[1:])), (
        "merged GPX timestamps are not nondecreasing"
    )


def check_embedded_capture_date(folder: Path) -> None:
    root = ET.parse(folder / "GOPR-HL1.MP4.GPX").getroot()
    first_time = root.find(".//gpx:trkpt/gpx:time", GPX_NS)
    assert first_time is not None and first_time.text == "2018-01-23T19:44:33.299Z", (
        "GPX points should use the clip's embedded GPSU capture time"
    )


def check_one_second_overlay(folder: Path) -> None:
    root = ET.parse(folder / "GOPR-HL1.MP4.FCPXML").getroot()
    overlay = next(node for node in root.iter("media") if node.attrib.get("id") == "Overlay")
    titles = list(overlay.iter("title"))
    assert titles, "FCPXML did not generate GPS overlay titles"
    for title in titles:
        assert title.attrib.get("duration") == "1s", "GPS overlay duration must remain one second"
        assert duration_seconds(title.attrib["offset"]).is_integer(), (
            "GPS overlay points should keep their one-second cadence"
        )


def check_successive_fcpxml(folder: Path, clips_dir: Path) -> None:
    ffprobe = shutil.which("ffprobe")
    if not ffprobe:
        return
    expected = 0.0
    for filename in SUCCESSIVE_CLIPS:
        clip = clips_dir / filename
        probe = json.loads(run([
            ffprobe, "-v", "error", "-select_streams", "v:0",
            "-show_entries", "stream=duration", "-of", "json", str(clip),
        ]).stdout)
        expected += float(probe["streams"][0]["duration"])
    root = ET.parse(folder / "GOPR2884_ML.MP4.FCPXML").getroot()
    sequences = list(root.iter("sequence"))
    assert sequences and all(
        abs(duration_seconds(node.attrib["duration"]) - expected) <= 0.003
        for node in sequences
    ), "merged FCPXML sequence duration does not match the combined clips"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--clips-dir", type=Path, default=Path(__file__).resolve().parents[2] / "test")
    parser.add_argument("--binary", type=Path, default=Path(__file__).resolve().parents[1] / "GraffitiCL")
    args = parser.parse_args()
    clips_dir = args.clips_dir.resolve()
    binary = args.binary.resolve()
    if not binary.is_file():
        raise SystemExit(f"Build GraffitiCL first: {binary} not found")

    required = set(KNOWN_MARKERS_MS) | set(SUCCESSIVE_CLIPS)
    clips = sorted(path for path in clips_dir.glob("*.MP4") if path.name in required)
    missing = required - {path.name for path in clips}
    if missing:
        raise SystemExit(f"Missing timing fixtures in {clips_dir}: {', '.join(sorted(missing))}")

    with tempfile.TemporaryDirectory(prefix="graffiticL-timing-") as temporary:
        output_dir = Path(temporary)
        for clip in clips:
            (output_dir / clip.name).symlink_to(clip)
        result = run([str(binary), "-d", "-g", "-k", "-f", "-M", str(output_dir)])
        for name, expected_count in (("GOPR-HL1.MP4", 1), ("GOPR-HL5.MP4", 5)):
            assert f"TAG {name}\t{expected_count} HiLigth" in result.stdout, (
                f"Unexpected highlight count for {name}"
            )
        for generated in output_dir.glob("*.FCPXML"):
            ET.parse(generated)
        check_highlight_markers(output_dir)
        check_clip_timing(output_dir, clips)
        check_successive_gpx(output_dir)
        check_embedded_capture_date(output_dir)
        check_one_second_overlay(output_dir)
        check_successive_fcpxml(output_dir, clips_dir)

    print("Timing regression checks passed")


if __name__ == "__main__":
    main()
