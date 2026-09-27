#!/usr/bin/env python3
"""Build plugin targets and write listenable preview WAVs for VS Code / agent workflows."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path

OPS_ROOT = Path(__file__).resolve().parent
REPO_ROOT = OPS_ROOT.parent
PROFILE_PATH = OPS_ROOT / "preview-profile.json"
PREVIEWS_DIR = OPS_ROOT / "previews"
MANIFEST_PATH = PREVIEWS_DIR / "preview-manifest.json"
LATEST_DIR = PREVIEWS_DIR / "latest"

OFFLINE_RENDER = REPO_ROOT / "build/OfflineRender"
JD_STANDALONE = REPO_ROOT / "build/JDUpgraded_artefacts/Release/Standalone/JD Upgraded"
WAVE909_STANDALONE = (
    REPO_ROOT / "build/Wave909/Wave909_artefacts/Release/Standalone/WAVE-909"
)
WAVE909_VST3 = REPO_ROOT / "build/Wave909/Wave909_artefacts/Release/VST3/WAVE-909.vst3"

if str(OPS_ROOT) not in sys.path:
    sys.path.insert(0, str(OPS_ROOT))

from business_pipeline import ensure_cmake_configured, _run  # noqa: E402


@dataclass
class PreviewClip:
    id: str
    label: str
    wav_path: str
    program: int
    midi_note: int
    seconds: float
    peak_db: float | None = None


@dataclass
class PreviewRun:
    ok: bool
    started_at: float
    finished_at: float
    clips: list[PreviewClip]
    standalone: dict[str, str]
    messages: list[str]

    def to_dict(self) -> dict:
        return {
            "ok": self.ok,
            "started_at": self.started_at,
            "finished_at": self.finished_at,
            "duration_s": self.finished_at - self.started_at,
            "clips": [asdict(c) for c in self.clips],
            "standalone": self.standalone,
            "messages": self.messages,
            "listen_hint": (
                "Open WAV files under vst-testing-ops/previews/latest/ in VS Code "
                "(Audio Preview extension) or any media player. "
                "For live UI, run the Standalone binary paths in standalone."
            ),
        }


def load_profile() -> dict:
    if not PROFILE_PATH.is_file():
        raise SystemExit(f"missing profile: {PROFILE_PATH}")
    return json.loads(PROFILE_PATH.read_text(encoding="utf-8"))


def build_targets(targets: list[str]) -> tuple[bool, str]:
    cmd = ["cmake", "--build", "build", "-j", "--target", *targets]
    code, out = _run(cmd, cwd=REPO_ROOT)
    return code == 0, out


def wav_peak_db(path: Path) -> float | None:
    try:
        data = path.read_bytes()
        if len(data) < 44:
            return None
        # Minimal RIFF PCM parse (OfflineRender writes standard WAV)
        fmt_off = data.find(b"fmt ")
        if fmt_off < 0:
            return None
        data_off = data.find(b"data")
        if data_off < 0:
            return None
        data_start = data_off + 8
        samples = memoryview(data)[data_start:]
        if len(samples) < 4:
            return None
        peak = 0.0
        step = 4  # stereo 16-bit assumed; OfflineRender uses 24-bit — use float fallback
        # JUCE 24-bit packed: use subprocess sox if available
        proc = subprocess.run(
            ["sox", str(path), "-n", "stat"],
            capture_output=True,
            text=True,
            check=False,
        )
        if proc.returncode == 0:
            for line in proc.stderr.splitlines():
                if "Maximum amplitude" in line:
                    try:
                        amp = float(line.split(":")[-1].strip())
                        if amp > 0:
                            import math

                            return 20.0 * math.log10(amp)
                    except ValueError:
                        pass
        # fallback: scan 16-bit if sox missing
        import array

        arr = array.array("h")
        arr.frombytes(bytes(samples[: len(samples) // 2 * 2]))
        if not arr:
            return None
        peak_i = max(abs(x) for x in arr)
        if peak_i <= 0:
            return -120.0
        import math

        return 20.0 * math.log10(peak_i / 32768.0)
    except OSError:
        return None


def render_jd_clips(run_id: str, profile: dict) -> tuple[list[PreviewClip], list[str]]:
    if not OFFLINE_RENDER.is_file():
        return [], ["OfflineRender binary missing after build."]

    out_dir = PREVIEWS_DIR / run_id
    out_dir.mkdir(parents=True, exist_ok=True)
    clips: list[PreviewClip] = []
    errors: list[str] = []

    for row in profile.get("jd_upgraded", []):
        clip_id = row["id"]
        out_wav = out_dir / f"jd-{clip_id}.wav"
        cmd = [
            str(OFFLINE_RENDER),
            str(out_wav),
            str(row["program"]),
            str(row["midiNote"]),
            str(row.get("velocity", 100)),
            str(row.get("seconds", 2.0)),
            str(row.get("sampleRate", 44100)),
        ]
        code, out = _run(cmd, cwd=REPO_ROOT)
        if code != 0 or not out_wav.is_file():
            errors.append(f"render failed {clip_id}: {out[-2000:]}")
            continue
        clips.append(
            PreviewClip(
                id=clip_id,
                label=row.get("label", clip_id),
                wav_path=str(out_wav.relative_to(REPO_ROOT)),
                program=int(row["program"]),
                midi_note=int(row["midiNote"]),
                seconds=float(row.get("seconds", 2.0)),
                peak_db=wav_peak_db(out_wav),
            )
        )
    return clips, errors


def sync_latest(clips: list[PreviewClip]) -> None:
    LATEST_DIR.mkdir(parents=True, exist_ok=True)
    for old in LATEST_DIR.glob("*.wav"):
        old.unlink()
    for clip in clips:
        src = REPO_ROOT / clip.wav_path
        dest = LATEST_DIR / Path(clip.wav_path).name
        if src.is_file():
            dest.write_bytes(src.read_bytes())


def launch_standalone(which: str) -> tuple[bool, str]:
    paths = {
        "jd": JD_STANDALONE,
        "wave909": WAVE909_STANDALONE,
    }
    path = paths.get(which)
    if path is None or not path.is_file():
        return False, f"Standalone not found: {path}"
    env = os.environ.copy()
    subprocess.Popen(
        [str(path)],
        cwd=str(path.parent),
        env=env,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        start_new_session=True,
    )
    return True, f"Launched {path.relative_to(REPO_ROOT)}"


def detect_targets_from_git() -> list[str]:
    proc = subprocess.run(
        ["git", "diff", "--name-only", "HEAD"],
        cwd=str(REPO_ROOT),
        capture_output=True,
        text=True,
        check=False,
    )
    names = [n.strip() for n in proc.stdout.splitlines() if n.strip()]
    if not names:
        return [
            "OfflineRender",
            "JDUpgraded_Standalone",
            "Wave909_VST3",
            "Wave909Tests",
        ]
    jd = any(
        n.startswith(("Source/", "tools/", "CMakeLists.txt", "tests/golden/"))
        for n in names
    )
    w909 = any(n.startswith("Wave909/") for n in names)
    targets: list[str] = []
    if jd:
        targets.extend(["OfflineRender", "JDUpgraded_Standalone"])
    if w909:
        targets.extend(["Wave909_VST3", "Wave909_Standalone", "Wave909Tests"])
    if not targets:
        targets = ["OfflineRender", "JDUpgraded_Standalone"]
    return list(dict.fromkeys(targets))


def run_preview(
    *,
    scope: str,
    launch: str | None,
    skip_build: bool,
    auto_git: bool,
) -> PreviewRun:
    t0 = time.time()
    messages: list[str] = []

    ok_cfg, cfg_out = ensure_cmake_configured(REPO_ROOT)
    if not ok_cfg:
        return PreviewRun(False, t0, time.time(), [], {}, [cfg_out])
    messages.append(cfg_out.strip() or "CMake OK.")

    if scope == "jd":
        build_list = ["OfflineRender", "JDUpgraded_Standalone"]
    elif scope == "wave909":
        build_list = ["Wave909_VST3", "Wave909_Standalone", "Wave909Tests"]
    else:
        build_list = [
            "OfflineRender",
            "JDUpgraded_Standalone",
            "Wave909_VST3",
            "Wave909_Standalone",
            "Wave909Tests",
        ]

    if auto_git:
        build_list = detect_targets_from_git()
        messages.append(f"git-scoped build targets: {', '.join(build_list)}")

    if not skip_build:
        ok, bout = build_targets(build_list)
        messages.append(bout[-4000:] if len(bout) > 4000 else bout)
        if not ok:
            return PreviewRun(False, t0, time.time(), [], {}, messages + ["Build failed."])

    if "Wave909Tests" in build_list:
        code, tout = _run(["ctest", "-R", "Wave909", "--output-on-failure"], cwd=REPO_ROOT / "build")
        messages.append(tout[-3000:])
        if code != 0:
            return PreviewRun(False, t0, time.time(), [], {}, messages + ["Wave909Tests failed."])

    profile = load_profile()
    run_id = time.strftime("%Y%m%d-%H%M%S")
    clips: list[PreviewClip] = []
    if scope in ("jd", "all"):
        jd_clips, errs = render_jd_clips(run_id, profile)
        clips.extend(jd_clips)
        messages.extend(errs)

    standalone: dict[str, str] = {}
    for key, path in (("jd_upgraded", JD_STANDALONE), ("wave909", WAVE909_STANDALONE)):
        if path.is_file():
            standalone[key] = str(path.relative_to(REPO_ROOT))
    if WAVE909_VST3.is_dir():
        standalone["wave909_vst3"] = str(WAVE909_VST3.relative_to(REPO_ROOT))

    if clips:
        sync_latest(clips)

    if launch:
        ok_launch, msg = launch_standalone(launch)
        messages.append(msg)
        if not ok_launch and scope != "wave909":
            messages.append("Tip: build Standalone first or use --scope wave909.")

    PREVIEWS_DIR.mkdir(parents=True, exist_ok=True)
    ok = True
    if scope in ("jd", "all") and not clips:
        ok = False
        messages.append("No JD preview WAVs produced.")
    if scope == "wave909" and not standalone.get("wave909") and not standalone.get("wave909_vst3"):
        ok = False
        messages.append("Wave909 artefacts missing after build.")

    run = PreviewRun(
        ok=ok,
        started_at=t0,
        finished_at=time.time(),
        clips=clips,
        standalone=standalone,
        messages=messages,
    )
    manifest = run.to_dict()
    manifest["run_id"] = run_id
    manifest["scope"] = scope
    MANIFEST_PATH.write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    return run


def main() -> int:
    parser = argparse.ArgumentParser(description="VST preview agent — build + OfflineRender WAVs.")
    parser.add_argument(
        "--scope",
        choices=("jd", "wave909", "all"),
        default="all",
        help="Which product lane to preview (default: all)",
    )
    parser.add_argument(
        "--launch-standalone",
        choices=("jd", "wave909"),
        default=None,
        help="Start interactive Standalone (live UI, no WAV render)",
    )
    parser.add_argument("--skip-build", action="store_true", help="Only render (binaries must exist)")
    parser.add_argument(
        "--from-git",
        action="store_true",
        help="Build only targets implied by git diff (vs HEAD)",
    )
    args = parser.parse_args()

    run = run_preview(
        scope=args.scope,
        launch=args.launch_standalone,
        skip_build=args.skip_build,
        auto_git=args.from_git,
    )
    print(json.dumps(run.to_dict(), indent=2))
    if run.clips:
        print("\nListen in VS Code:", ", ".join(c.wav_path for c in run.clips))
        print("Stable folder:", LATEST_DIR.relative_to(REPO_ROOT))
    if run.standalone:
        print("\nStandalone (live preview):", json.dumps(run.standalone, indent=2))
    return 0 if run.ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
