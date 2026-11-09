# DiskLordz VST Factory

## Purpose

Local-first **AI development studio** for VST3 plugins: plain-English intake, APC-aligned project state, JUCE scaffold, compile, and QA — without conflating SaaS drum generation or the CHORD ENGINE DPF tree.

Operators: you. Implementers: Cursor Factory (`disklordz-audio-engineering-lane`) + this HTTP surface.

## Build & run

```bash
cd disklordz/vst-factory
python3 -m venv .venv && source .venv/bin/activate   # Windows: py -3 -m venv .venv && .\.venv\Scripts\Activate.ps1
pip install -e ".[dev]"
pytest tests -q
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
```

Env (bash):

```bash
export VST_FACTORY_DB=factory_projects.sqlite
export VST_FACTORY_REPO_ROOT=../..          # optional; auto-detected
export VST_FACTORY_SLACK_WEBHOOK_URL=       # leave empty → Slack no-op
export VST_FACTORY_PUBLIC_URL=http://127.0.0.1:8790
export VST_FACTORY_URL=http://127.0.0.1:8790
```

Env (PowerShell):

```powershell
$env:VST_FACTORY_DB = "factory_projects.sqlite"
$env:VST_FACTORY_SLACK_WEBHOOK_URL = ""   # unset/empty → Slack no-op
$env:VST_FACTORY_PUBLIC_URL = "http://127.0.0.1:8790"
$env:VST_FACTORY_URL = "http://127.0.0.1:8790"
```

Never commit webhook URLs. See [docs/VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md).

## Data flow

```text
Prompt + category
  → POST /api/projects (SQLite row + factory-spec/v2 JSON)
  → approve_architecture gate (Slack notify if webhook set)
  → POST /api/projects/{id}/scaffold
       → plugins/<Name>/status.json + .ideas/creative-brief.md
       → tools/new-vst.sh
  → POST /api/projects/{id}/build
       → build_broker (linux: tools/build-vst.sh)
  → POST /api/projects/{id}/test
       → vst-testing-ops/run_business.py --profile factory --plugin <Name>
  → POST /api/projects/{id}/certify
       → certification.evaluate_certification (hard gates)
  → model_router (Phase 1: export Cursor prompt text to DB / API)
```

Portfolio / taxonomy (read-only overlays):

- `GET /api/portfolio` — `data/product-library.json` merged with SQLite projects
- `GET /api/taxonomy` — `modules/product-taxonomy.json`

CHORD ENGINE reference: `GET /api/references/chord-engine` (no duplicate scaffold).

## Threading / realtime

HTTP only in this service. Generated C++ must follow [docs/dsp-architecture-specification.md](../../docs/dsp-architecture-specification.md) (no alloc/I/O in `processBlock`).

## Orchestration states

`intake` → `dream` → `plan` → `design` → `scaffold` → `impl` → `build` → `test` → `repair` → `ship`

Phase 1 automates: `intake`, `scaffold`, `build`, `test`, `certify`. Others are manual/Cursor until Phase 2.

## Module library (stored templates)

Reusable JSON + markdown under `modules/` and `templates/` — loaded by `vst_factory/modules.py` and exposed on `/api/modules/*`.

| Path | Role |
|------|------|
| `modules/catalog.json` | Index of plugin classes, DSP, GUI, workflows |
| `modules/product-taxonomy.json` | Commercial categories (e.g. `fx.metering`) |
| `modules/plugin-classes/*.json` | Synth / FX / MIDI / Sampler defaults + module ids |
| `modules/dsp/*.json` | DSP block catalog (links to `vst/factory-lib/`) |
| `modules/gui/controls.json` | GUI control catalog |
| `templates/apc/*.md` | Creative brief stubs per class |
| `templates/prompts/cursor-impl.md` | Cursor implementer export |
| `agents/*.md` | Role prompts (Phase 2 orchestration) |

## Key modules

| Path | Role |
|------|------|
| `vst_factory/main.py` | FastAPI routes, static UI mount |
| `vst_factory/db.py` | SQLite projects, runs, approvals, product WOs |
| `vst_factory/orchestrator.py` | State transitions, subprocess jobs, gates |
| `vst_factory/certification.py` | Hard-gate certify (`testing/certification-v1.json`) |
| `vst_factory/portfolio.py` | Product library + DB merge for `/api/portfolio` |
| `vst_factory/modules.py` | Catalog, taxonomy, factory-spec/v2 builder |
| `vst_factory/product_wo.py` | Product work-order snapshot builder |
| `vst_factory/dsp_research.py` | DSP research stub + `approve_dsp_promotion` |
| `vst_factory/apc_bridge.py` | APC `status.json` + brief stubs |
| `vst_factory/build_broker.py` | `linux_compile_only` / factory + factory-audio QA |
| `vst_factory/model_router.py` | ModelBackend protocol + export-only Phase 1 |
| `vst_factory/slack_notify.py` | Optional Slack gate posts (no-op if unset) |
| `vst_factory/references.py` | CHORD ENGINE reference metadata |
| `static/` | Phase 1a Builder + logs UI |
| `schemas/factory-spec-v1.json` | Legacy spec contract |
| `schemas/factory-spec-v2.json` | Current project spec contract |
| `data/product-library.json` | Static SKU catalog (includes MeterTap) |
| `data/meter-tap-dogfood.json` | MeterTap intake prompt + fixture pointer |
| `data/liaison-status.json` | Snippet merged into `PASTE_TO_FACTORY.md` |
| `testing/certification-v1.json` | Certify hard-gate definitions |
| `scripts/complete_metertap.py` | Cross-platform MeterTap smoke (stdlib urllib) |
| `scripts/smoke_metertap.sh` | Bash wrapper for the Python smoke |

## MeterTap smoke

Dogfood SKU proving intake → architecture gate → portfolio/taxonomy (no scaffold by default).

```bash
# Terminal A
cd disklordz/vst-factory
source .venv/bin/activate
export VST_FACTORY_DB=/tmp/factory-smoke.sqlite
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790

# Terminal B
cd disklordz/vst-factory
source .venv/bin/activate
export VST_FACTORY_URL=http://127.0.0.1:8790
python scripts/complete_metertap.py
# or: bash scripts/smoke_metertap.sh
```

PowerShell:

```powershell
$env:VST_FACTORY_DB = "$env:TEMP\factory-smoke.sqlite"
$env:VST_FACTORY_URL = "http://127.0.0.1:8790"
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
# other shell:
python scripts/complete_metertap.py
```

`--through-scaffold` opts into `POST /scaffold` (writes `plugins/` + `vst/`). Default smoke stops after approving `approve_architecture` and checking portfolio/taxonomy so APC Dream/Plan/Design is not skipped.

## UI map (phases)

| Area | Phase |
|------|-------|
| AI Builder | 1a |
| Build Center | 1a |
| Plugin Lab | 1a |
| Source Editor (Monaco) | 1b |
| Visual Designer | 3 |

## Extension points

- New `ModelBackend` in `model_router.py`.
- Windows worker: extend `build_broker.py` with subprocess to local agent.
- Component library: `vst/factory-lib/` (Phase 4).
- Taxonomy categories: edit `modules/product-taxonomy.json`.
- Library SKUs: edit `data/product-library.json`.

## Related docs

- [docs/DISKLORDZ_VST_FACTORY.md](../../docs/DISKLORDZ_VST_FACTORY.md)
- [docs/DISKLORDZ_VST_FACTORY_PLATFORM.md](../../docs/DISKLORDZ_VST_FACTORY_PLATFORM.md)
- [docs/VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md)
- [vst-testing-ops/ARCHITECTURE.md](../../vst-testing-ops/ARCHITECTURE.md)
