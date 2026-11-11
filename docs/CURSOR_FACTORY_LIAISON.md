# Cursor Factory Liaison — `plugin-factory-liaison`

**Fleet agent:** [`plugin-factory-liaison`](../disklordz/agents/profit/plugin-factory-liaison/agent.md)  
**Implements code:** separate **Cursor Cloud** run with [disklordz-audio-engineering-lane](../.cursor/rules/disklordz-audio-engineering-lane.mdc)  
**Business sprint:** [`audio-business-copilot`](../disklordz/agents/profit/audio-business-copilot/agent.md)

## Purpose

When **Claude Code is paused** or you run two Cursor chats, the liaison is the **PM for the plugin factory**: it reads specs and CI, writes the next **single** Factory task, and opens GitHub issues — it does **not** write JUCE.

## Division of labor

```text
Spark (chat) ──PASTE_LIAISON──► plugin-factory-liaison (this agent)
                                        │
                    ┌───────────────────┴───────────────────┐
                    ▼ PASTE_TO_FACTORY                     ▼ request-claude-support.sh
             Cursor Cloud Factory (C++ / JUCE)      Antigravity (DPF build · pytest · fixtures)
             Claude Code (terminal DPF) ◄──outbox──┘
                    │
                    ▼ PR / push
             golden-wav-qa (run_business.py --profile ci)
                    │
                    ▼ red/green
             plugin-factory-liaison (triage → next slice)
```

Antigravity + Claude Code: [ANTIGRAVITY_CLAUDE_CODE_COPILOT.md](ANTIGRAVITY_CLAUDE_CODE_COPILOT.md).

## Liaison MAY

- Read entire repo; grep; run **read-only** verification (`cmake -B`, `run_business.py` when toolchain exists)
- Edit **docs/**, issue bodies, `disklordz/audio-command/data/progress.json` when user confirms
- Create/update GitHub issues with prefix `[Plugin][CHORD]` or `[Plugin][DPF]`
- Summarize PR/CI for Samuel in §18.3 shape

## Liaison MUST NOT

- Modify `Source/`, `Products/`, `DPF-*`, `Wave9090/`, `SP1200/` C++ without explicit OVERRIDE for docs-only paths
- Ship multi-feature “mega prompts” — one slice per Factory run

## Every liaison response format

1. **Status** — sprint item, branch/PR if any  
2. **Blocker** — `human-windows` | `factory` | `none`  
3. **Next owner** — Samuel | Factory | golden-wav-qa  
4. **PASTE_TO_FACTORY** — full prompt (only if Factory is next)  
5. **Done when** — cites `docs/CHORD_ENGINE_PRD.md` or roadmap §18 stage  

## Factory prompt template (liaison fills blanks)

```text
DiskLordz Audio — Factory implementer only.

Stage: {A|B|C|… from roadmap §18.2}
Slice: {one sentence}
Read: docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md §18, docs/DPF_TECHNICAL_ARCHITECTURE.md, docs/CHORD_ENGINE_PRD.md
Out of scope: {explicit}

Acceptance:
- {bullet}
- Build: cmake … and artifact path
- Tests: {ctest/pytest names if any}

Report §18.3: Completed, Files, Build, Tests, Limitations, Next task.
```

## Spark / human paste label

Gemini Spark uses **`PASTE_LIAISON`** (not `PASTE_CURSOR`) for engineering queue work → paste into Cursor Cloud with skill **`disklordz-plugin-factory-liaison`**.

## VST Factory HTTP (greenfield APC)

When the DiskLordz VST Factory service is running (`uvicorn` on `:8790`):

- `POST /api/projects/{id}/sync` — pull APC `status.json` / gate results into factory state  
- `GET /api/projects/{id}/pipeline` — 10-stage strip + blocking gate + next SOP  
- Gates: `approve_architecture` → scaffold; `approve_gui` → accept impl; `approve_release` → ship  

See [disklordz/vst-factory/ARCHITECTURE.md](../disklordz/vst-factory/ARCHITECTURE.md) and [DISKLORDZ_VST_FACTORY_PLATFORM.md](DISKLORDZ_VST_FACTORY_PLATFORM.md).

## Current gate (`plugin-skeleton`, 2026-11)

**Blocker `human-windows`:** Stage B JUCE shell is on **`main`**. Samuel proves VST3 in a DAW — **do not** open Factory for greenfield CMake.

| Path | Action |
|------|--------|
| `PASTE_TO_FACTORY.md` | Build script + optional **DiskLordz Audio (Windows VST3)** artifact |
| Operator tiers | [CURSOR_OPERATOR_ON_20_PRO.md](CURSOR_OPERATOR_ON_20_PRO.md) |
| Factory chat | Only after a **failed** local or CI build (paste log + this file) |

## Related

- [DISKLORDZ_MASTER_PLAN.md](DISKLORDZ_MASTER_PLAN.md)  
- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)  
- [GEMINI_SPARK_DAILY_PROMPT.md](../disklordz/audio-command/docs/GEMINI_SPARK_DAILY_PROMPT.md)
