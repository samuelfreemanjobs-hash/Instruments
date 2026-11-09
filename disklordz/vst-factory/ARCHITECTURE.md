# DiskLordz VST Factory

## Purpose

Local-first **AI development studio** for VST3 plugins: plain-English intake, APC-aligned project state, JUCE scaffold, compile, and QA — without conflating SaaS drum generation or the CHORD ENGINE DPF tree.

Operators: you. Implementers: Cursor Factory (`disklordz-audio-engineering-lane`) + this HTTP surface.

## Build & run

```bash
cd disklordz/vst-factory
python3 -m venv .venv && source .venv/bin/activate
pip install -e ".[dev]"
pytest tests -q
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
```

Env: `VST_FACTORY_DB` (default `factory_projects.sqlite` in package dir), `VST_FACTORY_REPO_ROOT` (auto-detected).

## Data flow

```text
Prompt + category
  → POST /api/projects (SQLite row + factory-spec/v1 JSON)
  → POST /api/projects/{id}/scaffold
       → plugins/<Name>/status.json + .ideas/creative-brief.md
       → tools/new-vst.sh
  → POST /api/projects/{id}/build
       → build_broker (linux: tools/build-vst.sh)
  → POST /api/projects/{id}/test
       → vst-testing-ops/run_business.py --profile factory --plugin <Name>
  → model_router (Phase 1: export Cursor prompt text to DB / API)
```

CHORD ENGINE reference: `GET /api/references/chord-engine` (no duplicate scaffold).

## Threading / realtime

HTTP only in this service. Generated C++ must follow [docs/dsp-architecture-specification.md](../../docs/dsp-architecture-specification.md) (no alloc/I/O in `processBlock`).

## Orchestration states

`intake` → `dream` → `plan` → `design` → `scaffold` → `impl` → `build` → `test` → `repair` → `ship`

Phase 1 automates: `intake`, `scaffold`, `build`, `test`, `certify` (API). Collaborative gates in SQLite: `approve_architecture` (blocks scaffold), `approve_dsp_promotion`, `approve_gui`, `approve_release`. Slack optional via `VST_FACTORY_SLACK_WEBHOOK_URL` — see [VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md).

Specs: `factory-spec/v2`. Portfolio: `GET /api/portfolio`, taxonomy: `GET /api/taxonomy`.

## Module library (stored templates)

Reusable JSON + markdown under `modules/` and `templates/` — loaded by `vst_factory/modules.py` and exposed on `/api/modules/*`.

| Path | Role |
|------|------|
| `modules/catalog.json` | Index of plugin classes, DSP, GUI, workflows |
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
| `vst_factory/db.py` | SQLite projects, runs, specs |
| `vst_factory/orchestrator.py` | State transitions, subprocess jobs |
| `vst_factory/apc_bridge.py` | APC `status.json` + brief stubs |
| `vst_factory/build_broker.py` | `linux_compile_only` / Windows doc paths |
| `vst_factory/model_router.py` | ModelBackend protocol + export-only Phase 1 |
| `vst_factory/references.py` | CHORD ENGINE reference metadata |
| `static/` | Phase 1a Builder + logs UI |
| `schemas/factory-spec-v1.json` | Shared project spec contract |
| `agents/*.md` | Role prompts for Phase 2 |
| `data/liaison-status.json` | Snippet merged into `PASTE_TO_FACTORY.md` |

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

## Related docs

- [docs/DISKLORDZ_VST_FACTORY.md](../../docs/DISKLORDZ_VST_FACTORY.md)
- [vst-testing-ops/ARCHITECTURE.md](../../vst-testing-ops/ARCHITECTURE.md)
