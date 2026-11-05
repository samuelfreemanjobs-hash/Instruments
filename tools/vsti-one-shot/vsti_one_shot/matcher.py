"""Preset Matcher — maps recipe slots to factory program indices (musical choice layer)."""

from __future__ import annotations

import hashlib
import json
import subprocess
import tempfile
from pathlib import Path
from typing import Any, Literal

from vsti_one_shot.preview_analyze import analyze_preview_wav, score_metrics_for_kind

MatchMode = Literal["names", "preview", "hybrid"]

PREVIEW_MAX_SEC = 1.5
PREVIEW_CANDIDATES = 8


def _score_program_name(name: str, style: str, tags: list[str]) -> int:
    """Heuristic score: factory preset name vs style/tags (higher = better fit)."""
    name_l = name.lower()
    style_tokens = {t for t in style.lower().replace(",", " ").split() if len(t) > 2}
    tag_tokens = {t.lower() for t in tags}
    score = 0
    for tok in style_tokens | tag_tokens:
        if tok in name_l:
            score += 3
    if "stab" in tag_tokens and "short" in name_l:
        score += 2
    if "pad" in tag_tokens and "pad" in name_l:
        score += 2
    if "key" in tag_tokens and ("key" in name_l or "piano" in name_l):
        score += 2
    return score


def _deterministic_program(style: str, slot_id: str, num_programs: int) -> int:
    if num_programs <= 0:
        return 0
    h = hashlib.sha256(f"{style}::{slot_id}".encode()).hexdigest()
    return int(h[:8], 16) % num_programs


def list_programs(vst3_path: Path, render_bin: Path) -> dict[str, Any]:
    proc = subprocess.run(
        [str(render_bin), "--list-programs", str(vst3_path)],
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        raise RuntimeError(proc.stderr.strip() or "list-programs failed")
    return json.loads(proc.stdout)


def _candidate_indices(
    slot: dict[str, Any],
    style: str,
    programs: list[dict[str, Any]],
    num_programs: int,
    *,
    limit: int = PREVIEW_CANDIDATES,
) -> list[int]:
    """Rank program indices for a slot; always includes a slot-specific deterministic pick."""
    slot_id = slot["id"]
    tags = slot.get("tags") or []
    scored: list[tuple[int, int]] = []

    for p in programs:
        idx = int(p.get("index", 0))
        name = str(p.get("name", ""))
        scored.append((_score_program_name(name, style, tags), idx))

    scored.sort(key=lambda t: (-t[0], t[1]))
    chosen: list[int] = []
    det = _deterministic_program(style, slot_id, num_programs)
    chosen.append(det)

    for _score, idx in scored:
        if idx not in chosen:
            chosen.append(idx)
        if len(chosen) >= limit:
            break

    if len(chosen) < limit:
        for offset in range(limit * 2):
            idx = (det + offset * 7 + 1) % max(num_programs, 1)
            if idx not in chosen:
                chosen.append(idx)
            if len(chosen) >= limit:
                break

    return chosen[:limit]


def _render_preview(
    render_bin: Path,
    vst3_path: Path,
    program: int,
    midi: int,
    out_wav: Path,
) -> bool:
    proc = subprocess.run(
        [
            str(render_bin),
            "--preview",
            str(out_wav),
            str(vst3_path),
            str(program),
            str(midi),
            "100",
            str(PREVIEW_MAX_SEC),
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    return proc.returncode == 0 and out_wav.is_file()


def _pick_program_for_slot(
    slot: dict[str, Any],
    style: str,
    programs: list[dict[str, Any]],
    num_programs: int,
    vst3_path: Path,
    render_bin: Path,
    match_mode: MatchMode,
    preview_dir: Path | None,
    used_counts: dict[int, int],
) -> tuple[int, str]:
    slot_id = slot["id"]
    tags = slot.get("tags") or []
    kind = str(slot.get("kind", "stab"))
    root_midi = int(slot["root_midi"])

    program_names = {int(p.get("index", 0)): str(p.get("name", "")) for p in programs}
    candidates = _candidate_indices(slot, style, programs, num_programs)

    best_index = candidates[0]
    best_total = -1e9
    rationale = "deterministic seed"

    for program in candidates:
        name = program_names.get(program, "")
        name_score = float(_score_program_name(name, style, tags))
        audio_score = 0.0

        if match_mode in ("preview", "hybrid") and preview_dir is not None:
            preview_path = preview_dir / f"{slot_id}_p{program}.wav"
            if _render_preview(render_bin, vst3_path, program, root_midi, preview_path):
                try:
                    metrics = analyze_preview_wav(preview_path)
                    audio_score = score_metrics_for_kind(metrics, kind)
                except (OSError, ValueError):
                    audio_score = -50.0

        if match_mode == "names":
            total = name_score - used_counts.get(program, 0) * 4.0
        elif match_mode == "preview":
            total = audio_score - used_counts.get(program, 0) * 4.0
        else:
            total = name_score * 0.35 + audio_score * 0.65 - used_counts.get(program, 0) * 4.0

        if total > best_total:
            best_total = total
            best_index = program
            if match_mode == "names" and name_score > 0:
                rationale = f"name score {int(name_score)}: {name!r}"
            elif match_mode == "preview":
                rationale = f"preview audio score {audio_score:.1f} (program {program})"
            elif audio_score > 0:
                rationale = (
                    f"hybrid name={name_score:.0f} audio={audio_score:.1f} "
                    f"program {program} {name!r}"
                )
            else:
                rationale = f"hybrid fallback program {program}"

    return best_index, rationale


def match_assignments_offline(
    recipe: dict[str, Any],
    vst3_path: Path,
    *,
    assumed_programs: int = 256,
) -> dict[str, Any]:
    """Dry-run matcher when render binary or VST3 is unavailable."""
    style = str(recipe.get("style", ""))
    assignments: list[dict[str, Any]] = []
    used: dict[int, int] = {}
    for slot in recipe["slots"]:
        slot_id = slot["id"]
        idx = _deterministic_program(style, slot_id, assumed_programs)
        used[idx] = used.get(idx, 0) + 1
        assignments.append(
            {
                "slot_id": slot_id,
                "vst3_path": str(vst3_path.resolve()),
                "program": idx,
                "root_midi": int(slot["root_midi"]),
                "max_capture_sec": float(slot.get("max_capture_sec", 8.0)),
                "matcher_rationale": "offline dry-run (assumed factory bank)",
            }
        )
    return {
        "version": 1,
        "style": style,
        "style_slug": recipe.get("style_slug"),
        "vst3_path": str(vst3_path.resolve()),
        "plugin_name": None,
        "num_programs": assumed_programs,
        "match_mode": "offline",
        "assignments": assignments,
    }


def match_assignments(
    recipe: dict[str, Any],
    vst3_path: Path,
    render_bin: Path,
    *,
    match_mode: MatchMode = "hybrid",
    work_dir: Path | None = None,
) -> dict[str, Any]:
    """
    Fill factory program index per slot.
    hybrid: name heuristics + short preview renders + reuse spread.
    """
    catalog = list_programs(vst3_path, render_bin)
    programs: list[dict[str, Any]] = catalog.get("programs") or []
    num_programs = int(catalog.get("num_programs") or len(programs) or 1)
    style = str(recipe.get("style", ""))

    preview_dir: Path | None = None
    if match_mode in ("preview", "hybrid"):
        base = work_dir or Path(tempfile.mkdtemp(prefix="vsti_preview_"))
        preview_dir = base / "matcher_previews"
        preview_dir.mkdir(parents=True, exist_ok=True)

    assignments: list[dict[str, Any]] = []
    used_counts: dict[int, int] = {}

    total = len(recipe["slots"])
    for n, slot in enumerate(recipe["slots"], start=1):
        if total >= 10 and (n == 1 or n % 10 == 0 or n == total):
            print(f"  matcher: slot {n}/{total} ({slot['id']})", flush=True)
        program, rationale = _pick_program_for_slot(
            slot,
            style,
            programs,
            num_programs,
            vst3_path,
            render_bin,
            match_mode,
            preview_dir,
            used_counts,
        )
        used_counts[program] = used_counts.get(program, 0) + 1

        assignments.append(
            {
                "slot_id": slot["id"],
                "vst3_path": str(vst3_path.resolve()),
                "program": program,
                "root_midi": int(slot["root_midi"]),
                "max_capture_sec": float(slot.get("max_capture_sec", 8.0)),
                "matcher_rationale": rationale,
            }
        )

    return {
        "version": 1,
        "style": style,
        "style_slug": recipe.get("style_slug"),
        "vst3_path": str(vst3_path.resolve()),
        "plugin_name": catalog.get("plugin_name"),
        "num_programs": num_programs,
        "match_mode": match_mode,
        "assignments": assignments,
    }


def build_render_job(
    recipe: dict[str, Any],
    assignments_doc: dict[str, Any],
    output_dir: Path,
) -> dict[str, Any]:
    """Merge recipe + assignments into strict job JSON for VstiOneShotRender."""
    by_slot = {a["slot_id"]: a for a in assignments_doc["assignments"]}
    renders: list[dict[str, Any]] = []

    for slot in recipe["slots"]:
        sid = slot["id"]
        if sid not in by_slot:
            raise ValueError(f"matcher missing assignment for slot {sid}")
        a = by_slot[sid]
        if a.get("program") is None:
            raise ValueError(f"assignment missing program for {sid}")

        renders.append(
            {
                "slot_id": sid,
                "vst3_path": a["vst3_path"],
                "program": int(a["program"]),
                "root_midi": int(a["root_midi"]),
                "max_capture_sec": float(slot.get("max_capture_sec", 8.0)),
            }
        )

    defaults = dict(recipe.get("defaults") or {})
    max_cap = max(float(s.get("max_capture_sec", 8.0)) for s in recipe["slots"])
    defaults["max_capture_sec"] = max(defaults.get("max_capture_sec", 8.0), max_cap)

    return {
        "version": 1,
        "style": recipe.get("style"),
        "output_dir": str(output_dir.resolve()),
        "defaults": defaults,
        "renders": renders,
    }


def unique_program_count(assignments_doc: dict[str, Any]) -> int:
    progs = {int(a["program"]) for a in assignments_doc.get("assignments", [])}
    return len(progs)
