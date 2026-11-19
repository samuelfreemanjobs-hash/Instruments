# Build — DiskLordz Audio (CHORD ENGINE)

## Windows (primary)

From monorepo root (matches dev checkout):

```powershell
cd C:\dev\Instruments
git pull origin main
.\DiskLordz-Audio\build-windows.ps1
```

Or run CMake directly (Visual Studio 2022 x64):

```powershell
cmake -B DiskLordz-Audio\Build\Windows -S DiskLordz-Audio -G "Visual Studio 17 2022" -A x64
cmake --build DiskLordz-Audio\Build\Windows --config Release --target ChordEngine_VST3
```

VST3 artifact (typical, from repo root):

`DiskLordz-Audio\Build\Windows\Products\ChordEngine\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3` (CMake may also nest under `Build\Windows\ChordEngine_artefacts\` on some generators — use `build-windows.ps1` output line)

## GitHub Actions (Windows artifact)

Workflow **DiskLordz Audio (Windows VST3)** (`.github/workflows/disklordz-audio-windows.yml`) runs on `windows-latest` for pushes/PRs that touch `DiskLordz-Audio/`.

1. Open the run on GitHub → **Artifacts** → `chord-engine-vst3-windows-*`.
2. Unzip and install the `.vst3` bundle (or copy into your VST3 folder).
3. Rescan in FL Studio / REAPER. See [docs/CHORD_ENGINE_FL_STUDIO_QUICKSTART.md](../docs/CHORD_ENGINE_FL_STUDIO_QUICKSTART.md).

CI also runs **pluginval** (strictness **5**) and `ChordEngine_MusicTheoryTests` on Windows and Linux.

You can trigger manually: **Actions** → **DiskLordz Audio (Windows VST3)** → **Run workflow**.

## Linux (CI smoke)

```bash
cmake -B DiskLordz-Audio/Build/Linux -S DiskLordz-Audio \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build DiskLordz-Audio/Build/Linux -j --target ChordEngine_VST3 ChordEngine_MusicTheoryTests
ctest --test-dir DiskLordz-Audio/Build/Linux -R ChordEngine --output-on-failure
xvfb-run -a python3 scripts/vst/run_pluginval.py \
  --plugin "DiskLordz-Audio/Build/Linux/Products/ChordEngine/ChordEngine_artefacts/Release/VST3/CHORD ENGINE.vst3" \
  --strictness-level 5
```
