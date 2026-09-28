# Research → Engine B parameter map

This table ties the Memphis phonk kick/snare research reports to controls in `index.html` and the Python generators under `scripts/`.

## Kick (MEMPHIS-660)

| Research topic | Sonic target | Engine B control | Default | Notes |
|----------------|--------------|------------------|---------|-------|
| (1) TR-808 / DR-660 lineage | Pitch-transposed PCM knock | `pitch_mod`, `pitch_decay` | 48 st, 22 ms | Exponential pitch drop on sine fundamental |
| (1) Sub foundation 35–55 Hz | Room for 808 bass | `root_freq` | 43.65 Hz (F1) | Tune to project key; keep sub decay short via `amp_d` |
| (1) Body knock 65–120 Hz | Dense thud | Implicit in pitch envelope peak | — | Higher `root_freq` or lower `pitch_mod` shifts weight |
| (1) Click 1–2.8 kHz | Woody transient | **Not separate layer** | — | Use `flt_type=bp`, raise `flt_cut`, or add sample layer in DAW (Engine A) |
| (2a) Transient click layer | Cut through mix | — | — | Gem Engine A: layer 808 click sample; future: optional click osc |
| (2b) Body layer | Mid thud | Single sine + pitch env | — | Same as core oscillator |
| (2c) Sub tuning vs bassline | Avoid clash | `root_freq`, `duration`, `amp_d` | 250 ms gate, 140 ms decay | Short gate leaves glide space |
| (3) Sine + pitch env + decay | Core synthesis | `root_freq`, `pitch_mod`, `pitch_decay`, amp ADSR | amp 0/140/0/50 ms | `getADSR()` on amplitude |
| (4a) EQ / tilt | Tape head absorption | `flt_type=tape`, `flt_cut` | 12 kHz one-pole | Or SVF LP/BP/HP + filter ADSR |
| (4b) Saturation / clip | PortaStudio red meters | `clip_drive` | 4 dB → tanh | Terminal stage after decimation |
| (4c) Bitcrush / SP-1200 | 12-bit @ 26.04 kHz | `bit_depth`, `lofi_sr` | 12, 26040 | ZOH decimation then quantize |
| (4d) Compression | Punch | — | — | Use DAW bus comp; clipper approximates peak control |
| (5) Mix mono sub, sidechain vs bus clip | Low-end | Documented in Gem Module 4 | — | Not simulated in one-shot export |

## Snare (MEMPHIS-DR660)

| Research topic | Sonic target | Engine B control | Default | Notes |
|----------------|--------------|------------------|---------|-------|
| (1) DR-660 / 808 / 909 lineage | PCM grit | `s_sr`, `s_bit` | 26040 Hz, 12-bit | Same decimation topology as kick |
| (2) Membrane + noise + flam | Acoustic triad | `s_root`, `s_pmod`, `s_pdec`, `s_ndec`, `s_flam` | 220 Hz, 24 st, 18 ms, 190 ms, 4 ms | Triangle blend 20% on body |
| (2) Noise wires 1.2–5.5 kHz | Snare rattle | Fixed BP @ 2.4 kHz, BW 1.8 kHz | — | Hard-coded in `synthesizeSnare()` |
| (3) Osc + noise + envelopes | Build from scratch | Body hold 3 ms; noise attack 2.5 ms | — | Matches research delayed noise onset |
| (4a) Pitch / decay | DR-660 pad pitch | `s_pmod`, `s_pdec`, `s_ndec` | — | Shorter `s_ndec` for drift phonk |
| (4b) 12-bit decimation | Aliasing | `s_sr`, `s_bit` | — | |
| (4c) Cassette roll-off | HF damping | Fixed LP 11.5 kHz | — | After 105 Hz HPF |
| (4d) EQ box cut | Mud removal | Fixed HPF 105 Hz | — | Gem Engine A: −4 dB @ 480 Hz parametric |
| (4e) Soft clip | Blown-out knock | `s_drive` | 5.5 dB | tanh waveshaper |
| (5) Room / plate / width | Space | — | — | Engine A only; clap layer is mono sum |
| (6) Mix vs 808 / cowbell | Arrangement | Gem Module 4 | — | Sidechain or bus soft-clip choice |

## Style presets (quick starting points)

| Style | Kick tweaks | Snare tweaks |
|-------|-------------|--------------|
| Raw 90s / Doomshop | `lofi_sr` 26040, `bit_depth` 12, `clip_drive` 4–5, `flt_type` tape @ 11–12 kHz | `s_flam` 4–6 ms, `s_drive` 5.5+, long `s_ndec` |
| Drift phonk | Shorter `duration` / `amp_d`, `clip_drive` 5–6, filter BP for click | `s_ndec` ≤ 160 ms, `s_pmod` 28–36 st, `s_drive` 6+ |
| Modern cleaner | `lofi_sr` 32000+, `bit_depth` 16+, lower `clip_drive` | Same decimation loosened, shorter flam |
