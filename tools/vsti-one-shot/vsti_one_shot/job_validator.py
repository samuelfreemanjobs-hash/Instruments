"""Validate render job JSON before invoking VstiOneShotRender."""

from __future__ import annotations

from pathlib import Path
from typing import Any


def validate_job(job: dict[str, Any]) -> list[str]:
    errors: list[str] = []

    if job.get("version") != 1:
        errors.append("job.version must be 1")

    output_dir = job.get("output_dir")
    if not output_dir or not str(output_dir).strip():
        errors.append("job.output_dir is required")

    defaults = job.get("defaults")
    if not isinstance(defaults, dict):
        errors.append("job.defaults object required")
        defaults = {}

    for key in (
        "sample_rate",
        "velocity",
        "channels",
        "normalize_peak_db",
        "tail_silence_db",
        "max_capture_sec",
    ):
        if key not in defaults:
            errors.append(f"defaults.{key} missing")

    if defaults.get("channels") not in (None, "mono", "stereo"):
        errors.append("defaults.channels must be mono or stereo")

    renders = job.get("renders")
    if not isinstance(renders, list) or len(renders) == 0:
        errors.append("renders must be a non-empty array")
        return errors

    seen_slots: set[str] = set()
    for i, r in enumerate(renders):
        if not isinstance(r, dict):
            errors.append(f"renders[{i}] must be object")
            continue
        sid = r.get("slot_id")
        if not sid:
            errors.append(f"renders[{i}].slot_id missing")
        elif sid in seen_slots:
            errors.append(f"duplicate slot_id {sid}")
        else:
            seen_slots.add(str(sid))

        if not r.get("vst3_path"):
            errors.append(f"renders[{i}].vst3_path missing")
        elif not Path(str(r["vst3_path"])).exists():
            errors.append(f"renders[{i}].vst3_path not found: {r['vst3_path']}")

        if r.get("program") is None:
            errors.append(f"renders[{i}].program missing")

        midi = r.get("root_midi")
        if midi is None or not (0 <= int(midi) <= 127):
            errors.append(f"renders[{i}].root_midi must be 0..127")

    return errors
