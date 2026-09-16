# Plugin formats (VST3 / CLAP / AU)

## Shipped today

| Format | Target | Linux artefact (Release) |
|--------|--------|---------------------------|
| VST3 | `JDUpgraded_VST3` | `build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3` |
| CLAP | `JDUpgraded_CLAP` | `build/JDUpgraded_artefacts/Release/CLAP/JD Upgraded.clap` |
| Standalone | `JDUpgraded_Standalone` | `build/JDUpgraded_artefacts/Release/Standalone/JD Upgraded` |

CI builds all three plugin formats on Ubuntu 24.04.

## JUCE and CLAP wiring

- **JUCE** is pinned in root `CMakeLists.txt` (currently **8.0.15** via `FetchContent`).
- **CLAP** is not a native `juce_add_plugin` format yet; this project uses [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions) (`FetchContent` + `GIT_SUBMODULES_RECURSE`) and `clap_juce_extensions_plugin()` after `juce_add_plugin`.
- **CLAP ID:** `com.instruments.jdupgraded` (matches `BUNDLE_ID`).
- The extensions repo is pinned to a specific commit (newer than tag **0.26.0**) because the tagged wrapper does not compile against JUCE 8’s headless parameter layout.

```cmake
clap_juce_extensions_plugin(TARGET JDUpgraded
    CLAP_ID "com.instruments.jdupgraded"
    CLAP_FEATURES instrument synthesizer stereo)
```

Host install paths (when `COPY_PLUGIN_AFTER_BUILD` is enabled): `~/.clap` on Linux, `~/Library/Audio/Plug-Ins/CLAP` on macOS.

## AU (planned)

**AU** is macOS-only and requires Xcode toolchains; keep VST3 + CLAP + Standalone on Linux CI.

Track remaining format work in [PHASE5.md](PHASE5.md).
