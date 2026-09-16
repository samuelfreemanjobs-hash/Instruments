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
./tools/new-vst.sh MySynth
```

Creates `vst/MySynth/` with processor, editor, and CMake wired for VST3 + Standalone.

### 3. Build

```bash
cd vst/MySynth
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

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
