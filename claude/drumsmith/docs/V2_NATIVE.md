# DRUMSMITH v2.0 native developer guide

## Delivered and boundaries
- **C++20 native synthesis**: eight drum voices, deterministic seeded noise, 32-voice realtime engine, sample-accurate MIDI events in JUCE wrapper, MIDI pitch for 808/cowbell, guarded floats, no dynamic voice allocation in process loop.
- **Native command-line WAV renderer**: dependency-free CMake target and CLI; no overwrite.
- **JUCE 8 app/plugin code**: shared source targets for VST3 and Standalone, controls, parameter state persistence. *Not compiled or DAW-scanned in this environment because JUCE SDK unavailable.*
- **Legacy Python**: v1.3 genre factory, browser, CALIPER, and technical QA unchanged; the native C++ renderer is an additional separate path.
- **Known missing features**: native waveform editor, user-customizable multistage envelopes, preset morph, engine GUI batch-export, JUCE kit browser, true-peak limiter, verified device exports, listening tests, DAW validation, managed parallel render jobs. No claim of feature parity or commercially mastered sound.

## Build portable renderer and tests
```
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release
cmake --build native/build --config Release
ctest --test-dir native/build --output-on-failure
./native/build/drumsmith-render output.wav 0 52 1234
```
The last line assumes a Unix-like shell; on Windows use `native\\build\\Release\\drumsmith-render.exe` for multi-config generators (or `native\\build\\drumsmith-render.exe` for single-config).
Kind IDs: 0=808, 1=kick, 2=snare, 3=clap, 4=hat, 5=cowbell, 6=perc, 7=fx.

## Build Windows VST3 and standalone (JUCE 8 required)
1. Install Visual Studio 2022 with Desktop development with C++ and CMake. Obtain JUCE 8 from its official repository and review its licensing for commercial use.
2. Either clone JUCE into `native/JUCE` or pass `-DJUCE_DIR=<JUCE install>/lib/cmake/JUCE-8.x` pointing to installed JUCE CMake config.
3. In PowerShell:
```
cmake -S native -B native/build-juce -DDRUMSMITH_BUILD_JUCE=ON -DCMAKE_BUILD_TYPE=Release
cmake --build native/build-juce --config Release
ctest --test-dir native/build-juce --output-on-failure -C Release
```
4. Locate resulting `DrumsmithPlugin_artefacts/Release/VST3/` and `Standalone/` outputs; use a DAW's plugin scanner and a hardware-backed audio interface to test.
5. Verify with MIDI note-on/off, automation, state reload, 44.1/48/96 kHz, offline bounce, multiple instances, low block sizes, and no xrun/allocations in callback before shipping.

## Release policy
Native outputs are exploratory, not validated commercial masters. Technical QA isn't listening approval. Hardware support claims need device tests. Keep provenance manifests and CALIPER read-only audit workflow.

## Architecture
```
Claude Code DRUMSMITH > Python factory + CALIPER -> JSON kit + technical QA
                        native/C++ core -> native WAV renderer
                                        -> JUCE AudioProcessor -> VST3/standalone UI
```
The C++ core is a new independent synthesizer; exported FORGE `parametric-v2` contracts are **not directly loaded** into the C++ plugin. Integration and equivalence testing remain future work.

## Tested native end-to-end kit pipeline
```
python scripts/native_factory.py --out MY_NEW_NATIVE_KIT --renderer native/build/drumsmith-render --genre drift-phonk --count 64 --plan tail-light-static
python scripts/kit_browser.py --kit MY_NEW_NATIVE_KIT
```
`native_factory.py` requires a new destination, calls the native C++ renderer for every manifest recipe, captures per-file SHA-256, and runs the same technical verifier and diversity report as the Python factory. It does not grant release authorization or guarantee a high-quality sound. The native core currently uses seeded xorshift noise, so do not assert sample-for-sample matching against the Python reference renderer.
