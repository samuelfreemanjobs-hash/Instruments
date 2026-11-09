# VST / VSTi boilerplate

**Every new plugin in this repo starts from `vst/template/`.**

Cursor enforces this in `.cursor/rules/06-vst-vsti-template.mdc`.

## JUCE version policy

The monorepo pins **one** JUCE tag in the repo-root file [`/JUCE_VERSION`](../JUCE_VERSION) (currently **8.0.15**). Root CMake, `MyFirstPlugin`, `DiskLordz-Audio`, `vst/template*`, `tools/build-vst.sh`, and CI all read that file — do not hard-code a tag in CMake or workflows. To bump: edit `/JUCE_VERSION`, open a PR that builds every JUCE target on CI, and re-baseline golden WAV / visual render tests in the **same** PR if output changes. `tools/build-vst.sh` clones that tag into `vst/JUCE` when the tree is missing (there is no `vst/JUCE` git submodule).

## Quick start

### 1. JUCE (one time per clone, or automatic)

```bash
# Preferred: let the build script clone the pin
./tools/build-vst.sh MeterTap

# Or clone manually using the pin:
TAG="$(tr -d '[:space:]' < JUCE_VERSION)"
git clone --depth 1 --branch "$TAG" https://github.com/juce-framework/JUCE.git vst/JUCE
```

Templates also FetchContent the `/JUCE_VERSION` tag when `vst/JUCE` is absent.

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
| `vst/JUCE/` | Local JUCE checkout at the `/JUCE_VERSION` tag (gitignored / not a submodule) |

## logue SDK vs VST

| | `src/oscillators/` | `vst/<PluginName>/` |
|--|-------------------|---------------------|
| Target | NTS-1 / XD / Prologue / mkII | VST3 / Standalone (DAW) |
| API | logue SDK | JUCE |
| DSP style | Often fixed-point / q31 | Float `processBlock` |

Port algorithms between them deliberately; do not include `userosc.h` in VST builds.
