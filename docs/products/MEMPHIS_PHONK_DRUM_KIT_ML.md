# Memphis Phonk Drum Kit ML Engine

**Freeman Intelligence / Disklordz**  
**Started:** 2026-09-28  
**Status:** PHASE 0 — Research & characterization  
**Completion:** ~5% (planning + DDSP foundation)

## Proposed outcome

AI-assisted pipeline that learns **Memphis Phonk drum character** from reference audio and produces **sellable drum kits** (24-bit WAV one-shots + loops) under the Disklordz brand — Beatstars, Gumroad, Etsy, or owned storefront.

## Audience

Producers in Memphis Phonk, drift phonk, 90s Memphis rap, lo-fi trap.

## Commercial model

| Tier | Range |
|------|--------|
| Premium kits | $10–$30 |
| Bundles | $49–$99 |
| Subscription (future) | ~$9.99/mo |

Digital goods: zero marginal cost after generation + QC.

## Stack (repo mapping)

| Layer | Tools | Where in repo |
|-------|--------|----------------|
| Analysis | librosa, scipy, soundfile | Phase 0 scripts (TBD); fingerprint doc below |
| ML training | PyTorch DDSP | [`tools/drum-synth-blueprint/`](../../tools/drum-synth-blueprint/ARCHITECTURE.md) |
| 808 param encoder | Mel → 4 controls + ONNX | [`torch_ddsp/`](../../tools/drum-synth-blueprint/drum_synth_blueprint/torch_ddsp/) · [`scripts/ddsp_training_loop.py`](../../tools/drum-synth-blueprint/scripts/ddsp_training_loop.py) |
| Browser design (Engine B) | WebAudio kick/snare | [`disklordz/memphis-architect/`](../../disklordz/memphis-architect/ARCHITECTURE.md) |
| Kit scripting | Python packager | [`disklordz/sound-factory/`](../../disklordz/sound-factory/ARCHITECTURE.md) (extend) |
| Generation candidates | AudioLDM2, DrumGAN, EnCodec VAE | Evaluate in Phase 1 — not wired |

**Output spec (product):** 24-bit WAV, 44100 Hz, mono one-shots; stereo loops where applicable.

## Reference fingerprint

Five reference loops characterized in Phase 0 — full table and cross-file DNA:

→ **[memphis_phonk_fingerprint.md](../../tools/drum-synth-blueprint/docs/memphis_phonk_fingerprint.md)**

Store licensed reference WAVs in **`tools/drum-synth-blueprint/reference-loops/`** (flat folder; see that README). Do not commit copyrighted third-party loops without rights.

## Done

- [x] Reference loop spectral + rhythmic characterization (5 files) — see fingerprint doc
- [x] DDSP 808 encoder training loop (PyTorch)
- [x] `DrumSampleDataset` — flat `.wav` folder, resample/mono/pad/peak-norm
- [x] Memphis Phonk DNA summary (sub%, BPM, centroid, dynamics)

## To-do (phased)

| Phase | Work |
|-------|------|
| **0** | ~~Slicer spike~~ **shipped** — [`slice_phonk_loops.py`](../../tools/drum-synth-blueprint/scripts/slice_phonk_loops.py) |
| **1** | Curated URL ingest — [`phonk_sample_ingest_agent.py`](../../disklordz/integrations/agents/phonk_sample_ingest_agent.py) (robots-aware; no blind crawl) |
| **2** | Auto-label QC + SQLite feature store |
| **3** | Train on labeled library (DDSP kicks; diffusion/VAE for snare/hat/cowbell) |
| **4** | Post chain to match fingerprint (compression, saturation, limiter) |
| **5** | Kit packager + branding + first storefront listing |

Checklist (original):

- [x] One-shot slicer: onset detection → per-hit WAV + heuristic label
- [ ] Sample download — **URL list + robots.txt** (Looperman crawl: operator extends after ToS review)
- [ ] Auto-labeler + feature database (SQLite or Airtable)
- [ ] ML approach decision (DDSP vs diffusion vs VAE)
- [ ] Train on labeled one-shots
- [ ] Post-processing to fingerprint
- [ ] Kit packager folder layout + product page

**Urgency (owner):** first kit target ~4 weeks from 2026-09-28.

## Agents (fleet mapping)

| Planned role | Existing / path |
|--------------|------------------|
| Sample download | **[`phonk_sample_ingest_agent.py`](../../disklordz/integrations/agents/phonk_sample_ingest_agent.py)** — robots-aware URL list; extend Looperman after license review |
| Slicer + label | **[`slice_phonk_loops.py`](../../tools/drum-synth-blueprint/scripts/slice_phonk_loops.py)** + **phonk_kit/slicer.py** |
| ML training | **ddsp-ml-engineer** — [`ddsp_ml_engineer.py`](../../disklordz/integrations/agents/ddsp_ml_engineer.py) |
| JUCE / realtime | **audio-plugin-coder** |
| PRD / kit SKU | **code-project-planner** |
| Packager / presets | **hardware-preset-designer** + **sound-factory** scripts |

New profit agents require [`PM_ADD.md`](../../disklordz/agents/workflows/PM_ADD.md) + `_specs.json` sync.

## Artifacts (correct paths)

| Artifact | Path |
|----------|------|
| Folder training CLI | `tools/drum-synth-blueprint/scripts/ddsp_training_loop.py` |
| Loop slicer | `tools/drum-synth-blueprint/scripts/slice_phonk_loops.py` |
| Sample ingest agent | `disklordz/integrations/agents/phonk_sample_ingest_agent.py` |
| Dataset | `drum_synth_blueprint/torch_ddsp/dataset.py` |
| ONNX export | `artifacts/ddsp_808_encoder.onnx` (under drum-synth-blueprint) |
| R&D log | [docs/RD_EXPERIMENT_LOG.md](../RD_EXPERIMENT_LOG.md) |

## API keys

None for local DDSP. Optional later: Replicate (hosted AudioLDM2).

## Related

- [DDSP_TRAP_PHONK_AGENT_PROMPTS.md](../DDSP_TRAP_PHONK_AGENT_PROMPTS.md)
- [SNARE_RESEARCH_PLAN.md](../SNARE_RESEARCH_PLAN.md)
- [DISKLORDZ_ILLUGEN_RESEARCH.md](../DISKLORDZ_ILLUGEN_RESEARCH.md) — genre stack JSON
- [COMPANY_MEMORY_INDEX.md](../COMPANY_MEMORY_INDEX.md)
