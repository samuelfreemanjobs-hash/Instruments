# Nebula6 — DPCS Phase 0 constitution

**Product:** Nebula6 · **Class:** polyphonic synthesizer · **Track:** APC greenfield (desktop primary, MPC derivative later)

## North star

Build a **remarkable** Juno-soul instrument for **cloud phonk, phonkwave, and atmospheric phonk**—not a Roland or Arturia facsimile. One demo motion must land in under 15 seconds: **Frozen Drift chorus bloom on a minor hold**.

## Non-negotiables

1. **Juno truth in the voice path** — per-voice DCO (saw/pulse/sub) + 24 dB LP + ADSR; no sample playback.
2. **Remarkable bus, not remarkable complexity** — post-sum **Nebula haze** (formant cloud) + **shimmer feedback** + **tape gravity**; one optional ghost overtone path for genre glue.
3. **No heap on the audio thread** — fixed voice pool; chorus/b FX sized at `prepareToPlay`.
4. **Genre-native defaults** — factory presets tagged Cloud / Phonkwave / Atmospheric; scale lock presets in minor modes.
5. **Shared core with Melodee lane** — extract `shared/dsp/juno/*` for MpcJuno106; Nebula6 adds bus + performance only on desktop v1.

## Success criteria (Dream)

| Test | Pass |
|------|------|
| Hero motion | User holds Cm triad, enables **Frozen**, increases **Drift** → audible width bloom without pitch wobble on fundamental |
| Identity | Blind A/B vs plain Juno chorus: listener identifies “more space/haze” on 3 factory presets |
| Stack | Sits beside `PhonkCowbell94` at conservative mix (Nebula loud, cowbell low) without mud in 120–300 Hz |
| CI | Offline render smoke: 3 presets, peak −24 to −0.1 dBFS, no NaN |

## Out of scope v1

- ROMpler, user wavetable import, MPE, full Arturia FX suite
- Aggressive Memphis cowbell lead (see PhonkCowbell94)
- 16+ voices, unison ×4+

## Related products

| SKU | Relationship |
|-----|----------------|
| MpcJuno106 | Shared Juno DSP; headless; no Nebula bus |
| PhonkCowbell94 | Stack partner (hard phonk) |
| CloudSupplyTrap | FM trap bells — different timbre lane |
| AnalogDreams84 | Bright synthwave — marketing opposite |

**Next phase:** `/apc-plan Nebula6`