# DRUMSMITH AI v2.0 — Native Drum Synthesis Foundation

The v2.0 distribution retains **all existing v1.3 Python factory commands, Claude Code agents, CALIPER QA, browser and reference-kit examples** and adds `native/` with a JUCE 8 plugin/standalone **source project**, a portable C++20 DSP renderer, automated native tests, and an eight-kind synthesis engine.

## Quick start
```
python scripts/self_test.py
python scripts/test_upgrade.py
python scripts/test_v13.py
python scripts/test_v20.py
cmake -S native -B native/build
cmake --build native/build
ctest --test-dir native/build --output-on-failure
```
Then use `native/build/drumsmith-render example.wav 0 52 1234` for a C++-generated 808 reference. See `docs/V2_NATIVE.md` for Windows JUCE instructions and acceptance gates.

## Build JUCE VST3 + standalone
```
cmake -S native -B native/build-juce -DDRUMSMITH_BUILD_JUCE=ON
cmake --build native/build-juce --config Release
```
Requires locally installed JUCE 8 (or `native/JUCE`) and compatible Windows/macOS compiler. **Only the portable C++ engine was built/tested in the supplied environment; not a compiled VST3/standalone.**

## End-to-end v1.3 kit factory still works
```
python scripts/factory.py --out MY_NEW_KIT --genre drift-phonk --sku DKL --count 64
python scripts/kit_browser.py --kit MY_NEW_KIT
```

## Safety and release
No sampler or Keygroup engine in native synthesizer. WAV files are technical reference samples, not mastered products. The QA report doesn't waive listening/provenance/hardware gates. JUCE licensing and VST3 distribution requirements must be checked before sales.

## Native 64-sound batch render (tested)
After compiling the portable C++ target, run:

```powershell
python scripts/native_factory.py --out Native_TLS_64 --renderer native/build/drumsmith-render --genre drift-phonk --count 64 --plan tail-light-static
```

On a Visual Studio multi-configuration build, substitute `native/build/Release/drumsmith-render.exe` for the renderer. This adapter preflights v1.3 recipe manifests, passes the full recipe parameter set to the native CLI, hashes the rendered WAVs, then runs the unchanged technical QA and diversity audit. The native C++ noise generation and synthesis algorithms are not byte-identical to Python; hash and sonic equivalence are **not** asserted. A full 64-hit native draft passed both technical QA and the duplicate audit in the Linux C++ build environment.
