# Build — DiskLordz Audio (CHORD ENGINE shell)

## Windows (primary)

```powershell
cd C:\dev\Instruments\DiskLordz-Audio
cmake -B Build/Windows -G "Visual Studio 17 2022" -A x64
cmake --build Build/Windows --config Release --target ChordEngine_VST3
```

VST3 artifact (typical):

`Build\Windows\ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3`

## Linux (CI smoke)

```bash
cmake -B DiskLordz-Audio/Build/Linux -S DiskLordz-Audio
cmake --build DiskLordz-Audio/Build/Linux --config Release --target ChordEngine_VST3
```
