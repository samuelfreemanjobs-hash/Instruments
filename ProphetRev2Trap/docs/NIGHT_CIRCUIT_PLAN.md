# Night Circuit — product plan (living doc)

**Product name:** Night Circuit  
**Codename / CMake target:** `ProphetRev2Trap` (rename folder optional later)  
**Lane:** `[Plugin][JUCE]` · VST Plugin Factory

## Vision

A **Prophet Rev2–influenced**, **trap- and vaporwave-aware** poly synth whose factory library is organized by **mix role**. Strength: **synthetic** oscillators only — no acoustic samples.

## Milestone status

| Milestone | Scope | Status |
|-----------|--------|--------|
| **A** | Dual osc, amp + filter ADSR, category/preset UI, factory library | Shipped in PR #60 |
| **A.1** | **Synth** category + **Night Circuit** branding + taxonomy docs | This change |
| **B** | Full panel (cutoff, res, osc mix, detune, filt env amt); glide; unison | Planned |
| **C** | User presets; CLAP; voice count / CPU tiers | Planned |
| **D** | Prophet-depth (PWM, wavetable layer, sequencer hooks) | Research |

## How to make Night Circuit better (prioritized)

### 1. Sound engine (highest impact)

- **Unison / spread** on Synth category defaults (2–4 voices, controlled detune) — chords feel “expensive” without samples.
- **Glide / portamento** with legato (Pierre / vintage poly feel).
- **Oscillator mix per category:** Synth = more balanced saw+pulse; Bass = more osc1; Lead = brighter cutoff baseline.
- **Filter key tracking** so chords stay balanced across the keyboard.
- **Soft clip / drive** bus (Mike Will / Shawty Redd aggression) — one knob, post-filter.

### 2. Synth category content

- Target **8–12 Synth presets** before ship: vaporwave (slow attack, wide detune), Jeezy-style **stacked fifths**, Mike Dean **wide poly**, minimal **two-note tension** patches.
- **Avoid overlap:** move any preset that only works mono from Synth → Lead; long-only textures → Pad.

### 3. UX / preset manager

- Category dropdown shows **one-line role** (tooltip or subtitle): e.g. `Synth — poly chords & beds`.
- **Init** preset per category; **A/B** compare; host program change sync (already partial via `currentProgram` in state).
- Optional **favorites** row (user tags) in Milestone C.

### 4. Technical / ship readiness

- Add **ProphetRev2Trap** to `vst-testing-ops` smoke (pluginval only at first).
- **DSP tests:** envelope continuity, filter stability at high Q, denormals at silence.
- **CLAP** via existing `clap_juce_extensions` pattern from JD Upgraded.

### 5. Brand / SKU (Business Planner)

- `product_id`: e.g. `DL-NIGHT-CIRCUIT` when Marketing brief exists.
- Store story: “Poly synth for trap chords and neon beds — no samples.”

## Open questions (defaults if unanswered)

| Question | Default for now |
|----------|-----------------|
| Rename repo folder `ProphetRev2Trap/` → `NightCircuit/`? | Keep path; display name **Night Circuit** |
| MPE / microtonal? | No until Milestone D |
| Built-in arp? | No — DAW handles; revisit for Pierre lane |

## Success criteria (Milestone B)

- [ ] All taxonomy categories have ≥2 presets; **Synth ≥4**
- [ ] Chord voicing test: C minor 7 @ 3 velocities — no audible voice steal clicks at 16 notes
- [ ] UI exposes filter + osc macros used by Synth presets
- [ ] pluginval + standalone manual preset walkthrough recorded
