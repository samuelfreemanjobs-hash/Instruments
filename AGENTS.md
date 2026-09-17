# Cloud agents — Disklordz / Instruments monorepo

## Before changing code

1. Read [ARCHITECTURE.md](ARCHITECTURE.md).
2. Read [docs/JUCE_FACTORY.md](docs/JUCE_FACTORY.md) for JUCE track vs HISE lane and ecosystem do-not-use list.
3. Open the product doc for your area ([docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md), [hise-sketch/ARCHITECTURE.md](hise-sketch/ARCHITECTURE.md)).

## Build and test (Linux)

Environment install matches [.cursor/environment.json](.cursor/environment.json):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

Validate like CI:

```bash
./build/OfflineRender /tmp/a.wav 0 60 100 0.5 44100
./build/OfflineRender /tmp/b.wav 0 60 100 0.5 44100
./tests/golden/verify_golden.sh
```

Built plugin artefacts: `build/JDUpgraded_artefacts/Release/` (VST3, CLAP, Standalone).

## New JUCE plugin work

Use [MyFirstPlugin/NEW_PLUGIN.md](MyFirstPlugin/NEW_PLUGIN.md) and [MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md](MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md). Do not scaffold from external templates (TAP, pamplejuce clone, react-juce) unless a work order says otherwise.

## HISE lane

Only when the task is explicitly HISE sketch work: [docs/HISE_ANTIGRAVITY_LANE.md](docs/HISE_ANTIGRAVITY_LANE.md). Do not modify root JUCE factory for sketch-only requests.

## JUCE examples (DemoRunner)

Optional; requires `libgtk-3-dev` and `libwebkit2gtk-4.1-dev`. See **SOP C** in [docs/JUCE_FACTORY.md](docs/JUCE_FACTORY.md).
