# ILLUGEN research → Disklordz product map

**Purpose:** Capture competitive reverse-engineering of Waves ILLUGEN (text-to-sound) and map it to Disklordz SaaS phases. Use this when writing **WO-SAAS-007+** in Airtable and PR titles.

**Related:** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) · [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md) · [disklordz/rag/ARCHITECTURE.md](../disklordz/rag/ARCHITECTURE.md)

---

## What ILLUGEN is (product level)

- **Text → musical intent → remote generative model → multiple WAV candidates → local library → DAW**
- Waves positions it as **text-to-sound**, not a static sample library.
- The **audio engine runs on Waves’ servers**; the desktop app is a **client/orchestrator** (internet required).

Disklordz implication: separate **orchestration product** from **generation engine** so you can start with parametric/factory audio and swap in hosted models later without rewriting UX.

---

## Reference architecture (ILLUGEN-shaped)

```text
User prompt + spec (mode, key, BPM, length, stereo, model, creativity)
    → Auth / credits API
    → Generation API (job)
    → Remote audio engine (variations + post)
    → Result manager (preview, metadata, history)
    → Local folder + cloud backup (~6 months) → DAW drag/drop
```

Disklordz v0 today implements a **thin slice**: prompt + preset → sync generate → preview/ZIP → optional Supabase history. See checklist in [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md).

---

## Twelve systems vs Disklordz status

| # | System | ILLUGEN | Disklordz (PR #28 / main) |
|---|--------|---------|---------------------------|
| 01 | Prompt intelligence | Semantic + UI spec | Prompt + 5 lane presets |
| 02 | Musical intent parser | Key, BPM, bars, mode | `prompt-params` (partial) |
| 03 | Audio generation gateway | Async jobs + credits | Sync `POST /api/generate` |
| 04 | Creative model | 3 variations | Single parametric synth path |
| 05 | Studio model | 2 variations, higher fidelity | Not split |
| 06 | Variation engine | Candidate sets + “Generate more” | One kit per request |
| 07 | Post-processor | Loop build, conditioning | Basic synth → WAV |
| 08 | Sample library | Local folder + COSMOS | ZIP download |
| 09 | Generation history | Rich search/filter | `saved_kits` (when Supabase live) |
| 10 | Credit/billing | Per-operation ledger | IP daily cap (in-memory) |
| 11 | DAW export | Drag/drop desktop | Web only |
| 12 | Autonomous sample factory | N/A at consumer scale | `sound-factory/` + kit ZIP |
| **13** | **Product factory** (Disklordz moat) | General sounds | Lane-specific packs, metadata, storefront |

---

## Generation spec (target shape)

ILLUGEN treats the prompt as **one field in a spec**. Target API body for Disklordz:

```json
{
  "mode": "one_shot",
  "engine": "studio",
  "prompt": "1997 Memphis phonk cowbell, SP-1200 grit",
  "presetId": "DL006",
  "key": "F# minor",
  "bpm": 92,
  "bars": 4,
  "length": "medium",
  "stereo": 0.35,
  "wildness": 0.6,
  "variations": 2
}
```

**WO-SAAS-007** introduces this schema in the API and UI (presets seed defaults).

---

## Credit model (v1 target)

| Operation | Credits (example) |
|-----------|-------------------|
| One shot | 1 |
| SFX short / medium / long | 1 / 2 / 3 |
| Loop | 2–7 |
| Studio engine surcharge | +N |

Backend pattern: **reserve → job → commit** or **release** on failure (see [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md) for job table sketch).

---

## Disklordz differentiation (vs generic ILLUGEN)

Structured **genre stack** for lanes: phonk, drift, cyber funk, screw, French touch → era → instrument → processing → tempo → key → pattern.

Example structured conditioning (fed to factory + RAG prompt assistant):

```json
{
  "genre": "memphis_phonk",
  "era": "1990s",
  "instrument": "cowbell",
  "character": ["lo-fi", "distorted", "gritty", "sampler"],
  "key": "F# minor",
  "bpm": 92,
  "bars": 4,
  "stereo": 0.35,
  "processing": { "saturation": 0.72 }
}
```

---

## Phased backlog (WO-SAAS-007+)

| WO | Title | Outcome |
|----|--------|---------|
| **007** | Generation spec v1 | `mode`, `engine`, `key`, `bpm`, `bars`, `stereo`, `wildness` in API + UI; presets fill defaults |
| **008** | Variation engine | 2 (studio) / 3 (creative) candidates per spec; audition UI; “Generate more” frozen spec |
| **009** | Async jobs + storage | Job queue, object storage (S3/R2/Supabase Storage); drop `/tmp`-only kits on Vercel |
| **010** | Credits + Stripe | Reserve/commit ledger; map operation → cost; Pro tier |
| **011** | History v2 | Search/filter: BPM, key, mode, engine, date; expand prior prompts |
| **012** | RAG prompt assistant | Vector index over lanes + prompt tips; suggest spec + “Random” discovery loop |
| **013** | Creative vs Studio routes | Two generation backends; routing by `engine` |
| **014** | Loop + SFX modes | Bar-length loops, SFX length tiers; credit table |
| **015** | Product factory batch | Brief → N WAVs per folder → ZIP SKU + manifest for storefront |
| **016** | Desktop / watch folder | Optional Electron or folder sync for DAW drag/drop |

Each WO: Airtable → GitHub issue → PR title `WO-SAAS-00N` → merge → Done.

---

## What to steal from ILLUGEN (design principles)

1. **Idea generation** — random prompt / RAG suggestions  
2. **Specification** — musical controls beside natural language  
3. **Variation** — multiple candidates per run  
4. **Asset management** — searchable generation DB  
5. **Production integration** — fastest path into DAW (web ZIP now; desktop later)

The model is not the whole product; **orchestration + metadata + credits + history** are.

---

## SaaS ladder alignment

| Stage | ILLUGEN parallel | Disklordz focus |
|-------|------------------|-----------------|
| v0 (shipped in git) | Demo web + thin gen | Kits, ZIP, auth stub |
| v0.5 | Spec + variations | WO 007–008 |
| v1 | Credits + Pro | WO 009–010 |
| v2 | Library + API | WO 011–012 |
| v3 | Team + factory | WO 014–016 |
