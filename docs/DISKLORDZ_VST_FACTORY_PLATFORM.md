# DiskLordz VST Factory — platform index

Master map for the **12-system** commercial audio development platform. Phase 1 orchestration lives in [DISKLORDZ_VST_FACTORY.md](DISKLORDZ_VST_FACTORY.md); this document indexes standards, intelligence, and roadmap.

**Dogfood rule:** Shared blocks in `vst/factory-lib/`, tests in `vst-testing-ops/`, and factory conventions must be used to ship the next SKU.

**Operator order (locked):** **B** — **MeterTap** first (factory proof), then **market-research SKU #1** from audio-business-copilot. Implementation obeys [DPCS.md](DPCS.md) (phase-locked; Phase 4 = scripts exit 0).

**QA tier (locked):** **A** — GitHub Actions / Linux: `build-vst.sh` + `run_business.py --profile factory` on factory PRs; **pluginval** and DAW smoke on your Windows machine when online (not a CI blocker day one).

## Twelve systems

| # | System | Location |
|---|--------|----------|
| 1 | AI Studio | [disklordz/vst-factory/static/](../disklordz/vst-factory/static/) |
| 2 | Agent Engine | [vst_factory/orchestrator.py](../disklordz/vst-factory/vst_factory/orchestrator.py), [agents/](../disklordz/vst-factory/agents/) |
| 3 | DSP Intelligence | [knowledge/dsp/](../disklordz/vst-factory/knowledge/dsp/), [vst/factory-lib/](../vst/factory-lib/) |
| 4 | Music Intelligence | [DiskLordz-Audio/DPF-MIDI/](../DiskLordz-Audio/DPF-MIDI/) (commercial); APC `vst/` for greenfield |
| 5 | Plugin Framework | [vst/template/](../vst/template/), APC [plugins/](../plugins/) |
| 6 | GUI Studio | [gui/](../disklordz/vst-factory/gui/) (DSL V2) |
| 7 | Code Studio | Cursor / future Monaco in `web/` |
| 8 | Build Engine | [build_broker.py](../disklordz/vst-factory/vst_factory/build_broker.py), [tools/build-vst.sh](../tools/build-vst.sh) |
| 9 | Testing Lab | [vst-testing-ops/](../vst-testing-ops/) |
| 10 | Project Manager | SQLite + [data/product-library.json](../disklordz/vst-factory/data/product-library.json) |
| 11 | Release Manager | [tools/package-vst.sh](../tools/package-vst.sh), `release/<Name>/` |
| 12 | Business Studio | [ANTIGRAVITY_PRODUCT_FACTORY.md](ANTIGRAVITY_PRODUCT_FACTORY.md) |

## Standards & policy

| Doc | Path |
|-----|------|
| Engineering | [standards/ENGINEERING.md](../disklordz/vst-factory/standards/ENGINEERING.md) |
| Formats | [standards/FORMATS.md](../disklordz/vst-factory/standards/FORMATS.md) |
| Memory | [standards/MEMORY.md](../disklordz/vst-factory/standards/MEMORY.md) |
| Commercial / security | [standards/COMMERCIAL_READINESS.md](../disklordz/vst-factory/standards/COMMERCIAL_READINESS.md) |
| DSP spec (repo) | [dsp-architecture-specification.md](dsp-architecture-specification.md) |

## Modules & schemas

| Artifact | Path |
|----------|------|
| Product taxonomy | [modules/product-taxonomy.json](../disklordz/vst-factory/modules/product-taxonomy.json) |
| Factory spec v2 | [schemas/factory-spec-v2.json](../disklordz/vst-factory/schemas/factory-spec-v2.json) |
| DAW matrix | [testing/daw-matrix.schema.json](../disklordz/vst-factory/testing/daw-matrix.schema.json) |
| Feature registry | [modules/features.json](../disklordz/vst-factory/modules/features.json) |

## Roadmap gates

### MVP (current tranche)

- Platform doc stack, taxonomy, spec v2, portfolio API, memory injection
- `factory` + `factory-audio` QA profiles
- MeterTap dogfood SKU definition
- DSP Research agent + knowledge index

### V2

- GUI DSL generator, DAW evidence UI, Ollama model router, real `factory-lib` C++ blocks

### V3

- Multi-DAW automation, AAX, marketplace handoff, portfolio regression graph

## Related

- [DISKLORDZ_VST_FACTORY_AGENT_ROSTER.md](DISKLORDZ_VST_FACTORY_AGENT_ROSTER.md)
- [disklordz/vst-factory/ARCHITECTURE.md](../disklordz/vst-factory/ARCHITECTURE.md)
- [CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md)
- Fleet foreman: [vst-factory-director](../disklordz/agents/profit/vst-factory-director/agent.md)
