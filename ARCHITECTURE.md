# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **Disklordz Drum SaaS** (v0 web; factory TBD) | [docs/DISKLORDZ_SAAS_V0.md](docs/DISKLORDZ_SAAS_V0.md) · [disklordz/company/OPERATING_RHYTHM.md](disklordz/company/OPERATING_RHYTHM.md) | — (web under `disklordz/website/` planned) |

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

Airtable PM: [`airtable-work-order-to-github.yml`](.github/workflows/airtable-work-order-to-github.yml) (A1) · [`airtable-pr-sync.yml`](.github/workflows/airtable-pr-sync.yml) (A2). Agent guide: [`AGENTS.md`](AGENTS.md).

## Documentation policy

Every product must maintain an `ARCHITECTURE.md` — see [.cursor/rules/architecture-documentation.mdc](.cursor/rules/architecture-documentation.mdc).
