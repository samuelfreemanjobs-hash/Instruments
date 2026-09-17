# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **Junova-X** (JUCE VST3 + CLAP) | [Junova-X/REPO_HANDOFF.md](Junova-X/REPO_HANDOFF.md) | `JunovaX_VST3`, `JunovaX_CLAP` (planned) |
| **NovaDrum** (TR-808 class) | [vst-tr808/plugin-spec-mvp.md](vst-tr808/plugin-spec-mvp.md) | Spec only — [plugin tracks](docs/DISKLORDZ_PLUGIN_TRACKS.md) |

## Repository layout

```
Source/          JD Upgraded (JUCE) processor, DSP, assets
vst-juno106/     Junova-X (iPlug2) — see REPO_HANDOFF.md
vst-tr808/       NovaDrum / 808-class spec (iPlug2, future)
tools/           CLI binaries (link JUCE / plugin static lib)
docs/            User and agent docs (SYSEX, ROM, phases, handoff)
tests/golden/    manifest.tsv + golden WAVs; verify_golden.sh / refresh_golden.sh
```

## Build (all targets)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

## CI

[`.github/workflows/build.yml`](.github/workflows/build.yml): Release build, determinism (`OfflineRender` ×2), and golden WAV compare via `SpectralDiff`.

Optional Slack notifications: [`.github/workflows/ci-slack-notify.yml`](.github/workflows/ci-slack-notify.yml) (configure with [`scripts/setup-disklordz-integrations.sh`](scripts/setup-disklordz-integrations.sh)).

## Documentation policy

Every product must maintain an `ARCHITECTURE.md` — see [.cursor/rules/architecture-documentation.mdc](.cursor/rules/architecture-documentation.mdc).
