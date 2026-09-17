# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Character FM** (Track E, Milestone 1) | [products/CharacterFM/ARCHITECTURE.md](products/CharacterFM/ARCHITECTURE.md) | `CharacterFM_VST3`, `CharacterFM_Standalone`, `CharacterFMSmoke` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **HISE sketch lane** (rompler / sampler R&D) | [docs/HISE_ANTIGRAVITY_LANE.md](docs/HISE_ANTIGRAVITY_LANE.md) · [hise-sketch/ARCHITECTURE.md](hise-sketch/ARCHITECTURE.md) | HISE local export (Antigravity); not in root CMake |
| **Character FM** (planned Track E) | [docs/CHARACTER_FM_PRODUCT_PLAN.md](docs/CHARACTER_FM_PRODUCT_PLAN.md) · [docs/CHARACTER_FM_ENGINEERING_SPEC.md](docs/CHARACTER_FM_ENGINEERING_SPEC.md) | Not in CMake yet — Milestone 1 spec |
| **DiskLordz factory OS** | [disklordz/AGENTS.md](disklordz/AGENTS.md) · [disklordz/docs/MASTER_PLAN.md](disklordz/docs/MASTER_PLAN.md) | Agents, Airtable, compiler (partial on branch) |

## Repository layout

```
Source/          Plugin processor, DSP, assets, preset import
hise-sketch/     HISE / Antigravity projects (optional imports)
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
