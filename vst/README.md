# VST / VSTi boilerplate

**Every new plugin in this repo starts from `vst/template/`.**

Cursor enforces this in `.cursor/rules/06-vst-vsti-template.mdc`.

## Quick start

### 1. JUCE (one time per clone)

```bash
cd ~/code/Instruments   # your repo root
git submodule update --init vst/JUCE
```

If submodule is missing, add it once:

```bash
git submodule add https://github.com/juce-framework/JUCE.git vst/JUCE
```

### 2. New plugin from template

```bash
./tools/new-vst.sh MySynth                  # VSTi (template)
./tools/new-vst.sh VinylNoise --type effect # VST FX (template-effect)
```

### 3. Build

```bash
./tools/build-vst.sh MySynth
./tools/build-vst.sh --list
./tools/build-vst.sh --all
```

Shared DSP: `shared/dsp/` (biquad, ADSR, smoothing, phasor).

Artifacts (paths vary by OS): `build/MySynth_artefacts/Release/VST3/`

## Layout

| Path | Purpose |
|------|---------|
| `vst/template/` | **Do not build shipping products here** — copy only |
| `vst/<PluginName>/` | Your actual plugins |
| `vst/JUCE/` | JUCE submodule (not committed until you `submodule add`) |

## logue SDK vs VST

| | `src/oscillators/` | `vst/<PluginName>/` |
|--|-------------------|---------------------|
| Target | NTS-1 / XD / Prologue / mkII | VST3 / Standalone (DAW) |
| API | logue SDK | JUCE |
| DSP style | Often fixed-point / q31 | Float `processBlock` |

Port algorithms between them deliberately; do not include `userosc.h` in VST builds.
