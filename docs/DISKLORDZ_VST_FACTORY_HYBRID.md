# DiskLordz VST Factory — hybrid platform (index)

**Shape:** Replit-style product intake + engineering studio. **Autonomy:** approval-based (not unattended release).

Operator decisions locked in [DISKLORDZ_VST_FACTORY_PLATFORM.md](DISKLORDZ_VST_FACTORY_PLATFORM.md): internal factory team + `vst-factory-director`, MeterTap dogfood → research SKU #1, [DPCS.md](DPCS.md), QA tier A, Slack `#disklordz-factory-dept`.

## Capabilities → repo → phase

| Capability | Path | Phase |
|------------|------|-------|
| Orchestrator + gates | `vst_factory/orchestrator.py`, `standards/OPERATING_MODES.md` | MVP |
| Certification | `testing/certification-v1.json`, `POST .../certify` | MVP |
| Product WO | `schemas/product-wo-v1.json`, `product_wo.py` | MVP / V3 UI |
| DSP research | `dsp_research.py`, `knowledge/dsp/algorithms/` | V2 |
| Verified knowledge | `knowledge/verified/` | V2 |
| Signal-flow | `signal-flow/schema-v1.json`, `signal_flow/validate.py` | V2 |
| Synth/sampler engines | `vst/factory-lib/engines/` | V2 |
| Reverse-engineering lab | `labs/reverse_engineering/` | V2 |
| Preset batch | `labs/presets/batch_generate.py` | V2 |
| Slack gates | `slack_notify.py`, [VST_FACTORY_SLACK_SETUP.md](VST_FACTORY_SLACK_SETUP.md) | MVP |

## Gates

| Gate | Blocks |
|------|--------|
| `approve_architecture` | scaffold |
| `approve_dsp_promotion` | verified knowledge write |
| `approve_gui` | GUI asset import (V2) |
| `approve_release` | commercial `release_ready` |

## Certification

Weighted score in `testing/certification-v1.json` plus **hard_gates** (build, artefact, pluginval, no crash, no NaN). Fails closed.

## Non-goals (near term)

- Unattended public release
- DAW farm automation
- AAX without licensing WO
- Claiming reverse-engineering clones proprietary circuits

## Related

- [DISKLORDZ_VST_FACTORY_AGENT_ROSTER.md](DISKLORDZ_VST_FACTORY_AGENT_ROSTER.md)
- [disklordz/vst-factory/ARCHITECTURE.md](../disklordz/vst-factory/ARCHITECTURE.md)
