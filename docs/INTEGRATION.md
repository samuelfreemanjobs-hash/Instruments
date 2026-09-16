# Plugin formats (VST3 / CLAP / AU)

## Shipped today

| Format | Target | Platforms |
|--------|--------|-----------|
| VST3 | `JDUpgraded_VST3` | Linux, macOS, Windows (CI: Linux) |
| Standalone | `JDUpgraded_Standalone` | Same |

## CLAP / AU (planned)

JUCE **8.0.6** (pinned in `CMakeLists.txt`) does not expose a `CLAP` artefact target in this project’s CMake graph. When upgrading JUCE or wiring [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions), add:

```cmake
# Example future hook (not active):
# option(JDUPGRADED_ENABLE_CLAP "Build CLAP plugin" OFF)
```

**AU** is macOS-only and requires Xcode toolchains; keep VST3 + Standalone on Linux CI.

Track format work in [PHASE5.md](PHASE5.md).
