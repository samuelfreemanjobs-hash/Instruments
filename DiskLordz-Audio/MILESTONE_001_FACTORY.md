# Milestone 001 — Cursor Factory directive

**Implement in this tree:** `DiskLordz-Audio/` (monorepo root).

## Factory agent

- Rule: `.cursor/rules/disklordz-audio-engineering-lane.mdc`
- Skill: `disklordz-plugin-factory-liaison` or dedicated Factory chat
- Read: `docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md` **§18**, `docs/CHORD_ENGINE_PRD.md`, `docs/DPF_TECHNICAL_ARCHITECTURE.md`

## Stage B — plugin skeleton (current slice)

**Landed:** CMake + JUCE sources in `Products/ChordEngine/Source/` (Milestone 001 shell). Build: [BUILD.md](BUILD.md).

**Samuel (Windows):** compile + DAW scan — then mark Coach `plugin-skeleton` done.

1. ~~CMake at `DiskLordz-Audio/CMakeLists.txt`~~
2. Minimal JUCE plugin (silent audio; MIDI buses enabled for Stage C).
3. Do **not** commit `Build/` artifacts.

## Acceptance (§18.3 report required)

- VST3 artifact path on disk
- DAW load screenshot or log
- `ctest` or documented test command if added
- **Limitations** + **Next task** (Stage C MIDI)

## Out of scope

- SaaS `disklordz/website/`
- Full CHORD ENGINE theory (Stage D+)
- Antigravity-owned drafts under `docs/drafts/` (merge data later)
