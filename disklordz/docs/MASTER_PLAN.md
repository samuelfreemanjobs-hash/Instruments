# DiskLordz Master Plan — Build the factory first

**Do not** ask Cursor to make 50 plugins. Build **infrastructure**, then **compile** products.

## Phase map

| Phase | Name | Outcome |
|-------|------|---------|
| **1** | Foundation | Company brain, rules, contracts, schemas, gates, orchestrator |
| **2** | Sound Factory | Processing, DNA ingest, hash registry, kits, MPC export |
| **3** | Plugin Factory | Kernel split, DSP modules, presets, UI system, build |
| **4** | QA Factory | Golden Ear, regression, pluginval, DAW matrix |
| **5** | Product Factory | Compiler, families, packaging, docs, demos |
| **6** | Commerce | Store, simple license, delivery, support KB |
| **7** | Intelligence | Market + customer signals, economics, portfolio |
| **8** | Autonomous loop | Idea → compile → RC → human → learn → next |

**Current:** Phase 1 largely in repo; Phase 2–3 execution next.

---

## Phase 1 — Foundation (in repo)

- [x] `disklordz/company/*` — brain, sonic constitution, scoring  
- [x] `.cursor/rules/disklordz-*.mdc` — OS around agents  
- [x] Agent contracts + three meta-skills  
- [x] Sound DNA + Product Genome schemas  
- [x] Gates / definition of done  
- [x] Repository map, factories documented  
- [ ] Airtable base live  
- [ ] Sprint A automations  

## Phase 2 — Sound Factory

- WAV pipeline, classification, metrics → DNA  
- SHA-256 registry (`hash_audio.py`)  
- Character Lab DSP modules  
- Kit + MPC/Maschine export  

## Phase 3 — Plugin Factory

- Migrate `MyFirstPlugin` → `plugin-kernel/`  
- Generator from brief template  
- Preset factory  

## Phase 4 — QA Factory

- `tests/audio/` regression harness  
- CI integration  
- DAW matrix population  

## Phase 5 — Product Factory

- `compile.py` (manifest driver)  
- Content atomizer + demo factory  
- Release templates  

## Phase 6 — Commerce

- Webhook → order → download  
- No heavy DRM v1  

## Phase 7 — Intelligence

- Product idea engine operational  
- Economics on each genome  
- Portfolio metrics  

## Phase 8 — Autonomous company

- Opportunity → compile → gates → human approve → ship → feedback → intelligence  

---

## Success

Creative Director receives **approval packets**; everything else is factory + gates + Airtable truth.

See also: [ARCHITECTURE.md](ARCHITECTURE.md), [VISION_MAP.md](VISION_MAP.md), [../REPOSITORY_MAP.md](../REPOSITORY_MAP.md).
