# PASTE_TO_FACTORY (generated)

_Generated 2026-11-09 00:00 UTC · task `plugin-skeleton`_

**Blocker:** human-windows  
**Next owner:** Samuel (Windows build + DAW)  
**Liaison:** skill `disklordz-plugin-factory-liaison`

---

Stage B **JUCE shell is landed** under `DiskLordz-Audio/Products/ChordEngine/`.

**Stage:** B — Blank VST3 shell compiles and loads in DAW  
**Slice:** Delegate: blank VST3 that loads

### Build (from `C:\dev\Instruments`)

```powershell
git pull origin main
powershell -ExecutionPolicy Bypass -File .\DiskLordz-Audio\build-windows.ps1
```

VST3: printed at end of `build-windows.ps1` (search under `DiskLordz-Audio\Build\Windows\` for `CHORD ENGINE.vst3`)

**Or CI:** GitHub → Actions → **DiskLordz Audio (Windows VST3)** → download artifact `chord-engine-vst3-windows-*` (no local compile).

**Done when:** You have evidence the plugin scans and opens in REAPER or FL.

**Factory chat:** only if build fails — paste this file + log; rule `disklordz-audio-engineering-lane`.

**Parallel:** Antigravity WO-PREP (docs/batch) — not required for compile.
