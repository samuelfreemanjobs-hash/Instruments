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

VST3 (typical): `DiskLordz-Audio\Build\Windows\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3`

## Factory chat (only if build fails)

Paste [`disklordz/audio-command/data/PASTE_TO_FACTORY.md`](../../audio-command/data/PASTE_TO_FACTORY.md) + failure log. Rule: `disklordz-audio-engineering-lane`.

## Done when

- Plugin scans and opens in REAPER or FL.
- Coach: mark **`plugin-skeleton`** complete.
- Report §18.3: Completed, Files, Build path, Tests, Limitations, Next task (= Stage C).

## Parallel (do not block compile)

- Antigravity WO-PREP / WO-CC per `sync-all-antigravity.sh` (tokens not required for skeleton).

## Reference

- [CURSOR_FACTORY_LIAISON.md](../../../docs/CURSOR_FACTORY_LIAISON.md)
- [BUILD.md](../../../DiskLordz-Audio/BUILD.md)
- [CHORD_ENGINE_PRD.md](../../../docs/CHORD_ENGINE_PRD.md)
