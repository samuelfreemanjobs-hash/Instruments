# DiskLordz VST Factory

AI-powered local-first orchestration for JUCE/VST3 plugins. This is **not** the drum-kit API in `disklordz/website/src/lib/generation/factory.ts`.

## Start here

| Doc | Role |
|-----|------|
| [disklordz/vst-factory/ARCHITECTURE.md](../disklordz/vst-factory/ARCHITECTURE.md) | Product build, data flow, API |
| [docs/apc-workflow.md](apc-workflow.md) | APC dream→ship lifecycle for generated SKUs |
| [docs/CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md) | PM agent (no C++) |
| [docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md](DiskLordz_Audio_Master_Roadmap_for_Cursor.md) | CHORD ENGINE §18 stages |

## Run (Phase 1)

```bash
cd disklordz/vst-factory
python3 -m venv .venv && source .venv/bin/activate
pip install -e .
uvicorn vst_factory.main:app --reload --port 8790
```

Open `http://127.0.0.1:8790/` — **AI Builder** + **Build Center** + **Plugin Lab** (static shell).

## Product paths

| Track | Location | When |
|-------|----------|------|
| Greenfield APC plugins | `plugins/<Name>/` + `vst/<Name>/` | Factory scaffold + build |
| CHORD ENGINE (Milestone 001) | `DiskLordz-Audio/Products/ChordEngine/` | Reference / liaison queue only |

## QA

```bash
python3 vst-testing-ops/run_business.py --profile factory --plugin <PascalCaseName>
```

## Agent roles (Phase 2+)

Prompt stubs live in `disklordz/vst-factory/agents/`. Phase 1 uses one orchestrator; roles map to APC phases and §18.

## Build broker

| Mode | Use |
|------|-----|
| `linux_compile_only` | Cloud / Linux `tools/build-vst.sh` |
| `local_windows` | `DiskLordz-Audio/build-windows.ps1` or VS on dev box |
| `github_actions` | **DiskLordz Audio (Windows VST3)** workflow artifact |

Human approval required before copying `.vst3` into system plugin folders.
