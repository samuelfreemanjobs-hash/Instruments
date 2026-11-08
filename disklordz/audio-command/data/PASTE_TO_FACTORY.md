# PASTE_TO_FACTORY (generated)

_Generated 2026-11-08 16:00 UTC · task `plugin-skeleton`_

**Blocker:** human-windows  
**Next owner:** Samuel (Windows build + DAW)  
**Liaison:** skill `disklordz-plugin-factory-liaison`

---

Stage B **JUCE shell is landed** under `DiskLordz-Audio/Products/ChordEngine/`.

**Stage:** B — Blank VST3 shell compiles and loads in DAW  
**Slice:** Delegate: blank VST3 that loads

### Build (from `C:\dev\Instruments`)

```powershell
git pull origin cursor/audio-business-copilot-b828
powershell -ExecutionPolicy Bypass -File .\DiskLordz-Audio\build-windows.ps1
```

VST3: `DiskLordz-Audio\Build\Windows\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3`

**Done when:** You have evidence the plugin scans and opens in REAPER or FL.

**Factory chat:** only if build fails — paste this file + log; rule `disklordz-audio-engineering-lane`.

**Parallel:** Antigravity WO-PREP (docs/batch) — not required for compile.
