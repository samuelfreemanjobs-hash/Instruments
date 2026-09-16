# DiskLordz Audio Lab

**Autonomous audio-product company** built on a **reusable production engine** (not a pile of one-off agents). **TRON Studio** = infrastructure; **DiskLordz** = first vertical.

## Start here

| Document | Purpose |
|----------|---------|
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Factories, compiler, TRON separation |
| [docs/MASTER_PLAN.md](docs/MASTER_PLAN.md) | Phased plan to autopilot |
| [docs/SOUND_DNA.md](docs/SOUND_DNA.md) | Proprietary asset intelligence |
| [docs/PLUGIN_KERNEL.md](docs/PLUGIN_KERNEL.md) | Shared JUCE engine + generator |
| [docs/PRODUCT_COMPILER.md](docs/PRODUCT_COMPILER.md) | One idea → product family |
| [docs/LICENSING.md](docs/LICENSING.md) | VST3 MIT, JUCE tiers, provenance gates |
| [docs/QA_LAB.md](docs/QA_LAB.md) | Technical + audio validation |
| [docs/CONTENT_FACTORY.md](docs/CONTENT_FACTORY.md) | Copy + demos from product truth |
| [docs/AIRTABLE_OPERATING_MODEL.md](docs/AIRTABLE_OPERATING_MODEL.md) | PM workflows |
| [docs/AUTOMATION_ROADMAP.md](docs/AUTOMATION_ROADMAP.md) | Automation backlog |
| [AGENTS.md](AGENTS.md) | Orchestrator + teams (not 25 agents) |

## Subsystems

| Path | Role |
|------|------|
| `sound-dna/` | JSON schema + future search/graph |
| `sound-factory/` | Generation + Character Lab |
| `plugin-kernel/` | Shared engine + generator briefs |
| `product-compiler/` | Manifest + provenance validation |
| `content-factory/` | Marketing/demo outputs |
| `qa-lab/` | CI profiles |
| `intelligence/` | Market signals + feedback loop |
| `automation/` | Zapier/n8n/scripts |

## Cursor skills

| Skill | Role |
|-------|------|
| `disklordz-product-orchestrator` | **CEO factory** — family compiles, team dispatch |
| `disklordz-audio-pm` | **Airtable PM** — truth, cadence, work orders |
| `disklordz-workflow-automation-engineer` | **Autopilot glue** — integrations until done |

## Repos (target layout)

```text
disklordz/
├── plugin-engine/      # JUCE VST3 (this repo: MyFirstPlugin → future split)
├── drum-factory/       # sample generation + processing
├── sample-library/     # sound assets + metadata
├── product-generator/  # kits, zips, store payloads
├── automation/         # workflows + scripts (this folder grows here)
└── website/            # storefront
```

Human role: **Creative Director** — taste, final listen, brand. Everything else trends toward agents + automations.
