# Vision map — your 20 expansions → repo

This table tracks the **production engine** vision (not agent sprawl). Status: **Doc** = specified in repo; **Schema** = machine-readable; **Code** = runnable; **Planned** = next implementation phase.

| # | Your pillar | Repo home | Status |
|---|-------------|-----------|--------|
| — | Audio Product Factory OS | [ARCHITECTURE.md](ARCHITECTURE.md) | Doc |
| — | Product Orchestrator | `.cursor/skills/disklordz-product-orchestrator/` | Skill |
| — | Asset → Product Compiler | [PRODUCT_COMPILER.md](PRODUCT_COMPILER.md), `product-compiler/` | Doc + Schema |
| 1 | Sound DNA / asset database | [SOUND_DNA.md](SOUND_DNA.md), `sound-dna/schema/sound-asset.schema.json` | Doc + Schema |
| 2 | Sound generation pipeline | `sound-factory/`, [SOUND_DNA.md](SOUND_DNA.md#sound-generation-pipeline) | Doc; pipelines Planned |
| 3 | Hardware Character Lab | `sound-factory/character-lab/MODULES.md` | Doc; DSP Planned |
| 4 | Plugin Generator | `plugin-kernel/generator/brief.template.json`, [PLUGIN_KERNEL.md](PLUGIN_KERNEL.md) | Doc + template |
| 5 | Reusable Plugin Kernel | [PLUGIN_KERNEL.md](PLUGIN_KERNEL.md), `plugin-kernel/`; code in `MyFirstPlugin/` → migrate | Doc; Code partial |
| 6 | Preset Factory | [preset-factory/README.md](../preset-factory/README.md) | Doc stub |
| 7 | Audio QA Lab | [QA_LAB.md](QA_LAB.md), `qa-lab/profiles/` | Doc + profile v0 |
| 8 | DAW compatibility matrix | Airtable `DAW Matrix`, [QA_LAB.md](QA_LAB.md) | Schema |
| 9 | Licensing / provenance | [LICENSING.md](LICENSING.md), `validate_provenance.py` | Doc + Code stub |
| 10 | Sonic problems (not generic genres) | Orchestrator skill, `sonic_problem` fields | Doc + Schema |
| 11 | Free → paid ladder | [MASTER_PLAN.md](MASTER_PLAN.md), `ladder_tier` on Products | Doc + Schema |
| 12 | Content Factory | [CONTENT_FACTORY.md](CONTENT_FACTORY.md), `content-factory/` | Doc |
| 13 | Audio Demo Generator | [CONTENT_FACTORY.md](CONTENT_FACTORY.md#audio-demo-generator) | Doc; tools Planned |
| 14 | Feedback → improvements | Airtable `Customer Feedback`, `intelligence/` | Schema |
| 15 | Audio knowledge graph | Airtable `Asset Relations`, [SOUND_DNA.md](SOUND_DNA.md) | Schema |
| 16 | Product intelligence | Airtable `Market Signals`, `intelligence/` | Schema |
| 17 | Product families / universe | Airtable `Product Families`, [ARCHITECTURE.md](ARCHITECTURE.md) | Doc + Schema |
| 18 | License / delivery v1 simple | [commerce/README.md](../commerce/README.md), [LICENSING.md](LICENSING.md) | Doc stub |
| 19 | Version infrastructure | `Releases` + `manifest.schema.json` `versions` | Schema |
| 20 | Team architecture (not 25 agents) | [AGENTS.md](../AGENTS.md) | Doc + skills |
| — | VST3 MIT ≥3.8, no VST2 | [LICENSING.md](LICENSING.md) | Doc |
| — | JUCE Starter / revenue tracking | [LICENSING.md](LICENSING.md) | Doc |
| — | TRON Studio vs DiskLordz | [ARCHITECTURE.md](ARCHITECTURE.md#tron-studio-separation) | Doc |

**PR:** [DiskLordz agent OS / production engine](https://github.com/samuelfreemanjobs-hash/Instruments/pull/9) on branch `cursor/disklordz-agent-os-ef38`.
