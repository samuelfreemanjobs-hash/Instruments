# Memphis 808 snare — systematic creation

Summary of how 1990s Memphis producers treated the TR-808 snare and how to recreate it in a modern chain. Pairs with **MEMPHIS-DR660** Gem and `index.html` snare engine.

## Elements

| Layer | Source | Frequency / timing | Role |
|-------|--------|-------------------|------|
| Dual-resonant body | TR-808 snare circuit (or DR-660 ROM) | ~180–330 Hz fundamentals | Wooden knock, above sub-808 |
| Noise burst | 808 noise → HPF/BP | 1.5–5 kHz | Brittle wire slap |
| Flam transient | 808 clap or rim, delayed | +2–8 ms vs body | Wider, harder attack |

## What 1990s producers did

1. **Boss DR-660 / DR-5** — 16-bit PCM 808 rips (not smooth analog 808).
2. **Pitch up +2 to +7 st** — shorter sample, sharper whip.
3. **Snare + clap same step** — hardware jitter → accidental flam.
4. **Tascam PortaStudio** — hot preamps, ferric tape harmonics, HF loss above ~10–12 kHz.
5. **Decay clamp / gating** — tail killed in 120–180 ms.
6. **Dark reverb** — Midiverb-style, band-limited under ~6 kHz when used.

## Systematic DAW chain (matches repo snare engine order)

```text
[808 snare body] ──┐
[808 clap +3–5 ms] ┘ → sum → HPF 105 Hz → ZOH decimate 26.04 kHz → quantize 12-bit
    → LP 11.5 kHz → tanh clip (+5.5 dB typical) → normalize −0.3 dBFS
```

### Step-by-step

1. **Layers:** Dry 808 snare + clap nudged 3–6 ms; pitch both +2–4 st.
2. **Pre-EQ:** HPF 18 dB/oct @ 105 Hz; notch −3 to −5 dB @ 450–550 Hz (Gem Engine A).
3. **Decimator:** 12-bit, 26.04 kHz sample rate.
4. **Tape:** Type I drive +3–5 dB; LP 10.5–12 kHz.
5. **Presence EQ:** +2 dB @ 220–260 Hz; +3.5 dB @ 3–4 kHz (Engine A).
6. **Terminal clip:** +4–6 dB into soft clipper, ~80% knee.

## Engine B in this repo

| Step | `index.html` / `generate_snare.py` |
|------|-------------------------------------|
| Membrane + pitch drop | `s_root`, `s_pmod`, `s_pdec` |
| BP noise + delayed amp | Fixed 2.4 kHz BP; `s_ndec` |
| Clap flam | `s_flam` |
| Decimate + quantize | `s_sr`, `s_bit` |
| HPF / LP / clip | Fixed 105 Hz / 11.5 kHz; `s_drive` |

For pitched 808 **sample** workflow (not pure synthesis), use Gem Module 2 and align sample start with the same degradation bus.
