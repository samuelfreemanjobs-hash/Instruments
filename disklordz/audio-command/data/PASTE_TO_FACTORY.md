# PASTE_TO_FACTORY (generated)

_Generated 2026-11-09 00:00 UTC · task `plugin-skeleton`_

**Blocker:** human-windows  
**Next owner:** Samuel (Windows build + DAW)  
**Liaison:** skill `disklordz-plugin-factory-liaison`

---

Stage B **JUCE shell is landed** in the private repo `samuelfreemanjobs-hash/DiskLordz-Audio` under `Products/ChordEngine/`.

**Stage:** B — Blank VST3 shell compiles and loads in DAW  
**Slice:** Delegate: blank VST3 that loads

### Build (from `C:\DiskLordz\DiskLordz-Audio`)

```powershell
git pull origin main
powershell -ExecutionPolicy Bypass -File Build\Windows\build.ps1 -Release
```

VST3: `out\build\Products\ChordEngine\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3`

**CI:** not yet — Windows CI is task B in that repo's milestone 003.

**Done when:** You have evidence the plugin scans and opens in REAPER or FL.

**Factory chat:** only if build fails — paste this file + log; rule `disklordz-audio-engineering-lane`.

**Parallel:** Antigravity WO-PREP (docs/batch) — not required for compile.
