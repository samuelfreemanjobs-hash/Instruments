# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **HISE sketch lane** (rompler / sampler R&D) | [docs/HISE_ANTIGRAVITY_LANE.md](docs/HISE_ANTIGRAVITY_LANE.md) · [hise-sketch/ARCHITECTURE.md](hise-sketch/ARCHITECTURE.md) | HISE local export (Antigravity); not in root CMake |
| **Disklordz Drum SaaS** (web) | [disklordz/website/ARCHITECTURE.md](disklordz/website/ARCHITECTURE.md) · [docs/DISKLORDZ_SAAS_V0.md](docs/DISKLORDZ_SAAS_V0.md) | `npm run build` in `disklordz/website/` |
| **Disklordz VST launch funnel** (web) | [docs/DISKLORDZ_MASTER_LAUNCH_CANVAS.md](docs/DISKLORDZ_MASTER_LAUNCH_CANVAS.md) · `/launch` in website | Same Next.js app as SaaS |
| **Disklordz RAG** (prompt knowledge) | [disklordz/rag/ARCHITECTURE.md](disklordz/rag/ARCHITECTURE.md) · [docs/RAG_AND_INTELLIGENT_AUTOMATION.md](docs/RAG_AND_INTELLIGENT_AUTOMATION.md) | `python3 disklordz/rag/scripts/chunk_corpus.py` |
| **Disklordz Marketing Dept (VARA)** | [docs/DISKLORDZ_MARKETING_DIRECTOR.md](docs/DISKLORDZ_MARKETING_DIRECTOR.md) · [disklordz/marketing/ARCHITECTURE.md](disklordz/marketing/ARCHITECTURE.md) | Slack `#disklordz-marketing-dept` |
| **Product Launch Manager (HELM)** | [docs/DISKLORDZ_PRODUCT_LAUNCH_MANAGER.md](docs/DISKLORDZ_PRODUCT_LAUNCH_MANAGER.md) · [disklordz/marketing/plm/](disklordz/marketing/plm/) | Pairs with PM + VARA; [HUMAN_TRIGGERS.md](disklordz/marketing/HUMAN_TRIGGERS.md) |
| **A&R Dept (CROW + artist agents)** | [docs/DISKLORDZ_AR_DEPARTMENT.md](docs/DISKLORDZ_AR_DEPARTMENT.md) · [disklordz/ar/ARCHITECTURE.md](disklordz/ar/ARCHITECTURE.md) | Factory QC before launch SKUs |
| **Chief of Staff (SAGE) + Slack EA** | [docs/DISKLORDZ_CHIEF_OF_STAFF.md](docs/DISKLORDZ_CHIEF_OF_STAFF.md) · `#disklordz-exec` | `/api/exec/digest` |
| **Antigravity ↔ Cursor bridge** | [disklordz/antigravity/ARCHITECTURE.md](disklordz/antigravity/ARCHITECTURE.md) | `./scripts/antigravity-bridge/antigravity-bridge.sh` |

## Repository layout

```
Source/          Plugin processor, DSP, assets, preset import
hise-sketch/     HISE / Antigravity projects (optional imports)
disklordz/       Drum SaaS (`website/`) and sound-factory scripts
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

Optional Slack: CI ([`ci-slack-notify.yml`](.github/workflows/ci-slack-notify.yml)), Antigravity inbox ([`antigravity-inbox-slack.yml`](.github/workflows/antigravity-inbox-slack.yml)), Airtable handoff ([`airtable-antigravity-handoff.yml`](.github/workflows/airtable-antigravity-handoff.yml)) — configure with [`scripts/setup-disklordz-integrations.sh`](scripts/setup-disklordz-integrations.sh).

**Cursor / agents:** [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) · [docs/CURSOR_IMPLEMENTATION_LEAD.md](docs/CURSOR_IMPLEMENTATION_LEAD.md) (what the coding agent is/isn't) · [docs/DISKLORDZ_INTEGRATION_AUDIT.md](docs/DISKLORDZ_INTEGRATION_AUDIT.md) (loose ends) · [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) · SaaS roadmap [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) · optional desktop agent [docs/BYTEBOT_SETUP.md](docs/BYTEBOT_SETUP.md).

**Disklordz package index:** [disklordz/ARCHITECTURE.md](disklordz/ARCHITECTURE.md)

## Documentation policy

Every product must maintain an `ARCHITECTURE.md` — see [.cursor/rules/architecture-documentation.mdc](.cursor/rules/architecture-documentation.mdc).
