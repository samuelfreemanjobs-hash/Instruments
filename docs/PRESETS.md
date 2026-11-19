# Factory presets (256)

Programs are **four-tone stacks**. Use host program change or the plugin **&lt; / &gt;** buttons.

| Index | Category |
|-------|----------|
| 1–36 | EP / Rhodes / JD-style keys |
| 37–72 | Pads |
| 73–108 | Bass |
| 109–144 | Vaporwave |
| 145–180 | 80s R&B |
| 181–216 | 90s R&B |
| 217–256 | Elite sound design |

## Per-tone mute

- **tone1Mute … tone4Mute** (or **M1–M4** in UI): silences that tone; level sliders are unchanged.
- Useful for auditioning a single layer in a stack.

## State save

DAW projects store APVTS parameters plus **currentProgram** in the plugin state blob.

## `.jdpreset` files

- **Export / Import** buttons in the plugin UI, or headless: `ExportPreset out.jdpreset 42`
- XML `ValueTree` with all APVTS parameters and `currentProgram`
- Not a substitute for Roland SysEx ([SYSEX.md](SYSEX.md))

**Curated factory programs:** indices **0**, **24**, **48**, **64**, **80**, and **112** use hand-tuned layers (names suffixed `(curated)`).

## External ROM (development)

Set `JDUPGRADED_ROM_PATH` to a `jdupg_cleanroom.rom` file (JDUPGROM v2) to replace the embedded bank at startup.
