# MPC 500 vibe project — architecture (kickoff)

Portable **Akai MPC 500** workflow lane for **vibe coding**: sample prep, pad programs, sequences, and tooling that connects Disklordz kits to hardware without pretending we ship Akai firmware.

## Purpose

- **Who:** You (hardware: MPC 500) + agents (repo: WAV factory, PGM export, optional JUCE helpers).
- **Goal:** Fast loop from idea → loaded pads → sequence → performance, with documented constraints of 12-pad / CF / classic OS workflow.
- **Not in v0 scope:** OS patching, reverse-engineering Akai binaries, or redistributing Akai ROM/OS.

Deep research index: [docs/MPC500_RESEARCH.md](../docs/MPC500_RESEARCH.md) · Brainstorm board: [MPC 500 Vibe Lab](https://cursor.com/canvas/store-c392cd47-933a-457e-a0b3-53bad03b7d82/50dde6fd-565c-4927-8aec-67391c29040d).

## Hardware snapshot (MPC 500)

| Topic | Notes |
|-------|--------|
| Pads | **12** velocity-sensitive (MPC Maid “500 profile”) |
| CPU | Hitachi **SH-3 7727** @ ~100 MHz ([MAME mpc notes](https://github.com/mamedev/mame/blob/master/src/mame/akai/mpc3000.cpp)) |
| Storage | **Compact Flash** — projects, samples, OS image (vendor-dependent capacity) |
| MIDI | Classic DIN MIDI; **USB MIDI** only via community OS extensions (e.g. [Open500](https://github.com/M45K/open500) on **OS 1.31**) |
| Sample workflow | Record / load WAV → assign to pads → **PGM** program + sequences on CF |

Treat exact RAM/time limits as **device-specific** — measure on your unit before large batch exports.

## Build & run (software lane v0)

No CMake target yet. Current entrypoints:

```bash
# Research + agent context
cat docs/MPC500_RESEARCH.md

# Disklordz kits → inbox (then manual or MPC Maid to CF)
cd disklordz/daw-inbox && npm start

# Optional: MPC Maid (Java) — drag WAVs → export PGM + samples
# https://github.com/cyriux/mpcmaid
```

```bash
python3 tools/mpc500/stage_cf_inbox.py --lane phonk --from /path/to/wavs --out ~/Music/Disklordz/MPC500_STAGING
```

CF layout spec: [docs/MPC500_CF_LAYOUT.md](../docs/MPC500_CF_LAYOUT.md) · Tool doc: [tools/mpc500/ARCHITECTURE.md](../tools/mpc500/ARCHITECTURE.md).

## Data flow (target vibe loop)

```text
Prompt / idea
  → Disklordz generate OR local WAV chop
  → daw-inbox / staging folder (44.1 kHz mono WAV, MPC-friendly names)
  → stage_cf_inbox.py (phonk pad names) → manual copy to Compact Flash
  → MPC 500: LOAD program → sequence → perform
  → MIDI clock master OUT → slaves (DAW / SP1200 / synth)
```

## Threading / realtime

Hardware handles audio. Repo code stays **offline batch** (Python/Node/Java helpers) unless we add a **JUCE monitor** plugin later (`dsp-vst-architect`).

## Key modules (planned)

| Path | Responsibility |
|------|----------------|
| `docs/MPC500_RESEARCH.md` | Ecosystem links, formats, community tools |
| `mpc500/ARCHITECTURE.md` | This file — lane index |
| `disklordz/daw-inbox/` | ZIP → WAV folders for DAW/MPC drag-in |
| `disklordz/website/` `mpc-ready-808` preset | Neutral peaks / naming aligned to MPC handoff |
| `SP1200/` | Different machine — borrow **chop / pad** UX ideas only |
| `tools/mpc500/` | `stage_cf_inbox.py` — CF-shaped staging + manifest |
| `mpc500/lanes/phonk/` | Pad map, copy checklist |

## Extension points

| Idea | Start here |
|------|------------|
| CF folder convention | Document in `docs/MPC500_RESEARCH.md` after you confirm your CF layout |
| Automate Maid export | Wrap MPC Maid JAR or reimplement PGM subset (Norum format docs) |
| Open500 tools | HUB + Chop Shop if you run OS 1.31 patched — keep optional |
| Emulation / editor | JUCE 12-pad UI sketch (not bit-accurate OS) |
| Disklordz SKU | “MPC500 pack” manifest + MPC Maid project template |

## Related docs

- [DISKLORDZ_SAAS_V0.md](../docs/DISKLORDZ_SAAS_V0.md) — ZIP / MPC-ready marketing
- [disklordz/daw-inbox/ARCHITECTURE.md](../disklordz/daw-inbox/ARCHITECTURE.md)
- [SP1200/ARCHITECTURE.md](../SP1200/ARCHITECTURE.md) — sampler product patterns
- [disklordz/agents/profit/dsp-vst-architect/agent.md](../disklordz/agents/profit/dsp-vst-architect/agent.md) — if we build pad UI / host tools

## Locked profile (2026-11-03)

| Item | Choice |
|------|--------|
| OS | Stock Akai |
| CF layout | [docs/MPC500_CF_LAYOUT.md](../docs/MPC500_CF_LAYOUT.md) v1 |
| Lane | **Phonk** — pads in [lanes/phonk/pad-map.json](lanes/phonk/pad-map.json) |
| Clock | **MPC master** (MIDI clock out) |
| Handoff | **Manual CF copy**; Maid optional later |

**Open on hardware [executed]:** CF capacity (MB/GB) and confirm Akai browser sees `MPC500/BANK_A/PHONK/`.
