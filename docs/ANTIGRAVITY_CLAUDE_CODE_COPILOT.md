# Antigravity + Claude Code (DPF copilot)

**Intent:** Claude Code owns **C++ / DPF implementation** in the terminal. Antigravity on **Windows** finishes the slow, parallel parts so Claude Code does not stall on builds, fixtures, or data prep.

*(If you meant **DPF** — DiskLordz Plugin Framework — this doc is the right lane. There is no separate “DPCS” package in-repo.)*

## Who does what

| Agent | DPF / coding role |
|-------|-------------------|
| **Claude Code** | `DPF-Core`, `DPF-MIDI`, JUCE glue, refactors in `Source/` / product tree — **primary author** |
| **Antigravity** | Windows **cmake build logs**, **pytest** on theory JSON, **MIDI fixtures**, **CMakeLists drafts**, **hpp stub markdown** from JSON — **never** merge into production C++ without Claude/Cursor review |
| **Cursor Factory** | Large slices, CI, PR review, `golden-wav-qa` |
| **plugin-factory-liaison** | Slices and `PASTE_TO_FACTORY` — no code |

```text
Claude Code (C++ slice)
        │ request-claude-support.sh
        ▼
claude-code-inbox/CC-*.json  →  inbox/HO-*.json
        │
        ▼  Windows: watch-handoffs.ps1
Antigravity: build · pytest · fixtures · drafts
        │
        ▼  outbox + outbox-artifacts/claude-code/
Claude Code (next turn): pull · continue C++
```

## Claude Code — ask Antigravity (one command)

After a slice that needs Windows or batch support:

```bash
./scripts/antigravity-bridge/request-claude-support.sh \
  --slice "DPF-MIDI TriadEngine — need pytest green on JSON fixtures" \
  --kind pytest_run \
  --dpf DPF-MIDI \
  --path docs/CHORD_ENGINE_PRD.md \
  --dispatch --push
```

**Kinds:** `windows_build` · `pytest_run` · `codegen_stub` · `midi_fixture` · `cmake_scaffold`

Standing jobs: `disklordz/antigravity/claude-code-support-catalog.json` (`WO-CC-001` … `005`).

## Automated dispatch

Daily product prep workflow also runs:

```bash
python3 scripts/antigravity-bridge/sync-claude-code-support.py --merge-only --dispatch --push
```

Or everything:

```bash
./scripts/antigravity-bridge/sync-all-antigravity.sh --push
```

## Antigravity — return work to Claude Code

```powershell
.\scripts\antigravity-bridge\publish-handoff.ps1 `
  -WorkOrderId WO-CC-002 -Status done `
  -FromAgent antigravity-dpf-support -ToAgent claude-code `
  -ArtifactPath disklordz/antigravity/outbox-artifacts/claude-code/pytest-music-theory.log `
  -Notes "pytest 42 passed"
git add -A; git commit -m "chore(dpf): Claude support artifacts"; git push
```

Claude Code: `git pull`, read log + drafts under `docs/drafts/dpf/`, continue implementation.

## Sprint alignment

When Coach hits **theory-engine**, **plugin-skeleton**, or **midi-output**, product prep + `WO-CC-*` jobs are prioritized together ([ANTIGRAVITY_PRODUCT_FACTORY.md](ANTIGRAVITY_PRODUCT_FACTORY.md)).

## Rules

- Antigravity **does not** replace Claude Code for core `dpf_cpp` — see `product-routing.json` → `claude_code_support`.
- Draft C++ lives only under `docs/drafts/dpf/` until Claude Code copies into the real tree.
- You: approve once, playtest in DAW; do not hand-run cmake unless Antigravity is offline.

## Related

- [DPF_TECHNICAL_ARCHITECTURE.md](DPF_TECHNICAL_ARCHITECTURE.md)
- [CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md)
- [ANTIGRAVITY_PRODUCT_FACTORY.md](ANTIGRAVITY_PRODUCT_FACTORY.md)
