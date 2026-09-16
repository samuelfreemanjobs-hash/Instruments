# Installer and distribution policy

Retail and public releases of **JD Upgraded** must follow this policy. Development workflows may differ when explicitly documented (e.g. [USER_ROM.md](USER_ROM.md)).

## ROM content

| Rule | Detail |
|------|--------|
| Ship only clean-room ROM | Installers and release artifacts include **`jdupg_cleanroom.rom`** embedded at build time (`GenerateCleanroomRom`). |
| No Roland dumps | Do not bundle, link, or download Roland JD wave ROMs, user SysEx wave dumps, or third-party copyrighted PCM in the installer. |
| No user-ROM loader in retail | The `JDUPGRADED_ROM_PATH` override is for **development and QA** only; disable or omit from retail builds if product policy requires a fixed ROM. |

## Presets and SysEx

- Factory programs are procedural and/or curated in-repo (`FactoryPatchLibrary.cpp`).
- Users may import **their own** JD patch SysEx locally; the plugin does not transmit patch data off-device.
- Exported `.jdpreset` files contain APVTS state only (no ROM samples).

## Legal summary

You are responsible for rights to any hardware dumps or SysEx you import. The project maintainers distribute only clean-room synthesis assets documented in [ROM.md](ROM.md).
