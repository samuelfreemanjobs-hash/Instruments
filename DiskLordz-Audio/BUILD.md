# Build — DiskLordz Audio (CHORD ENGINE shell)

## Windows (primary)

From monorepo root (matches dev checkout):

```powershell
cd C:\dev\Instruments
git pull origin cursor/audio-business-copilot-b828
cmake -B DiskLordz-Audio\Build\Windows -S DiskLordz-Audio
cmake --build DiskLordz-Audio\Build\Windows --config Release
```

If CMake does not pick Visual Studio automatically:

```powershell
cmake -B DiskLordz-Audio\Build\Windows -S DiskLordz-Audio -G "Visual Studio 17 2022" -A x64
cmake --build DiskLordz-Audio\Build\Windows --config Release --target ChordEngine_VST3
```

VST3 artifact (typical, from repo root):

`DiskLordz-Audio\Build\Windows\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3`

## Linux (CI smoke)

```bash
cmake -B DiskLordz-Audio/Build/Linux -S DiskLordz-Audio
cmake --build DiskLordz-Audio/Build/Linux --config Release --target ChordEngine_VST3
```
