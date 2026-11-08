# Milestone 001 — Cursor Factory directive

**Implement in this tree:** `DiskLordz-Audio/` (monorepo root).

## Factory agent

- Rule: `.cursor/rules/disklordz-audio-engineering-lane.mdc`
- Skill: `disklordz-plugin-factory-liaison` or dedicated Factory chat
- Read: `docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md` **§18**, `docs/CHORD_ENGINE_PRD.md`, `docs/DPF_TECHNICAL_ARCHITECTURE.md`

## Stage B — plugin skeleton (current slice)

1. Add **CMake** at `DiskLordz-Audio/CMakeLists.txt` (and `Products/ChordEngine/`) targeting **Windows x64 VST3**.
2. Minimal JUCE plugin: passes audio/MIDI through or silent; **must scan and open in REAPER/FL**.
3. Place implementation under `Products/ChordEngine/` and shared stubs under `DPF-Core/`, `DPF-MIDI/` as needed for compile.
4. Do **not** commit `Build/` artifacts; use `Build/Windows/` for local/cmake cache per `.gitignore`.

## Acceptance (§18.3 report required)

- VST3 artifact path on disk
- DAW load screenshot or log
- `ctest` or documented test command if added
- **Limitations** + **Next task** (Stage C MIDI)

## Out of scope

- SaaS `disklordz/website/`
- Full CHORD ENGINE theory (Stage D+)
- Antigravity-owned drafts under `docs/drafts/` (merge data later)
