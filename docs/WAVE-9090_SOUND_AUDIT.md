# WAVE-9090 sound audit

**Instrument:** sampleless **wavetable synthesizer** (trap / phonk / underground hip-hop) — not a 909 drum machine. Offline voice render at 48 kHz, velocity 0.9, 1.5 s (audit tool) or 1.0 s (CI).

Evidence logs:

- Before: `/opt/cursor/artifacts/wave9090-before-audit.log`
- After: `/opt/cursor/artifacts/wave9090-after-audit-full.log`
- Short WAVs: `/opt/cursor/artifacts/wave9090-before/`, `/opt/cursor/artifacts/wave9090-after/`

## Summary (before → after)

| Issue | Before (typical) | After (typical) | Fix |
|-------|------------------|-----------------|-----|
| DC on bass presets | **+0.075** (+~−22 dBFS) on Sub-Zero / preset 0 | **\< 0.002** | Remove mean from each wavetable frame; mipmapped bank |
| Peak level (voice) | **−0.94 dBFS** (~0.89 linear), hot | **−4 to −12 dBFS** pre-master + limiter at **−0.5 dBFS** in processor | Voice gain staging, exponential ADSR, 5 ms fade-in, `-0.5 dBFS` soft ceiling |
| Note-on clicks | **0.79–1.73** Δ sample (Metallic Stab / high notes) | **\< 0.43** on preset 0 @72; plucks **\< 0.17** typical | 6 ms fade-in, destruct reset on note-on, min attack 3 ms, filtered tape noise |
| Aliasing / HF grime | hf_diff_ratio **> 1.0** on bells/plucks | **\< 0.29** @ MIDI 72 on factory presets (audit) | 8-level wavetable mipmaps keyed to `phaseInc` |
| Velocity | Amp only | Amp **+** filter cutoff scaled by velocity^0.75 | `Voice::render` |
| Thin low end / 808 | Wavetable-heavy bass | Sub = band-limited table sine; **Trap 808** preset pitch env **−360 ct** | `sampleSine`, pitch envelope |
| Stereo | Hard L/R by note parity | Constant-power pan + width jitter | Per-voice pan |
| Noise macro | Unfiltered, loud | 1-pole LP @ ~0.22 cutoff, gain **0.018×** | `TapeWobble` + noise LP |

## Trap 808 preset (index 0) — headline numbers

| MIDI | peak dBFS (before) | peak dBFS (after) | dc (before) | dc (after) | click Δ (before) | click Δ (after) |
|------|--------------------|-------------------|-------------|------------|------------------|-----------------|
| 36 | −0.94 | −8.33* | 0.0755 | −0.0001 | 0.130 | 0.002 |
| 60 | −0.94 | −8.31* | 0.0754 | 0.0011 | 0.790 | 0.166 |
| 72 | −0.94 | −8.31* | 0.0751 | 0.0016 | 1.589 | 0.427 |

\*Pre-DAW master; in FL add **Output** gain (~+4.5 dB default APVTS) to sit around **−6…−1 dBFS** under limiter.

## Automated regression (`Wave9090Tests`)

- Wavetable peak in range, engine non-silent
- All 8 factory presets: **peak ≤ −0.5 dBFS**, **|dc| ≤ 0.0025**, click guard, **note 72** hf proxy ≤ 1.25
- Linux **pluginval strictness 5**: PASS (this branch)

## Sam listening (FL Studio)

Use Windows VST3 artifact from CI; compare before/after WAVs on the same MIDI (36 / 60 / 72). Preset bar: **Trap 808**, **Phonk Lead**, **Crisp Pluck**, etc.