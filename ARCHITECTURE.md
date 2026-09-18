# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **HISE sketch lane** (rompler / sampler R&D) | [docs/HISE_ANTIGRAVITY_LANE.md](docs/HISE_ANTIGRAVITY_LANE.md) · [hise-sketch/ARCHITECTURE.md](hise-sketch/ARCHITECTURE.md) | HISE local export (Antigravity); not in root CMake |
| **Disklordz Drum SaaS** (web) | [disklordz/website/ARCHITECTURE.md](disklordz/website/ARCHITECTURE.md) · [docs/DISKLORDZ_SAAS_V0.md](docs/DISKLORDZ_SAAS_V0.md) · [docs/DISKLORDZ_GO_LIVE.md](docs/DISKLORDZ_GO_LIVE.md) | `npm run build` in `disklordz/website/` |
| **Disklordz DAW inbox** (WO-016) | [disklordz/daw-inbox/ARCHITECTURE.md](disklordz/daw-inbox/ARCHITECTURE.md) | `npm start` in `disklordz/daw-inbox/` |
| **Disklordz RAG** (prompt knowledge) | [disklordz/rag/ARCHITECTURE.md](disklordz/rag/ARCHITECTURE.md) · [docs/RAG_AND_INTELLIGENT_AUTOMATION.md](docs/RAG_AND_INTELLIGENT_AUTOMATION.md) | `python3 disklordz/rag/scripts/chunk_corpus.py` |
| **Antigravity ↔ Cursor bridge** | [disklordz/antigravity/ARCHITECTURE.md](disklordz/antigravity/ARCHITECTURE.md) | `./scripts/antigravity-bridge/antigravity-bridge.sh` |
| **WAVE-909** (sampleless trap wavetable synth) | [Wave909/ARCHITECTURE.md](Wave909/ARCHITECTURE.md) | `Wave909_VST3`, `Wave909_CLAP`, `Wave909_Standalone`, `Wave909Tests` |
| **SP-1200** (VST drum sampler — Claude lane) | [SP1200/ARCHITECTURE.md](SP1200/ARCHITECTURE.md) | TBD — scaffold |
| **Pluggnb Vital preset pack** (first preset SKU) | [disklordz/packs/vital/ARCHITECTURE.md](disklordz/packs/vital/ARCHITECTURE.md) | Vital `.vital` / `.vitalbank` |
| **Junova-X** (Juno-class poly synth — **not** NovaDrum / PWA) | [Junova-X/REPO_HANDOFF.md](Junova-X/REPO_HANDOFF.md) · [docs/TR808_JUNOVA_PRODUCT_MATRIX.md](docs/TR808_JUNOVA_PRODUCT_MATRIX.md) | `JunovaX_VST3`, `JunovaX_CLAP` (planned; branch `cursor/junova-x-handoff-029a`) |
| **NovaDrum** (TR-808 class **plugin**, JUCE) | [vst-tr808/REPO_HANDOFF.md](vst-tr808/REPO_HANDOFF.md) | JUCE VST3 + CLAP (branch `cursor/novadrum-juce-handoff-029a`) |
| **TR-808 PWA** (browser — track **P**, not SaaS) | [disklordz/tr808-pwa/ARCHITECTURE.md](disklordz/tr808-pwa/ARCHITECTURE.md) | Static PWA (branch `cursor/tr808-pwa-app-94ae`) |
| **Business agents** (SKU research, content, ship eval) | [docs/business-agents/ARCHITECTURE.md](docs/business-agents/ARCHITECTURE.md) | `python3 scripts/business-agents/business_agent.py` |

## Repository layout

```
Source/          Plugin processor, DSP, assets, preset import
Junova-X/        Junova-X synth (JUCE); vst-juno106/ legacy import on same branch
vst-tr808/       NovaDrum JUCE plugin handoff (not PWA)
hise-sketch/     HISE / Antigravity projects (optional imports)
disklordz/       Drum SaaS (`website/`), TR-808 PWA (`tr808-pwa/`), sound-factory
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

**Cursor / agents:** [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) · [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) · [docs/PLUGIN_FACTORY_OS.md](docs/PLUGIN_FACTORY_OS.md) (JUCE plugins) · **[CLAUDE.md](CLAUDE.md)** / [docs/CLAUDE_CODE_INTEGRATION.md](docs/CLAUDE_CODE_INTEGRATION.md) · SaaS roadmap [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) · optional desktop agent [docs/BYTEBOT_SETUP.md](docs/BYTEBOT_SETUP.md).

## Documentation policy

Every product must maintain an `ARCHITECTURE.md` — see [.cursor/rules/architecture-documentation.mdc](.cursor/rules/architecture-documentation.mdc).
