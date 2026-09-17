# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **DiskLordz Factory** (batch catalog / agent OS scaffold) | [disklordz-factory/ARCHITECTURE.md](disklordz-factory/ARCHITECTURE.md) | Python `factory-api` (FastAPI), not in CMake |
| **DiskLordz SP-1200 VSTi** (planned; DSP external today) | [docs/DISKLORDZ_SP1200.md](docs/DISKLORDZ_SP1200.md) · [AGENTS.md](AGENTS.md) | JUCE when integrated here; DSP is plain C++17 |
| **YouTube network brands** | [docs/DISKLORDZ_YOUTUBE_NETWORK.md](docs/DISKLORDZ_YOUTUBE_NETWORK.md) | Programming + funnel per channel; research-gated |
| **Vintage collective (4 artists)** | [docs/FACTORY_VINTAGE_COLLECTIVE.md](docs/FACTORY_VINTAGE_COLLECTIVE.md) | Soul/jazz/R&B synthesis → phonk/screw/french/cyber; OpenClaw Workboard |

## Repository layout

```
Source/          Plugin processor, DSP, assets, preset import
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
