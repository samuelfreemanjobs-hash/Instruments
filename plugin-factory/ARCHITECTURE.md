# VST Plugin Factory OS

JUCE + C++ **multi-plugin build system** for shipping VST3 (and optional Standalone) targets from one CMake tree. Use it to scaffold new effects or instruments, register them in a single manifest, and build everything with one command.

## Purpose

- **Who:** Instrument developers and Cursor agents adding new VST products without copying CMake boilerplate each time.
- **What:** Registry-driven factory (`plugins/registry.txt`), shared `pf_add_juce_plugin` CMake helper, CLI scaffolder, and two reference plugins (effect + synth).

## Build & run

**Zero-touch product path** (scaffold optional → build → artefact check → pluginval → ship):

```bash
cd plugin-factory
./scripts/factory.sh release
./scripts/factory.sh release --new effect my-drive   # create + full pipeline
```

Ship target defaults to `~/.vst3` (Linux), `~/Library/Audio/Plug-Ins/VST3` (macOS), or `%COMMONPROGRAMFILES%\VST3` (Windows). Override with `FACTORY_VST3_INSTALL_DIR`.

**Monorepo full QA + factory + ship** (JD Upgraded / Wave909 CI parity plus factory):

```bash
python3 vst-testing-ops/run_business.py --profile release
```

Factory-only QA (no root CMake build):

```bash
python3 vst-testing-ops/run_business.py --profile factory
```

Manual steps:

```bash
cd plugin-factory
./scripts/factory.sh build
./scripts/factory.sh verify
./scripts/factory.sh validate
./scripts/factory.sh ship
```

VST3 build outputs: `build/plugins/<registry-dir>/*_artefacts/Release/VST3/*.vst3`

Validate from repo root:

```bash
python3 scripts/vst/run_pluginval.py --factory-artefacts
python3 plugin-factory/scripts/run_pluginval_factory.py
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
| `scripts/factory.sh` | `build`, `verify`, `validate`, `ship`, `release`, `new` |
| `scripts/factory_ops.py` | Discover VST3 artefacts, verify registry, ship to disk |
| `scripts/run_pluginval_factory.py` | pluginval all registered factory bundles |
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
