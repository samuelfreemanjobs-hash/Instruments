# DiskLordz Stem-Shield — DSP architecture

**Product role (Phase 5):** Sub-bus peak containment and master stem balancer for beatmakers exporting stems to artists (BeatStars / Airbit workflows).

## Design goals

1. **Inter-sample (true-peak) safety** — Stem masters must survive codec overshoot; default ceiling **-0.5 dBTP**.
2. **Dynamic sub balance** — When the split sub band dominates the mix energy, pull sub gain down before limiting so the limiter does less destructive work.
3. **Lookahead limiting** — Preallocated delay line; gain reduction reacts to peaks in a lookahead window (default **2.5 ms**).
4. **Realtime contract** — No allocation, locks, or I/O in `processBlock`; meters via atomics; denormal protection; NaN/inf guarded.

## Signal path

```
Input × GAIN
  → Linkwitz-Riley 4th-order split @ SUB CROSSOVER
  → Sub envelope vs full-band envelope → dynamic sub gain (SUB BALANCE %)
  → sum (low + high)
  → write to lookahead delay ring
  → linked true-peak scan (4× linear estimate) over [t … t+LOOKAHEAD]
  → gain envelope (instant attack, RELEASE ms release)
  → read delayed audio × GR → OUT meters
```

## True-peak estimation

Full oversampling on the entire buffer would be costly on every stem bounce. For Stem-Shield we use:

- **Limiter analysis:** 4× inter-sample estimate between adjacent samples (`estimateTruePeak4x`) on the lookahead window (same family as broadcast inter-sample meters).
- **Oversampling object** (4× polyphase IIR) is prepared for future extension and included in **reported latency** alongside lookahead.

This matches the commercial strategy note: *inter-sample peak limiting + dynamic sub balance* with a pragmatic CPU budget for beat export sessions.

## Latency

`latencySamples = lookaheadMs→samples + oversamplingAnalysisLatency`

The host receives this via `setLatencySamples` whenever lookahead changes.

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

- Latency reported > 0
- Hot sine + gain: **4× true-peak estimate** ≤ ceiling
- Sub balance reduces RMS on sub-heavy 50 Hz material vs 0% balance
- NaN → finite
- 20 factory presets load

## Related code

- Implementation: `vst/StemShield/Source/StemShieldDsp.*`
- Shared smoothing: `shared/dsp/parameter_smoothing.h`
