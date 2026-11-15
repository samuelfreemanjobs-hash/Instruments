# VST AI factory — what this repo already provides

Use this map instead of generic web articles (MarkTechPost, etc.). Everything below is **in-tree** or documented for **Cursor + Claude Code** on this monorepo.

## Agent & rules (shared by Cursor and Claude Code)

| Asset | Path |
|-------|------|
| Monorepo agent charter | `AGENTS.md` |
| JUCE 8 / RT safety (Windows desktop) | `docs/JUCE8_VST3_PROJECT_RULES.md` |
| Claude terminal mirror | `docs/CLAUDE_VST_ENGINEER.md` |
| APC lifecycle (Dream → Ship) | `docs/vst-lifecycle.md`, `.agents/skills/` |
| Existing Rev2 DSP patterns | `ProphetRev2Trap/Source/ProphetRev2Engine.h` |

## DiskLordz VST Factory (HTTP + APC gates)

```bash
cd disklordz/vst-factory && pip install -e ".[dev]" && uvicorn vst_factory.main:app --port 8790
```

- Orchestration: `disklordz/vst-factory/ARCHITECTURE.md`
- Linux build hook: `vst_factory/build_broker.py` → `./tools/build-vst.sh`
- QA profile: `vst-testing-ops/run_business.py --profile factory`

## Overnight batch queue (JSON prompts)

| Queue | Path |
|-------|------|
| Full roadmap (Prophet, Oxford, Play 6-pack, stubs…) | `disklordz/vst-factory/data/overnight-queue.json` |
| **Play 6-pack compact contract** | `disklordz/vst-factory/data/play_synth_6pack_master.json` |
| Build contract (arp, macro lock, 2x OS, FIFO viz) | `disklordz/vst-factory/data/play_synth_factory_contract.md` |
| Windows runner | `disklordz/vst-factory/scripts/run-factory-play6.ps1` |

## PlaySynth Producer Suite (pure synth, Kontakt Play-style)

| Doc | Path |
|-----|------|
| 6 SKUs index | `plugins/PLAY_SYNTH_6PACK.md` |
| XY, Dice, macros | `plugins/PLAY_SYNTH_SHARED_UI.md` |
| Compile scaffold | `templates/juce8-synth-base/` |

**Phonk:** `PhonkCowbell94` (`Pc94`) — 808 cowbell ratio synth, cassette macros, 12 phonk presets (APC + queue).

## Melodee / MPC lane (separate from desktop factory)

- Headless `vst/Mpc*` plugins, `./tools/build-vst.sh`, `docs/MPC_SAMPLE_VST3.md`
- Not the same CMake tree as JUCE 8 WebView desktop targets at repo root

## What is NOT in this repo (bring your own)

- `run-factory.ps1` for arbitrary 40+ SKU mega-queue (use `overnight-queue.json` + your own runner)
- Pre-pulled JUCE 8 under `templates/juce8-synth-base/JUCE/` (use parent `vst/JUCE` submodule or CPM on Windows)
- MarkTechPost / MCP tutorials (optional; JUCE docs MCP is an add-on on your machine)
- `pluginval.exe` at `tools/pluginval.exe` (install locally; runner skips if missing)

## Suggested workflow

1. **Plan:** APC artifacts under `plugins/<Name>/` (done for Play 6-pack).
2. **Scaffold:** Copy `templates/juce8-synth-base` → `<Name>/` at repo root.
3. **Implement:** Cursor in Linux CI for DSP; Claude Code on Windows for MSVC + WebView.
4. **Verify:** `pluginval` + `vst-testing-ops` when a target exists.
5. **Batch:** `run-factory-play6.ps1` on Windows with Claude CLI, or paste prompts from `play_synth_6pack_master.json`.