# VST Plugin Factory OS

JUCE + C++ **multi-plugin build system** for shipping VST3 (and optional Standalone) targets from one CMake tree. Use it to scaffold new effects or instruments, register them in a single manifest, and build everything with one command.

## Purpose

- **Who:** Instrument developers and Cursor agents adding new VST products without copying CMake boilerplate each time.
- **What:** Registry-driven factory (`plugins/registry.txt`), shared `pf_add_juce_plugin` CMake helper, CLI scaffolder, and two reference plugins (effect + synth).

## Build & run

```bash
cd plugin-factory
./scripts/factory.sh build
# or
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

VST3 outputs: `build/<Target>_artefacts/Release/VST3/*.vst3`

Validate (from repo root, after build):

```bash
python3 scripts/vst/run_pluginval.py --discover plugin-factory/build/ReferenceEffect_artefacts/Release/VST3
```

## Data flow

```
plugins/registry.txt  →  root CMakeLists.txt  →  add_subdirectory(plugins/<name>)
                              ↓
                    FetchContent(JUCE 8.0.15)
                              ↓
              pf_add_juce_plugin() per plugin CMakeLists.txt
                              ↓
                    VST3 + Standalone artefacts
```

Scaffolding:

```
factory.sh new effect|synth <dir>  →  templates/* copied to plugins/<dir>
                                   →  line appended to registry.txt
```

## Threading / realtime

Factory reference code follows the same rules as production plugins:

- No allocations, locks, or string work in `processBlock`.
- Cache `getRawParameterValue` in the processor constructor.
- Smooth automatable parameters with `juce::LinearSmoothedValue` (see `plugins/reference-effect`).

## Key modules

| Path | Responsibility |
|------|----------------|
| `cmake/InstrumentsPluginFactory.cmake` | `pf_add_juce_plugin`, registry loader |
| `plugins/registry.txt` | Enabled plugin directories |
| `plugins/reference-effect` | Demo stereo effect (SVF + drive + LFO) |
| `plugins/reference-synth` | Demo poly synth (JUCE `Synthesiser`) |
| `templates/effect`, `templates/synth` | Scaffolding sources for `factory.sh new` |
| `scripts/factory.sh` | `build`, `list`, `new` |
| `shared/RealTimeSafety.h` | Optional RT helpers |

## Extension points

1. **New plugin:** `./scripts/factory.sh new effect my-plugin` then edit `plugins/my-plugin/`.
2. **Manual plugin:** Add a folder under `plugins/` with `CMakeLists.txt` calling `pf_add_juce_plugin`, then append the folder name to `registry.txt`.
3. **Formats:** Pass `FORMATS VST3 CLAP Standalone` to `pf_add_juce_plugin` when CLAP extensions are wired at the monorepo root.
4. **Copy after build:** Set `COPY_AFTER_BUILD ON` in the plugin’s `pf_add_juce_plugin` call (off by default in CI).

## Related docs

- Legacy Windows-focused template: [../MyFirstPlugin/NEW_PLUGIN.md](../MyFirstPlugin/NEW_PLUGIN.md)
- Monorepo index: [../ARCHITECTURE.md](../ARCHITECTURE.md)
- Pluginval CI harness: [../vst-testing-ops/ARCHITECTURE.md](../vst-testing-ops/ARCHITECTURE.md)
