# Windows prebuild downloads (CI)

GitHub Actions builds **unsigned** VST3 + Standalone binaries for the three JUCE factory instruments on `windows-latest`. No GitHub Release is required — download **Artifacts** from a green workflow run.

## Get the zip

1. Open the repo on GitHub → **Actions** → **Instruments Windows prebuild**.
2. Pick a **successful** run on your branch (usually `main`).
3. Under **Artifacts**, download one of:
   - **`Instruments-Windows-x64`** — all three products in one zip (recommended).
   - **`Instruments-Windows-x64-split`** — three smaller zips (JD Upgraded, WAVE-9090, SP-1200 Drumulator).

Artifacts expire after about **90 days**.

## Install

Extract the zip. Each product folder contains `VST3/` and `Standalone/`.

| Product | VST3 folder to copy | Standalone |
|---------|---------------------|------------|
| JD Upgraded | `JD Upgraded.vst3` | `JD Upgraded.exe` |
| WAVE-9090 | `WAVE-9090.vst3` | `WAVE-9090.exe` |
| SP-1200 Drumulator | `SP-1200 Drumulator.vst3` | `SP-1200 Drumulator.exe` |

Copy each `.vst3` **folder** to:

`C:\Program Files\Common Files\VST3\`

Then rescan plugins in your DAW. Run the `.exe` files directly for standalone testing (configure audio/MIDI in **Options** if needed).

**SP-1200:** pads are silent until you import WAV samples or load a `.sp12p` project.

## Trigger a build manually

Actions → **Instruments Windows prebuild** → **Run workflow**.

## Local build (same targets)

```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DINSTRUMENTS_ENABLE_CLAP=OFF
cmake --build build -j --target JDUpgraded_VST3 JDUpgraded_Standalone Wave9090_VST3 Wave9090_Standalone SP1200_VST3 SP1200_Standalone
```

CLAP for JD Upgraded remains a Linux CI focus; Windows prebuild skips CLAP (`INSTRUMENTS_ENABLE_CLAP=OFF`) for faster, reliable MSVC builds.

## Workflow source

[`.github/workflows/instruments-windows-prebuild.yml`](../.github/workflows/instruments-windows-prebuild.yml)
