# Nebula6 — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design Nebula6`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 2 | 8-voice fixed pool (12 max prepare-time flag) |
| Oscillators/voice | 2 | Saw + pulse + sub + drift; bus FM is post-sum not per-voice stack |
| Filters | 2 | Juno 24 dB LP + parallel Hollow notch |
| Modulation | 2 | PerformanceEngine (arp, chord, scale, glide) + chorus LFO modes |
| FX | 2 | Chorus chain + tape + shimmer (no IR verb v1) |
| Surfaces | 3 | WebView Play shell + 13 presets + MIDI export + macro/dice |
| Targets | 2 | Desktop VST3/standalone v1; future MpcNebula6 aarch64 |
| **Total** | **15** | |

**Tier:** **L3** (Expert) — driven by **surfaces + dual-target shared core**, not raw voice count.

**Risk flags:**

- **Frozen chorus:** LFO phase latch must not de normalize states — test at mode switch (see sonic-proof A).
- **Shimmer feedback:** hard cap mix and feedback; fail CI if peak > −0.1 dBFS on render smoke.
- **Nebula bus + ghost:** cap `ghost_overtone` energy in `MacroRouter` when `macro8` Weight high (avoid mud with PhonkCowbell94 stacks).
- **Shared juno extract:** Nebula6 impl **blocked** until `shared/dsp/juno/` exists or MpcJuno106 Dsp stub landed — track in design gate.
- **WebView Linux CI:** use `PLAY_SYNTH_HEADLESS_CI=ON` pattern for factory; full UI on Windows primary.

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `webview` |
| Decision | Play 6-pack shell (`templates/juce8-synth-base`) with **Nebula6 Assets** (obsidian/cyan/violet) |
| Rationale | Reuse `PerformanceEngine`, XY, Dice, macro lock, scope FIFO; chorus **mode strip** is custom HTML widget |
| Fallback | If WebView blocked: ship **L2 native** chorus strip + defer XY to v1.1 (document in design-spec) |

## Implementation strategy (order)

1. **Extract** `JunoDco`, `JunoLpf24`, `JunoChorus` → `shared/dsp/juno/` from MpcJuno106 architecture (or implement once, consume twice).
2. **`/apc-design Nebula6`** — layout, preset rows, macro curves, Assets wireframe, `params.json` schema.
3. **`/apc-impl`** — copy `juce8-synth-base` → `Nebula6/`, replace Layer A/B with `NebulaEngine` path; keep PerformanceEngine + WebView.
4. Implement **remarkable path first:** `NebulaChorus` (Frozen/Drift) + `NebulaBus` before polish presets.
5. **12+1 presets** from parameter-spec; tag metadata for Dice.
6. **`/apc-test`** — sonic-proof A–E + `run_business.py --profile factory --plugin Nebula6` when registered.
7. **`/apc-ship`** — `tools/package-vst.sh Nebula6` (after path finalized: repo root vs `vst/` — **design** locks layout).

## Factory / agent hooks

| Asset | Path (planned) |
|-------|----------------|
| Compact manifest | `disklordz/vst-factory/data/nebula6_master.json` |
| Bash runner | extend `run-factory-play6.sh --only Nebula6` OR dedicated `run-factory-nebula6.sh` after impl |

## Gate before implement

```bash
./tools/apc-gate.sh Nebula6 impl   # requires design_complete
```

## Parked neighbors

| SKU | Note |
|-----|------|
| SemV | Park — not phonk atmosphere |
| Arturia Jun-6 clone | Nebula6 **is** the Juno creative product |
| AnalogDreams84 | Bright wave; Nebula6 owns **dark** phonkwave default |