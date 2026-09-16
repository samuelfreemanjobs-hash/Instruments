# DiskLordz Company OS

DiskLordz is an **autonomous audio-product company**. Cursor is the engineering environment; agents are the workforce. **Structured artifacts** (schemas, gates, contracts) beat chat history.

## Stack

| Layer | Location |
|-------|----------|
| Company brain | `disklordz/company/` |
| Production engine | `disklordz/docs/ARCHITECTURE.md` |
| Factories | `sound-factory/`, `plugin-kernel/`, `product-compiler/`, `content-factory/` |
| Data | Sound DNA, Product Genome, Airtable |
| Governance | `agents/contracts/`, `.cursor/rules/` |
| Knowledge | `disklordz/knowledge/` |
| Quality | `disklordz/gates/`, `tests/audio/` |

## Divisions (orchestrator view)

```text
INTELLIGENCE ── SOUND ── SOFTWARE ── PRODUCT ── COMMERCE
                      │
                 MEDIA FACTORY
                      │
               RELEASE FACTORY
                      │
               LEARNING ENGINE → NEW PRODUCTS
```

## Non-negotiables

1. **Sonic Constitution** governs taste; score before ship.  
2. **Provenance** hard gate — unknown → STOP.  
3. **Product Genome** for every SKU — no vague briefs in production.  
4. **Definition of Done** per gate in `gates/DEFINITION_OF_DONE.md`.  
5. **Human approval** for publish, pricing, irreversible commercial actions.  
6. **Agent contracts** — no role exceeds CAN/CANNOT/MUST.  
7. **Reuse** — kernel, DNA, presets before greenfield.  

## Read order for agents

1. `mission.md` → `sonic-constitution.md` → `decision-framework.md`  
2. `../gates/DEFINITION_OF_DONE.md`  
3. Role contract in `../agents/contracts/`  
4. Task-specific schema (genome, DNA, manifest)
