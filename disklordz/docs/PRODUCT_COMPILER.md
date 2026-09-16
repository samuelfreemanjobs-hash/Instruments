# Product Compiler

The **compiler** turns a **Product Spec** + **Sound DNA assets** into shippable artifacts. One spec, many targets.

## Inputs

1. `product-spec.json` — SKU, family, sonic problem statement, ladder tier  
2. Asset set — IDs from Sound DNA (all `commercial_ok`)  
3. `compile-targets` — which outputs to build  

## Outputs (targets)

| Target | Artifact | Factory |
|--------|----------|---------|
| `drum_kit_zip` | WAV folders + README + LICENSE | Sound Factory + Kit Builder |
| `mpc_expansion` | MPC programs + samples | Format adapter |
| `maschine_expansion` | Maschine-compatible layout | Format adapter |
| `vst3_effect` | `.vst3` + presets | Plugin Kernel product |
| `vst3_instrument` | sampler instrument | Plugin Kernel product |
| `preset_pack` | JSON/XML presets | Preset Factory |
| `free_teaser` | subset + watermark policy | Compiler rule |
| `bundle` | manifest of SKUs | Product Family |
| `content_pack` | copy, social, email, YT desc | Content Factory |
| `demo_audio` | loops, before/after | Demo Generator |

## Example: one creative idea → family

**Concept:** *Dark 2000s Digital Sampler*

| SKU | Target | Tier |
|-----|--------|------|
| A | 500 WAV | $29–49 |
| B | MPC expansion | included / upsell |
| C | Maschine | included / upsell |
| D | VST drum instrument | $49–99 |
| E | VST crush effect | $49–99 |
| F | 100 presets | expansion |
| G | Demo beats | marketing |
| H | Free 20-sample teaser | free |
| I | Bundle I+E+F | $99–199 |
| J | Launch campaign | content_pack |

Orchestrator runs one **compile job** with shared asset lockfile.

## Manifest

Schema: [`../product-compiler/manifest.schema.json`](../product-compiler/manifest.schema.json)

Compiler steps:

1. Validate provenance on all assets  
2. Build each target in dependency order (plugin before preset pack that references it)  
3. Run QA Lab profile for targets  
4. Write `PRODUCT_INFO.json` + version matrix  
5. Emit final zip(s) to `releases/`  

## Automation

Workflow Automation Engineer implements:

- Airtable `Products.status` = `In Development` → trigger compile job on tag  
- Failed provenance → block + PM alert  
- Success → `Releases` candidate row + Content Factory queue  
