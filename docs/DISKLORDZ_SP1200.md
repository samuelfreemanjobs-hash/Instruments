# DISKLORDZ — SP-1200 VSTi (project summary)

## Purpose

Circuit-modelled **E-MU SP-1200** drum sampler as a **commercial** VST3 / AU / CLAP plugin for beatmakers who want 12-bit SP character without hardware ($7k–$10k).

**Price target:** $49–$149  
**DSP:** Pure C++17, framework-agnostic (~6,400+ lines, 37 source files in the external DISKLORDZ repo)  
**Intended repo (external):** `github.com/Samuelfreemansjobs/DISKLORDZ` — not yet visible on GitHub from CI agents; may be private or under a different org slug.

## Framework decision (updated for this monorepo)

| Context | Recommendation |
|---------|----------------|
| **Standalone DISKLORDZ repo (Phase 0 doc)** | iPlug2 (MIT) — no GPL, no JUCE subscription on day zero |
| **Instruments / DiskLordz Labs monorepo (this repo)** | **JUCE** — same as JD Upgraded: one CMake graph, CLAP/VST3 CI, closed-source shipping pattern |

The DSP (signal chain, sampler, sequencer) **does not depend on iPlug2 or JUCE**. Only the **plugin scaffold**, parameter bridge, and UI do. Switching to JUCE at scaffold time is **one integration layer**, not a rewrite.

**Stay on iPlug2** only if SP-1200 ships as a **separate repo/product** with its own release train and you want zero JUCE licence coupling. **Move to JUCE** when merging into this monorepo or when you want shared tooling with JD Upgraded (OfflineRender-style harness, golden tests, CLAP parity).

## Build status (~40% complete)

| Module | State | Tests |
|--------|--------|-------|
| Signal chain (7 files) | Verified | 85 |
| SamplerEngine | Verified + ASan | 26 |
| Sequencer — MusicalTime | Verified | 26 |
| Sequencer — SwingTransform | Verified | 14 |
| Sequencer — SegmentStore | Verified + ASan + TSan | 30 |
| Sequencer — SegmentRecorder | Verified + ASan + TSan | 24 |
| Sequencer — SongPlayer | Compiles, **no suite** | — |
| Sequencer integration | Not started | — |
| Plugin scaffold | Not started | — |
| MixMatrix | Not started | — |
| UI | Not started | — |

**Done gate:** `run_verify.sh` — builds to `build/`, runs all suites (18 today).

## Acoustic lock-in (six circuit behaviours)

1. **26,041.66 Hz** internal rate (not 27.5 kHz marketing)
2. **12-bit** round-to-nearest quantization (~74 dB noise floor)
3. **Integer skip/repeat** pitch (no interpolation)
4. **Zero-order hold ×4** — images above 13 kHz; Ch 7–8 unfiltered
5. **SSM-2044 ZDF ladder VCF** on Ch 1–2 (tanh, cutoff 12 kHz → 350 Hz)
6. **Three output filter types** — VCF / 7.5 kHz LP / 10 kHz LP / bypass

Corrections **C1–C16** are numbered in `docs/SIGNAL_CHAIN.md` in the DISKLORDZ repo.

## Immediate next task

**SongPlayer** — add `tests/test_songplayer.cpp`, wire `run_verify.sh`:

- Repeat count = **N total**, not N extra
- Most recent `begin-repeat` wins when nested
- Sub-song depth **4** with stack overflow protection
- **C16:** repeat state restored on return from sub-song
- `songEnded` means **stopped**, not merely hitting an End step

## Open items

- Tune by ear vs hardware: VCF cutoff/floor/τ, AA filter, two LP cutoffs
- SampleLibrary: retired-buffer list full (32) — needs GC
- **MixMatrix** missing — Multilevel level multiplier (**C10**) has no sink
- **Block splitting** must live in the **audio callback** (or jitter ~10 ms)

## Agent / MCP (DISKLORDZ repo)

When that repo is checked out: `.cursor/mcp.json` (Airtable + GitHub tokens), **AGENTS.md** (traps), `docs/SIGNAL_CHAIN.md`, `docs/DPCS.md`.

This monorepo copy: see [AGENTS.md](../AGENTS.md) and [CLAUDE_MCP.md](CLAUDE_MCP.md).

## Related

- [ARCHITECTURE.md](../ARCHITECTURE.md) — repo index  
- [disklordz-factory/ARCHITECTURE.md](../disklordz-factory/ARCHITECTURE.md) — Records / Supply / **Labs** flywheel  
- Hardware reference tree (optional): `/workspace/sp-1200` (EMU-SP1200 fork, KiCad/BOM — not the VSTi DSP repo)
