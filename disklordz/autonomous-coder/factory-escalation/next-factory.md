# Factory escalation — plugin-skeleton (Stage B verify)

_Updated 2026-11-09 · product code moved to the private repo `samuelfreemanjobs-hash/DiskLordz-Audio`_

## Status

| Field | Value |
|-------|--------|
| Blocker | **human-windows** (DAW scan evidence) |
| Sprint | `plugin-skeleton` |
| Owner | **Samuel** (build + REAPER/FL) |
| Code | private repo **`samuelfreemanjobs-hash/DiskLordz-Audio`** — `Products/ChordEngine/` · Windows CI: not yet (task B, milestone 003) |
| Liaison skill | `disklordz-plugin-factory-liaison` |

## Build (Samuel)

From `C:\DiskLordz\DiskLordz-Audio`:

```powershell
git pull origin main
powershell -ExecutionPolicy Bypass -File Build\Windows\build.ps1 -Release
```

VST3: `out\build\Products\ChordEngine\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3`

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
- `DOCUMENTATION/BUILD.md` in the DiskLordz-Audio repo
- [CHORD_ENGINE_PRD.md](../../../docs/CHORD_ENGINE_PRD.md)
