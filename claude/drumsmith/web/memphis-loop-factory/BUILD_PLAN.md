# Factory OS — Claude Code × Antigravity × Cursor build plan

## Product thesis

**Commercial deliverable = original drum sample kits** (Converter Night / DRUMSMITH oneshots + loops). The Factory OS and MEMPHIS-660 Gem/CLI are generators that feed that kit pipeline — not a text-to-audio DAW replacement.

Hybrid generator, not a giant audio model:

**MIDI/Pattern Intelligence → Groove Engine → DDSP Drum Synth → SP-style DSP → Loop Renderer → Critic**

Killer UX: **Lock + Mutate** (“keep kick/snare; give 20 lazy Memphis hat/perc variations”).

## What already exists

| Asset | Location | Status |
|-------|----------|--------|
| Monolithic Factory OS UI + audio | `index.html` (this folder) | Staged prototype |
| Modular DR-660 lab | `../dr660-lab/` | Sequencer, ratchets, cowbell, bitcrush, cassette, wow/flutter, tape delay, 4-bar JSON |
| Converter Night research / kits | `kits/converter-night/` | Archetypes, competitor notes |
| Native DRUMSMITH renderer | `claude/drumsmith/native` | Kit renders (separate SKU path) |

## Lane ownership

| Lane | Owns | Does not own |
|------|------|--------------|
| **Cursor** | Merge Factory features into `dr660-lab` ES modules; PR/CI; keep Web Audio realtime-safe | Full Windows packaging |
| **Claude Code** | Headless mutate CLI, OfflineAudio contracts, future C++/DPF groove engine | Product-prep batch WAV banks |
| **Antigravity** | MIDI fixtures, pattern JSON pytest, packaging scripts, Windows smoke docs | Production DSP merges without review |

## Phase slices (ordered)

### P0 — Prototype truth (done / this PR)

1. Stage Factory OS HTML under `memphis-loop-factory/`
2. Publish `ARCHITECTURE.md` + `meta-prompt.md` + this plan
3. Keep `dr660-lab` as modular DSP source of truth for incremental ports

### P1 — Cursor: Lock + Mutate into `dr660-lab` ✅ (lab)

1. Per-track `locked` flags in sequencer state
2. `mutateUnlocked(rng)` regenerates unlocked rows (chh ratchets, cowbell pitch, perc) preserving locked kick/snare
3. UI: Lock toggles + “Mutate Unlocked” button on the rack
4. Unit tests for mutate invariants (locked rows unchanged)

Still open: port richer Factory OS mutate heuristics from `index.html` `generatePatterns()`.

### P1b — MEMPHIS-660 kick oneshot CLI ✅

1. Gem prompt: `claude/drumsmith/docs/GEM_MEMPHIS_660_KICK.md`
2. Python Engine B: `scripts/memphis_oneshots/phonk_kick.py` (12-bit / 26.04 kHz + tape LPF + tanh clip → 24-bit WAV)
3. Lab `DrumEngine` kick upgraded to click + body + sub stack

### P1c — MEMPHIS-DR660 snare oneshot CLI ✅

1. Gem prompt: `claude/drumsmith/docs/GEM_MEMPHIS_DR660_SNARE.md`
2. Python Engine B: `scripts/memphis_oneshots/phonk_snare.py` (membrane + BP wires + flam → 12-bit/26.04 kHz → tape → soft clip → 24-bit WAV)
3. Lab `DrumEngine` snare upgraded to acoustic triad

**Product framing:** outputs feed an **original drum sample kit** SKU (oneshots + loops). Artist names stay internal research-only; `release_ready` stays false until HITL.

### P2 — Cursor + Claude Code: Offline loop render

1. `OfflineAudioContext` 2-bar render with master FX chain
2. Download 16-bit PCM WAV (+ optional stems)
3. Headless Node/CLI twin for CI (`render_loop.py` or worklet-free offline path)
4. Batch kick/snare oneshots into Converter Night RAW banks via Antigravity

### P3 — Antigravity: fixtures & packaging

1. Emit MIDI Type-0 fixtures from pattern JSON
2. Pytest: swing math, ratchet counts, mutate lock invariants
3. Product-prep zip layout for “Loop Factory OS preview” (HTML + docs only)

### P4 — Critic / evaluator

1. Deterministic bounce score (kick density, hat roll rate, swing, ghost snares)
2. Optional Gemini/HTTP critic behind a feature flag (no secrets in repo)

### P5 — Native / VST (later)

1. Only after browser Lock+Mutate + offline export are green
2. Claude Code DPF/JUCE if product chooses DAW surface; otherwise stay web + CLI

## Antigravity work order (suggested)

```bash
./scripts/antigravity-bridge/antigravity-bridge.sh send \
  --wo WO-MLF-001 \
  --title "Memphis Loop Factory: MIDI fixtures + pattern pytest" \
  --to-agent antigravity-batch \
  --notes "From claude/drumsmith/web/memphis-loop-factory/BUILD_PLAN.md P3; fixtures under kits/converter-night/loops/; pytest swing/ratchet/mutate lock invariants; no copyrighted PCM." \
  --push
```

Claude Code support (when OfflineAudio CLI exists):

```bash
./scripts/antigravity-bridge/request-claude-support.sh \
  --slice "Loop Factory Offline WAV — pytest fixtures + Windows smoke" \
  --kind pytest_run \
  --path claude/drumsmith/web/memphis-loop-factory/BUILD_PLAN.md \
  --dispatch --push
```

## Definition of done (MVP Factory OS)

- [ ] Lock+Mutate in modular lab with tests
- [ ] Offline 2-bar WAV export (browser)
- [ ] MIDI Type-0 export of active pattern
- [ ] Antigravity fixture pack + pytest green
- [ ] Meta-prompt stays in sync with shipped pipeline
- [ ] No copyrighted sample banks shipped

## Related

- `meta-prompt.md` · `ARCHITECTURE.md`
- `../dr660-lab/ARCHITECTURE.md`
- `docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md`
