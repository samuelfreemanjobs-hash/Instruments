# Nebula6 — creative brief

**Elevator pitch:** *Nebula6 is the Juno that only exists after midnight—chorus that freezes into infinite cloud, tape drift welded to BBD motion, and a formant “nebula” that turns minor chords into phonkwave cathedrals.* It is the **remarkable** take on Jun-6 you reach for when cloud phonk, phonkwave, or atmospheric phonk needs **width and grief**, not brass stabs and yacht-rock presets.

**CODE (planned):** `Nb6s` · **Suite:** DiskLordz phonk atmosphere (sibling to Play 6-pack, **not** a Play skin clone)

---

## Why this exists (and what makes it remarkable)

| Ordinary Juno clone | **Nebula6** |
|---------------------|-------------|
| Chorus I/II and done | **I · II · Drift · Frozen** — Drift couples wow to chorus rate; **Frozen latches BBD phase** for infinite pad bloom without stacking voices |
| Generic preset bank | **4 Cloud + 4 Phonkwave + 4 Atmospheric + Init** — every patch is a genre lane, macro-initialized |
| Dry filter sweep | **Hollow** — parallel notch send on the Juno LP for drift leads without leaving Junoland |
| Isolated instrument | **Stack lore** — designed to sit **under** `PhonkCowbell94` and **beside** `CloudSupplyTrap` (mix notes in preset names) |
| Static synth | **Performance spine** — scale-locked chords (Cm, F♯ Phryg, D Dorian), dotted arp, long legato glide, **MIDI pattern export** for your loop workflow |

**Hero motion (marketing + QA):** Hold **Cm7** → **Frozen ON** → ride **Drift** and **Sky** → pad blossoms; fundamental stays stable, stereo field and haze grow. Under 15 seconds, unmistakable.

---

## Sonic identity

### Plane A — “106 Core” (per voice)

- **DCO:** PolyBLEP saw + PWM pulse + −1 oct square sub; thermal **micro-drift** (optional, macro-driven).
- **Filter:** Classic **24 dB/oct** Roland-style LP; resonance tame but musical to self-oscillate edge.
- **Amp:** ADSR; slow-attack cloud presets bias Attack > 400 ms.

### Plane B — “Nebula Haze” (post-sum stereo bus)

Single fused texture—not a Kontakt stack:

1. **Formant cloud** — 5-band vowel morph (choir-without-samples), level from **Nebula** macro.
2. **Crystal air** — single FM partial cluster (fixed ratio, not full DX7)—adds “cloud trap” glass at low mix.
3. **Ghost overtone** (optional, **Drift > 0.5**): bandpassed **808-ratio ghost** (very low level)—genre glue, not a second lead.

### Remarkable FX strip (post Nebula merge)

| Stage | Character |
|-------|-----------|
| **Chorus** | I / II / **Drift** (rate ↔ wow) / **Frozen** (latch LFO phase) |
| **Shimmer** | Short feedback pitch-shifted tail (cloud phonk halo) |
| **Tape gravity** | Wow/flutter + soft clip + gentle HF loss |

---

## Genre lanes (preset philosophy)

### Cloud phonk

Wide, wet, bell-optional; sub as **bed**. Macros: **Sky ↑ Drift mid Nebula ↑**; ghost overtone off or barely on.

### Phonkwave

Minor doronian drives; **Chorus II + Drift**; dotted arp enabled; PWM mid. Neon cyan **visual** nods, **sonic** nod is dark not Miami.

### Atmospheric phonk

**Frozen** friendly; **Hollow** sweeps; long releases; **Nebula** dominates over DCO level; horror-adjacent without sample choir.

---

## User stories

1. As a **phonk producer**, I hold a minor chord and widen the mix without adding a second plugin so I can stack cowbell on top later.
2. As a **phonkwave artist**, I enable dotted arp + scale lock and get genre-correct notes without theory friction.
3. As a **sound designer**, I latch **Frozen** and automate **Hollow** for a filter documentary moment in a cinematic drift edit.
4. As **factory CI**, I run render smoke on **Cloud / Wave / Atmos** presets and get bounded peak + no NaN.
5. As an **MPC user later**, I expect a headless **MpcNebula6** that shares Juno core with **K1/K2/K3 = cutoff / nebula / chorus**.

---

## Visual vibe

**Obsidian void + cyan nebula mist + violet edge lighting** (CloudSupplyTrap family, **not** Arturia beige). UI: **Play-compatible APVTS** where practical (`xy_x/y`, macros, arp, dice) with **custom chrome**:

- **XY “Night sky”:** X = **Darkness** (cutoff + Hollow), Y = **Sky** (chorus send + shimmer).
- **Chorus mode strip** always visible (I | II | Drift | Frozen).
- **Dice:** randomizes within current **genre tag** only (stay in Cloud lane).

Framework decision deferred to Plan; lean **WebView Play shell** for parity with 6-pack factory, or **native** if GTK/WebView cost blocks ship—Plan must choose.

---

## Reference audio

Drop references in `plugins/Nebula6/references/`:

- Slowed + reverbed Juno pad (phonk bed)
- Cloud trap instrumental stem (space, not drums)
- Drift phonk without dominant cowbell

When present, write `.ideas/reference-analysis.md` (spectrum: HF air, chorus width, sub occupancy).

---

## Dependencies & honesty

- **Inherits** performance + macro patterns from `templates/juce8-synth-base`.
- **Shares** Juno oscillators/filter/chorus with `MpcJuno106` after extraction to `shared/dsp/juno/`.
- **Does not** replace `PhonkCowbell94`, `CloudSupplyTrap`, or `AnalogDreams84`—completes the **atmosphere** vertex of the triangle.

---

## Anti-goals

- 1:1 Arturia Jun-6 UI clone
- Bright 80s pop factory bank
- Sample-based strings or choirs
- 8-oscillator supersaw gimmick