# User-supplied ROM (development only)

JD Upgraded ships an **embedded clean-room** `JDUPGROM` file. Installers must not bundle Roland or third-party JD wave dumps.

## Override path

Set at launch:

```bash
export JDUPGRADED_ROM_PATH=/path/to/your.jdupg.rom
./build/JDUpgraded_artefacts/Release/Standalone/JD\ Upgraded
```

Only files that pass `RomBank` validation are accepted:

- Magic `JDUPGROM`
- Supported version and CRC over PCM payload
- Invalid files are **rejected**; the embedded factory ROM remains active

## Legal

You are responsible for rights to any PCM you load. This project does not distribute or import copyrighted Roland ROM data. Use the built-in generator (`tools/GenerateCleanroomRom.cpp`) for shareable assets.

## UI

The plugin editor shows the active ROM source (`Embedded clean-room ROM` vs `User JDUPGROM: …`).
