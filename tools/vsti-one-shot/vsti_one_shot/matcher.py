"""Preset Matcher — maps recipe slots to factory program indices (musical choice layer)."""

from __future__ import annotations

import hashlib
import json
import subprocess
from pathlib import Path
from typing import Any


def _score_program_name(name: str, style: str, tags: list[str]) -> int:
    """Heuristic score: factory preset name vs style/tags (higher = better fit)."""
    name_l = name.lower()
    style_tokens = {t for t in style.lower().replace(",", " ").split() if len(t) > 2}
    tag_tokens = {t.lower() for t in tags}
    score = 0
    for tok in style_tokens | tag_tokens:
        if tok in name_l:
            score += 3
    # Prefer shorter names for stabs, longer for pads (weak signal)
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


def match_assignments_offline(
    recipe: dict[str, Any],
    vst3_path: Path,
    *,
    assumed_programs: int = 256,
) -> dict[str, Any]:
    """Dry-run matcher when render binary or VST3 is unavailable."""
    style = str(recipe.get("style", ""))
    assignments: list[dict[str, Any]] = []
    for slot in recipe["slots"]:
        slot_id = slot["id"]
        idx = _deterministic_program(style, slot_id, assumed_programs)
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
        "assignments": assignments,
    }


def match_assignments(
    recipe: dict[str, Any],
    vst3_path: Path,
    render_bin: Path,
) -> dict[str, Any]:
    """
    Fill factory program index per slot. Uses preset names when available;
    falls back to deterministic spread across the bank.
    """
    catalog = list_programs(vst3_path, render_bin)
    programs: list[dict[str, Any]] = catalog.get("programs") or []
    num_programs = int(catalog.get("num_programs") or len(programs) or 1)
    style = str(recipe.get("style", ""))

    assignments: list[dict[str, Any]] = []
    for slot in recipe["slots"]:
        slot_id = slot["id"]
        tags = slot.get("tags") or []

        best_index = _deterministic_program(style, slot_id, num_programs)
        best_score = -1
        rationale = "deterministic hash spread across factory bank"

        if programs:
            for p in programs:
                idx = int(p.get("index", 0))
                name = str(p.get("name", ""))
                sc = _score_program_name(name, style, tags)
                if sc > best_score:
                    best_score = sc
                    best_index = idx
                    if sc > 0:
                        rationale = f"name match score {sc}: {name!r}"

        assignments.append(
            {
                "slot_id": slot_id,
                "vst3_path": str(vst3_path.resolve()),
                "program": best_index,
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
    # Per-slot max capture: use the longest assigned slot cap as job default ceiling
    max_cap = max(float(s.get("max_capture_sec", 8.0)) for s in recipe["slots"])
    defaults["max_capture_sec"] = max(defaults.get("max_capture_sec", 8.0), max_cap)

    return {
        "version": 1,
        "style": recipe.get("style"),
        "output_dir": str(output_dir.resolve()),
        "defaults": defaults,
        "renders": renders,
    }
