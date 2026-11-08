#!/usr/bin/env python3
"""Emit next PASTE_TO_FACTORY block for plugin-factory-liaison (audio plugin lane)."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
PROGRESS = REPO / "disklordz/audio-command/data/progress.json"
PLAYBOOKS = REPO / "disklordz/audio-command/data/task-playbooks.json"
OUT = REPO / "disklordz/audio-command/data/PASTE_TO_FACTORY.md"
STATE = REPO / "disklordz/audio-command/data/factory-queue-state.json"

# Roadmap §18 stage hints by sprint task id
STAGE_BY_TASK = {
    "plugin-skeleton": ("B", "Blank VST3 shell compiles and loads in DAW"),
    "midi-output": ("C", "MIDI reaches instrument track"),
    "theory-engine": ("D", "Music theory unit tests"),
    "chord-engine": ("E", "Extended chord library data-driven"),
    "progression-generator": ("F", "4-chord progression generator"),
    "first-interface": ("G", "Key + grid UI wired"),
}


def load(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def next_open_task(progress: dict, playbooks: dict) -> tuple[str, dict] | None:
    completed = set(progress.get("completed_ids") or [])
    for tid in playbooks:
        if tid not in completed:
            pb = playbooks[tid]
            if pb.get("owner") == "cursor":
                return tid, pb
    return None


def blocker(progress: dict, task_id: str) -> str | None:
    completed = set(progress.get("completed_ids") or [])
    if "dev-environment" not in completed and task_id != "dev-environment":
        return "human-windows"
    return None


def render_factory(task_id: str, pb: dict, stage: tuple[str, str]) -> str:
    letter, blurb = stage
    return f"""# PASTE_TO_FACTORY (generated)

_Generated {datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC")} · task `{task_id}`_

**Blocker:** none  
**Next owner:** Cursor Factory (separate chat)  
**Liaison:** skill `disklordz-plugin-factory-liaison`

---

DiskLordz Audio — Factory implementer only.

**Stage:** {letter} — {blurb}  
**Slice:** {pb.get("headline", task_id)}  
**Read:** `docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md` §18, `docs/DPF_TECHNICAL_ARCHITECTURE.md`, `docs/CHORD_ENGINE_PRD.md`  
**Out of scope:** SaaS website, Stripe, marketing copy

**Acceptance:**
- {pb.get("done_when", "See CHORD_ENGINE_PRD.md")}
- Build: cmake artifact path on Windows x64
- Report §18.3: Completed, Files, Build, Tests, Limitations, Next task

**Parallel:** Antigravity may run WO-PREP / WO-CC per `sync-all-antigravity.sh` — do not wait on it for compile.
"""


def render_blocked(reason: str, task_id: str | None) -> str:
    return f"""# PASTE_TO_FACTORY (blocked)

_Generated {datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC")}_

**Blocker:** `{reason}`  
**Next owner:** Samuel (Windows dev box)  
**Sprint task:** `{task_id or "dev-environment"}`

Finish Coach task **dev-environment** before Factory opens Milestone 001 C++.

No Factory prompt until blocker cleared.
"""


def main() -> int:
    if not PROGRESS.is_file() or not PLAYBOOKS.is_file():
        OUT.write_text(render_blocked("missing-data", None), encoding="utf-8")
        print(f"Wrote {OUT} (blocked)")
        return 1

    progress = load(PROGRESS)
    playbooks = load(PLAYBOOKS)
    nxt = next_open_task(progress, playbooks)

    if not nxt:
        body = "# PASTE_TO_FACTORY (idle)\n\nSprint checklist complete or no `cursor`-owned tasks.\n"
        OUT.write_text(body, encoding="utf-8")
        print("queue idle")
        return 0

    task_id, pb = nxt
    block = blocker(progress, task_id)
    if block:
        OUT.write_text(render_blocked(block, task_id), encoding="utf-8")
        print(f"blocked={block}")
        return 0

    stage = STAGE_BY_TASK.get(task_id, ("B", pb.get("headline", task_id)))
    OUT.write_text(render_factory(task_id, pb, stage), encoding="utf-8")
    state = {
        "updated_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "task_id": task_id,
        "stage": stage[0],
    }
    STATE.write_text(json.dumps(state, indent=2) + "\n", encoding="utf-8")
    print(f"factory_next={task_id} stage={stage[0]}")
    print(f"Wrote {OUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
