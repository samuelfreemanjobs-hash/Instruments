# Instruments repository — architecture index

This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch.

## Products

| Product | Doc | CMake targets |
|---------|-----|----------------|
| **JD Upgraded** (VST3 + CLAP + standalone synth) | [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [Source/UI/ARCHITECTURE.md](Source/UI/ARCHITECTURE.md) | `JDUpgraded_VST3`, `JDUpgraded_CLAP`, `JDUpgraded_Standalone` |
| **Offline tooling** (ROM gen, render, regression) | [tools/ARCHITECTURE.md](tools/ARCHITECTURE.md) | `GenerateCleanroomRom`, `OfflineRender`, `SpectralDiff` |
| **SynthForge** (hardware preset workstation) | [tools/synth-forge/ARCHITECTURE.md](tools/synth-forge/ARCHITECTURE.md) | `pytest tests -v` · `uvicorn synth_forge.main:app --port 8000` in `tools/synth-forge/` |
| **VST testing ops** (pluginval command center) | [vst-testing-ops/ARCHITECTURE.md](vst-testing-ops/ARCHITECTURE.md) | `python3 vst-testing-ops/run_business.py --profile ci` · `streamlit run vst-testing-ops/app.py` |
| **HISE sketch lane** (rompler / sampler R&D) | [docs/HISE_ANTIGRAVITY_LANE.md](docs/HISE_ANTIGRAVITY_LANE.md) · [hise-sketch/ARCHITECTURE.md](hise-sketch/ARCHITECTURE.md) | HISE local export (Antigravity); not in root CMake |
| **Disklordz** (package index) | [disklordz/ARCHITECTURE.md](disklordz/ARCHITECTURE.md) · [docs/ROADMAP.md](docs/ROADMAP.md) | `./scripts/run-agent-fleet-now.sh` |
| **Disklordz Drum SaaS** (web) | [disklordz/website/ARCHITECTURE.md](disklordz/website/ARCHITECTURE.md) · [docs/DISKLORDZ_SAAS_V0.md](docs/DISKLORDZ_SAAS_V0.md) · [docs/DISKLORDZ_GO_LIVE.md](docs/DISKLORDZ_GO_LIVE.md) | `npm run build` in `disklordz/website/` |
| **Disklordz sound factory** | [disklordz/sound-factory/ARCHITECTURE.md](disklordz/sound-factory/ARCHITECTURE.md) | `python3 disklordz/sound-factory/scripts/generate_kit.py` |
| **Disklordz DAW inbox** (WO-016) | [disklordz/daw-inbox/ARCHITECTURE.md](disklordz/daw-inbox/ARCHITECTURE.md) | `npm start` in `disklordz/daw-inbox/` |
| **Disklordz RAG** (prompt knowledge) | [disklordz/rag/ARCHITECTURE.md](disklordz/rag/ARCHITECTURE.md) · [docs/RAG_AND_INTELLIGENT_AUTOMATION.md](docs/RAG_AND_INTELLIGENT_AUTOMATION.md) | `python3 disklordz/rag/scripts/chunk_corpus.py` |
| **Disklordz integrations** (35 OSS refs) | [disklordz/integrations/ARCHITECTURE.md](disklordz/integrations/ARCHITECTURE.md) · [docs/DISKLORDZ_OPEN_SOURCE_REFERENCES.md](docs/DISKLORDZ_OPEN_SOURCE_REFERENCES.md) | `./scripts/setup-open-source-integrations.sh` |
| **Disklordz agent fleet** (31 agents, repo-wide) | [DISKLORDZ_AGENTS.md](DISKLORDZ_AGENTS.md) · [disklordz/agents/workflows/PM_ADD.md](disklordz/agents/workflows/PM_ADD.md) · [disklordz/agents/ARCHITECTURE.md](disklordz/agents/ARCHITECTURE.md) | `./scripts/run-agent-fleet-now.sh` |
| **META LLM charter** | [disklordz/agents/charter/ARCHITECTURE.md](disklordz/agents/charter/ARCHITECTURE.md) · [CLAUDE.md](CLAUDE.md) | `./scripts/sync-meta-llm-charter.sh` |
| **Antigravity ↔ Cursor bridge** | [disklordz/antigravity/ARCHITECTURE.md](disklordz/antigravity/ARCHITECTURE.md) | `./scripts/antigravity-bridge/antigravity-bridge.sh` |
| **WAVE-9090** (sampleless trap wavetable synth) | [Wave9090/ARCHITECTURE.md](Wave9090/ARCHITECTURE.md) | `Wave9090_VST3`, `Wave9090_Standalone`, `Wave9090Tests` |
| **SP-1200 Drumulator** (standalone + VST3 sampler) | [SP1200/ARCHITECTURE.md](SP1200/ARCHITECTURE.md) · [docs/SP1200_HANDOFF.md](docs/SP1200_HANDOFF.md) | `SP1200_Standalone`, `SP1200_VST3`, `ctest -R SP1200` |

## Repository layout

```
Source/          Plugin processor, DSP, assets, preset import
SP1200/          SP-1200-style standalone sampler (JUCE)
hise-sketch/     HISE / Antigravity projects (optional imports)
disklordz/       Drum SaaS (`website/`) and sound-factory scripts
tools/           CLI binaries (link JUCE / plugin static lib)
tools/synth-forge/  Python SynthForge preset workstation (FastAPI)
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

[`.github/workflows/build.yml`](.github/workflows/build.yml): configure, build, then `python3 vst-testing-ops/run_business.py --profile ci-verify`. Nightly: [`nightly-qa.yml`](.github/workflows/nightly-qa.yml). Setup: [docs/REPO_AUTOMATION.md](docs/REPO_AUTOMATION.md).

Optional Slack: CI ([`ci-slack-notify.yml`](.github/workflows/ci-slack-notify.yml)), Antigravity inbox ([`antigravity-inbox-slack.yml`](.github/workflows/antigravity-inbox-slack.yml)), Airtable handoff ([`airtable-antigravity-handoff.yml`](.github/workflows/airtable-antigravity-handoff.yml)) — configure with [`scripts/setup-disklordz-integrations.sh`](scripts/setup-disklordz-integrations.sh).

**Cursor / agents:** [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) · [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) · **SOPs & templates** [docs/disklordz/README.md](docs/disklordz/README.md) · **Roadmap & progress** [docs/ROADMAP.md](docs/ROADMAP.md) · SaaS deep dive [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) · optional desktop agent [docs/BYTEBOT_SETUP.md](docs/BYTEBOT_SETUP.md).

## Documentation policy

Every product must maintain an `ARCHITECTURE.md` — see [.cursor/rules/architecture-documentation.mdc](.cursor/rules/architecture-documentation.mdc).
