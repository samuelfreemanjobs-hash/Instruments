# Sound factory — improvement roadmap

**Status:** Tier 1–4 implemented on branch `cursor/instrument-factory-agent-plan-d805` unless noted.

## Tier 1 — Sound quality ✅

| Item | Implementation |
|------|----------------|
| Rev2 DSP depth | WT blend, hard sync, filter FM in `Rev2Voice.h` |
| Analog drift | Slow drift on pitch/cutoff |
| Trap macro | `trapMacro` param + preset application |
| Post-render | `post_render.py` limiter + sub check |
| QA v2 | `factory_qa.py` crest, slope, low-band |

## Tier 2 — Factory pipeline ✅

| Item | Implementation |
|------|----------------|
| SpectralDiff gate | `factory_spectral_gate.py` (optional `--spectral-gate`) |
| best-of-N | `--best-of 3` on `run_product_batch.py` |
| Chunk CI | `.github/workflows/sound-factory-ci.yml` |
| JZ400 on Rev2Trap | Programs 128–527, `Jz400Presets.cpp`, auto engine |

## Tier 3 — Product / UX ✅ (baseline)

| Item | Implementation |
|------|----------------|
| Rev2Trap UI | Macro rotaries: Trap, WT, Cutoff, Drive |
| CLAP | `Rev2Trap` CLAP target in CMake |
| SFZ round-trip | `verify_sfz_roundtrip.py` (not in-plugin sampler yet) |
| MPCTK | `scripts/mpctk_handoff.sh` stub |

**Future:** In-plugin SFZ player; full MPCTK submodule WO.

## Tier 4 — Agents ✅

| Item | Implementation |
|------|----------------|
| factory-qa-audio | Updated for QA v2 metrics |
| PM/Airtable chunks | `jobs/product-batch-template.json` offset/limit/engine/bestOf |
| MPC-agent | Handoff unchanged; register in AGENTS.md when exported |

## Operational flow

```text
Rev2Trap VSTi (528 programs) → Rev2TrapOfflineRender
  → post_render → factory_qa v2 → optional SpectralDiff
  → SFZ + verify_sfz_roundtrip → MPC handoff
```

See [Rev2Trap/ARCHITECTURE.md](../Rev2Trap/ARCHITECTURE.md).
