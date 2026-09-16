# DiskLordz Audio Lab — Production Engine Architecture

**DiskLordz** is the first product vertical on **TRON Studio** (autonomous company infrastructure). The business is not “AI makes plugins and kits”; it is an **autonomous audio-product company** with a **reusable production engine** that compiles assets into many product shapes.

## North star

```text
                 CREATIVE IDEA
                       │
                       ▼
                 PRODUCT SPEC
                       │
                       ▼
              SOUND DNA + ASSETS
                       │
             ┌─────────┼─────────┐
             ▼         ▼         ▼
            WAV       MIDI     PRESETS
             │         │         │
             └─────────┼─────────┘
                       ▼
              PRODUCT COMPILER
                       │
       ┌───────────────┼────────────────┐
       ▼               ▼                ▼
    DRUM KIT         PLUGIN          CONTENT
       │               │                │
       ▼               ▼                ▼
     ZIP/MP3      INSTALLER         STORE PAGE
```

One sonic concept (e.g. *Dark 2000s Digital Sampler*) can emit a **product family**: WAV pack, MPC/Maschine, VST instrument, VST effect, presets, free teaser, bundle, demos, and launch copy—without redesigning the pipeline each time.

## Company OS (factories, not agent sprawl)

```text
                         AUDIO COMPANY OS
                                │
                    ┌───────────▼───────────┐
                    │  PRODUCT ORCHESTRATOR  │  ← one meta-agent + PM + automation
                    └───────────┬───────────┘
                                │
          ┌─────────────────────┼─────────────────────┐
          ▼                     ▼                     ▼
     SOUND FACTORY        PLUGIN FACTORY       CONTENT FACTORY
          │                     │                     │
     WAV / MIDI / kits      VST3 / AU / presets   web / video / copy
          │                     │                     │
          └─────────────────────┼─────────────────────┘
                                ▼
                         PRODUCT COMPILER
                                ▼
                    QA LAB + PROVENANCE GATE
                                ▼
                    PACKAGING + VERSIONING
                                ▼
                         STORE / DELIVERY
                                ▼
                         CUSTOMER / FEEDBACK
                                │
                                └──────► PRODUCT R&D (intelligence loop)
```

**Every shipped product feeds R&D:** feedback, sales signals, and asset reuse inform the next compile.

## Subsystems (code + docs homes)

| Subsystem | Path | Purpose |
|-----------|------|---------|
| Sound DNA | `sound-dna/` | Asset schema, embeddings/search, provenance |
| Sound Factory | `sound-factory/` | Generation pipeline → variants → QA → DB |
| Hardware Character Lab | `sound-factory/character-lab/` | Original DSP character (not branded clones) |
| Plugin Kernel | `plugin-kernel/` | Shared JUCE engine; products = kernel + module |
| Plugin Generator | `plugin-kernel/generator/` | Brief → spec → scaffold from templates |
| Preset Factory | `preset-factory/` | Presets from kits + plugin params |
| QA Lab | `qa-lab/` | Technical + audio tests; DAW matrix |
| Product Compiler | `product-compiler/` | Manifest-driven zip/installer/content |
| Content Factory | `content-factory/` | Copy, demos, launch assets from product truth |
| Intelligence | `intelligence/` | Market signals, knowledge graph queries |
| Commerce (v1 simple) | `commerce/` | Order → download → license record (no heavy DRM) |

Implementation phases tie to [MASTER_PLAN.md](MASTER_PLAN.md). **Do not add 25 chat agents**—add **factories** and let the orchestrator dispatch **team roles** (see [AGENTS.md](../AGENTS.md)).

## Product families (universe, not SKU list)

```text
DISKLORDZ UNIVERSE
        │
   SOUND ENGINE (DNA + Character Lab)
        │
   ┌────┴────┬────────────┐
   ▼         ▼            ▼
 DRUMS    EFFECTS    INSTRUMENTS
   │         │            │
 packs    plugins     instruments
   └────┬────┴────────────┘
        ▼
   EXPANSIONS + BUNDLES
```

`Product Families` in Airtable links SKUs that share DNA and marketing narrative.

## Commercial ladder (catalog first, no subscription day one)

| Tier | Example | Role |
|------|---------|------|
| Free | Teaser kit | Top of funnel |
| $9–19 | Mini expansion | Habit + email list |
| $29–49 | Full kit | Core sample revenue |
| $49–99 | Plugin / instrument | Engine showcase |
| $99–199 | Plugin + expansion | Bundle margin |
| $199–299 | Collection | Universe sell |
| Later | All-access | After catalog depth |

## Versioning (day one)

Every artifact class carries semver (or kit vol):

- Plugin binary, preset pack, sample library, installer, license format, docs  
- Stored on `Releases` + `product-compiler/manifest.schema.json`

## TRON Studio separation

| Layer | Owns |
|-------|------|
| **TRON Studio** | Orchestrator, automation engineer, Airtable patterns, Cloud Agent env, generic compiler framework |
| **DiskLordz Audio Lab** | Sound DNA, character algorithms, brand, catalog, JUCE products |

Other verticals (software, marketing, industrial) reuse TRON; DiskLordz proves the factory in a hard domain (audio + licensing + QA).

## Related docs

- [SOUND_DNA.md](SOUND_DNA.md) — proprietary asset intelligence  
- [PLUGIN_KERNEL.md](PLUGIN_KERNEL.md) — shared engine + generator  
- [PRODUCT_COMPILER.md](PRODUCT_COMPILER.md) — compile targets  
- [LICENSING.md](LICENSING.md) — VST3, JUCE, provenance, ship gates  
- [QA_LAB.md](QA_LAB.md) — validation matrix  
- [CONTENT_FACTORY.md](CONTENT_FACTORY.md) — marketing + demos from product truth  
