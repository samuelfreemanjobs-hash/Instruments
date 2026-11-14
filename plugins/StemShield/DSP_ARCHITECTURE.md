# DiskLordz Stem-Shield — DSP architecture

**Product role (Phase 5):** Sub-bus peak containment and master stem balancer for beatmakers exporting stems to artists (BeatStars / Airbit workflows).

## Design goals

1. **Inter-sample (true-peak) safety** — Stem masters must survive codec overshoot; default ceiling **-0.5 dBTP** measured with **ITU-R BS.1770-4 Annex 2** detection.
2. **Dynamic sub balance** — When the split sub band dominates the mix energy, pull sub gain down before limiting so the limiter does less destructive work.
3. **Lookahead limiting** — Preallocated delay line at **base rate**; gain reduction reacts to BS.1770 peaks in a lookahead window (default **2.5 ms**).
4. **Realtime contract** — No allocation, locks, or I/O in `processBlock`; meters via atomics; denormal protection; NaN/inf guarded.

## Signal path

```
Input × GAIN
  → Linkwitz-Riley 4th-order split @ SUB CROSSOVER
  → Sub envelope vs full-band envelope → dynamic sub gain (SUB BALANCE %)
  → sum (low + high)
  → write to lookahead delay ring (audio path, base rate)
  → BS.1770 polyphase FIR true-peak scan on sidechain (detection only)
  → linked gain envelope (instant attack, RELEASE ms release)
  → read delayed audio × GR → OUT meters
```

## True-peak detection (BS.1770-4 Annex 2)

**Implementation:** `Bs1770TruePeak.h` — 4× polyphase FIR, **12 taps per phase** (48 coefficients), tabulated Annex 2 coefficients (phases 0–3; phase 3 = reversed phase 0).

- **Sidechain only:** the audio path is **not** upsampled; oversampling exists on the measurement path inside the limiter lookahead scan.
- **Rate policy (libebur128-style):**
  - **&lt; 88.2 kHz:** 4× (all four phases)
  - **88.2–176.4 kHz:** 2× (phases 0 and 2 of the same bank)
  - **≥ 176.4 kHz:** pass-through (sample peak; inter-sample risk is negligible at these rates)
- **Group delay:** **6 base samples** (integer) added to the audio delay so gain reduction aligns with the delayed programme audio.

### Why not linear 4×?

Linear interpolation underestimates true peaks for the classic **fs/4 + 45°** sine (sample peak ~−3 dB vs true peak ~0 dBTP). The BS.1770 FIR is the normative reference for broadcast loudness tooling and is what our contract tests verify against.

## Latency

```
latencySamples = lookaheadMs→samples + detectorGroupDelay (6 at ≤176 kHz, 0 at ≥176.4 kHz)
```

Reported with `setLatencySamples`. Audio `readPos = writePos − lookahead − groupDelay` so detector peaks line up with the outgoing samples.

**Defaults @ 2.5 ms lookahead:**

| Sample rate | Lookahead | Group delay | Total |
|-------------|-----------|---------------|-------|
| 44.1 kHz | 110 | 6 | **116** |
| 48 kHz | 120 | 6 | **126** |
| ≥ 176.4 kHz | (scaled) | 0 | lookahead only |

## Parameters

| ID | UI | Default | Notes |
|----|-----|---------|-------|
| `ceiling` | CEILING | -0.5 dBTP | Linked stereo GR |
| `sub_balance` | SUB BALANCE | 65% | 0 = limiter only |
| `sub_crossover` | SUB XOVER | 80 Hz | LR4 split |
| `release` | RELEASE | 120 ms | GR release |
| `lookahead` | LOOKAHEAD | 2.5 ms | Delay + analysis |
| `gain` | GAIN | 0 dB | Input trim |

## Tests (`StemShieldProcessorTests`)

1. **Ceiling:** fs/4 sine (+45°) and 0 dBFS sample-peak square @ **44.1/48 kHz** — output true peak (8× cascaded BS.1770 reference meter) ≤ ceiling **+0.1 dB**
2. **Latency:** reported `getLatencySamples()` equals measured impulse peak index
3. **Safety:** NaN → finite; long silence → silence out (denormals flushed)

## Related code

- Limiter: `vst/StemShield/Source/StemShieldDsp.*`
- Detector: `vst/StemShield/Source/Bs1770TruePeak.h`
- Shared smoothing: `shared/dsp/parameter_smoothing.h`
