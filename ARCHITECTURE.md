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
| **Artist greenlights (HQ)** | [docs/ARTIST_GREENLIGHTS.md](docs/ARTIST_GREENLIGHTS.md) | DL001, DL002, DL006, DL004 — bootstrapped research + A&R |
| **OpenClaw 2.0 workspace** | [docs/OPENCLAW_VINTAGE_COLLECTIVE.md](docs/OPENCLAW_VINTAGE_COLLECTIVE.md) | SOUL/AGENTS hierarchy + Workboard dispatch |
| **OpenClaw gateway routing** | [docs/OPENCLAW_GATEWAY_ROUTING.md](docs/OPENCLAW_GATEWAY_ROUTING.md) | Model tiering, fork spawn, context isolation |
| **OpenClaw ensemble stack** | [docs/OPENCLAW_ENSEMBLE_STACK.md](docs/OPENCLAW_ENSEMBLE_STACK.md) | Gateway / SQLite / SOUL / MCP / fork layers |
| **Factory music MCP** | [docs/FACTORY_MCP.md](docs/FACTORY_MCP.md) | BPM, DL-BRF, MIDI shells for sectional agents |
| **OpenClaw migration** | [docs/OPENCLAW_MIGRATION.md](docs/OPENCLAW_MIGRATION.md) | 2.0 SQLite, doctor, parallel spawn stability |
| **Ensemble personas** | [docs/OPENCLAW_PERSONAS.md](docs/OPENCLAW_PERSONAS.md) | Isaac / Marcus / Bernard / Evelyn domain SOUL |
| **Sonic archaeology** | [docs/SONIC_ARCHAEOLOGY.md](docs/SONIC_ARCHAEOLOGY.md) | Screw / Memphis / Smokey → DL006/DL002 source design |

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
