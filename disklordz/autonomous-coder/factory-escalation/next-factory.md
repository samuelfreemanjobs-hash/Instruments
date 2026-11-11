# Factory escalation — plugin-skeleton (Stage B verify)

_Updated 2026-11-09 · PR **#112** merged to `main` (`c6e3bc2`)_

## Status

| Field | Value |
|-------|--------|
| Blocker | **human-windows** (DAW scan evidence) |
| Sprint | `plugin-skeleton` |
| Owner | **Samuel** (build + REAPER/FL) |
| Code | **`main`** — `DiskLordz-Audio/Products/ChordEngine/` · Windows CI: **DiskLordz Audio (Windows VST3)** |
| Liaison skill | `disklordz-plugin-factory-liaison` |

## Build (Samuel)

From `C:\dev\Instruments`:

```powershell
git pull origin main
powershell -ExecutionPolicy Bypass -File .\DiskLordz-Audio\build-windows.ps1
```

VST3: path printed by `build-windows.ps1`, or GitHub Actions artifact **`chord-engine-vst3-windows-*`** (workflow **DiskLordz Audio (Windows VST3)**).

## Factory chat (only if build fails)

Paste [`disklordz/audio-command/data/PASTE_TO_FACTORY.md`](../../audio-command/data/PASTE_TO_FACTORY.md) + failure log. Rule: `disklordz-audio-engineering-lane`.

## Done when

- Plugin scans and opens in REAPER or FL.
- Coach: mark **`plugin-skeleton`** complete.
- Report §18.3: Completed, Files, Build path, Tests, Limitations, Next task (= Stage C).

## Parallel (do not block compile)

Open Antigravity inbox (docs/batch — not VST compile):

| Handoff | WO | Status |
|---------|-----|--------|
| `HO-20261111-3A89` | WO-AG-002 (UI theme tokens batch) | open |
| `HO-20261109-2C87` | WO-AG-001 (genre rules JSON) | open |
| `HO-20261108-AD94` | WO-PREP-003 (Milestone 001 README) | open |
| `HO-20261108-6B03` | WO-PREP-004 (REAPER smoke template) | open |

Done: `HO-20261108-053B`, `HO-20261108-CAAD`. Windows: `watch-handoffs.ps1 -Once`.

## Reference

- [CURSOR_FACTORY_LIAISON.md](../../../docs/CURSOR_FACTORY_LIAISON.md)
- [BUILD.md](../../../DiskLordz-Audio/BUILD.md)
- [CHORD_ENGINE_PRD.md](../../../docs/CHORD_ENGINE_PRD.md)
