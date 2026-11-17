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
| Windows runner | `disklordz/vst-factory/scripts/run-factory-play6.ps1` (template build → pluginval → `--render-test` → optional Claude SKU pass) |
| **Linux / bash + Claude Code** | `disklordz/vst-factory/scripts/run-factory-play6.sh` (`--skip-claude`, `--only PhonkCowbell94`, `--with-webview` for GTK) |
| Windows one-time setup | `disklordz/vst-factory/scripts/setup-play-factory-windows.ps1` |
| Linux CI template smoke | `tools/build-play-template-smoke.sh` |
| Pre-wired core | `templates/juce8-synth-base/` (`LayerAEngine`, `LayerBEngine`, `MacroRouter`, `Assets/`, render smoke) |

## PlaySynth Producer Suite (pure synth, Kontakt Play-style)

| Doc | Path |
|-----|------|
| 6 SKUs index | `plugins/PLAY_SYNTH_6PACK.md` |
| XY, Dice, macros | `plugins/PLAY_SYNTH_SHARED_UI.md` |
| Compile scaffold | `templates/juce8-synth-base/` |

**Phonk:** `PhonkCowbell94` (`Pc94`) — 808 cowbell ratio synth, cassette macros, 12 phonk presets (APC + queue).

## Melodee / MPC lane (Play 6-pack + classic line)

| Asset | Path |
|-------|------|
| Play **MPC** 6-pack index | `plugins/MPC_PLAY_SYNTH_6PACK.md` |
| MPC Play manifest | `disklordz/vst-factory/data/mpc_play_synth_6pack_master.json` |
| Headless contract | `disklordz/vst-factory/data/mpc_play_factory_contract.md` |
| Template (reuses `juce8-synth-base` DSP) | `templates/mpc-play-synth-base/` |
| Linux batch + Claude | `disklordz/vst-factory/scripts/run-factory-mpc-play6.sh` (alias: `tools/run-factory-mpc-play6.sh`) |
| Finish all `vst/*` ship | `tools/finish-all-vst3.sh` · matrix `docs/VST3_FINISH_MATRIX.md` |
| Verify bundle | `scripts/vst/verify-mpc-sample-vst3.sh` |
| General MPC rules | `docs/MPC_SAMPLE_VST3.md` |

Same Layer A/B specs as `play_synth_6pack_master.json`, but output is **headless** `vst/MpcPlay*` (not WebView folders at repo root).

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