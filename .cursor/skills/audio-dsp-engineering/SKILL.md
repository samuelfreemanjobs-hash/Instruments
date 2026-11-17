---
name: audio-dsp-engineering
description: Engineers realtime and offline audio DSP across JUCE VST, logue SDK, and DRUMSMITH native C++20 — allocation-free callbacks, synthesis voices, metering/QA, and S/C/A number discipline. Use when writing or reviewing DSP algorithms, drum/synth engines, render CLIs, WAV technical checks, or when the user mentions audio-dsp-engineering, native render, processBlock, OSC_CYCLE, or drumsmith DSP.
---

# Audio DSP Engineering

## Read first

| Target | Doc / entry |
|--------|-------------|
| Architecture | `docs/dsp-architecture-specification.md` |
| Plugin orchestration | skill `audio-plugin-coder` |
| DRUMSMITH agent | `claude/drumsmith/project.md`, `claude/drumsmith/docs/V2_NATIVE.md` |
| JUCE engineer rules | `docs/CLAUDE_VST_ENGINEER.md` |

## Scope

**In**

- Realtime: `processBlock`, `OSC_CYCLE`, native render loops
- Offline: DRUMSMITH `native/`, `scripts/*_factory.py`, `verify_kit.py`
- Synthesis: oscillators, envelopes, filters, saturation, noise, polyphony
- Measurement: peaks, DC, fades, pitch (document meter limits)

**Out**

- Plugin GUI / APVTS layout → `dsp-surface-chain`
- New VST scaffold / APC dream–ship → `audio-plugin-coder` / `/apc-*`
- Commercial listening sign-off / storefront claims (escalate to Sam)

## Hard rules (all targets)

1. **No alloc / locks / I/O** in the audio callback or per-sample hot path.
2. **Preallocate** in prepare/init; fixed voice pools; bound buffer `frames` / block size.
3. **Tag numbers** as **S** (sourced), **C** (computed), or **A** (preset / tune by ear).
4. **Technical PASS ≠ commercial ready** — never imply storefront ship from QA scripts alone.
5. Prefer **pure float cores** in shared/native headers; glue JUCE / logue / CLI at the edges.

## Choose a lane

| Ask | Do |
|-----|----|
| VST program chain | skill `dsp-program-chain` |
| Params / smoothing | skill `dsp-parameter-mapping` |
| New VST project | skill `juce-plugin-scaffold` + APC gates |
| NTS-1 / logue | skill `logue-hardware-port` |
| Drum kit agent / native WAV batch | `claude/drumsmith/` — `native_factory.py` + `g++-12` cmake |
| Complexity tier | skill `dsp-complexity-assessment` |

## DRUMSMITH native (v2)

```bash
cd claude/drumsmith
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build native/build -j && ctest --test-dir native/build --output-on-failure
python3 scripts/native_factory.py --out /tmp/kit --renderer native/build/drumsmith-render \
  --genre drift-phonk --count 64 --sku TLS --plan tail-light-static
python3 scripts/verify_kit.py --kit /tmp/kit --manifest /tmp/kit/manifest.json
```

- Voices: 808, kick, snare, clap, hat, cowbell, perc, FX — pure synthesis, no sample ROM.
- Default `/usr/bin/c++` may fail `libstdc++` on Cloud images → use **`g++-12`**.
- JUCE under `native/juce/` is **source**; build only with `-DDRUMSMITH_BUILD_JUCE=ON` + JUCE present.

## Algorithm checklist

- [ ] Denormals guarded (JUCE `ScopedNoDenormals` or equivalent)
- [ ] Soft clip / peak ceiling documented (e.g. −1 dBFS delivery)
- [ ] Micro-fades / DC block where one-shots are written to disk
- [ ] Anti-aliasing plan for discontinuities / nonlinear stages (oversample or BLEP — name it)
- [ ] Seeded noise is deterministic when tests require it
- [ ] Pitch/loop lengths from `dsp_math` / named formulas, not model guesses

## QA discipline

- Run the smallest green suite that covers the change (`self_test`, `test_v20`, `ctest`, plugin golden).
- Approximate true-peak / ZC pitch: label **approximate**; do not claim certified loudness or tuning.
- CALIPER / `verify_kit.py`: read-only measurement; they do not approve releases.

## Deliverables

1. Where the DSP lives (paths) and callback entry points  
2. Voice / stage list in order  
3. Commands run + exit codes  
4. Open **A**-tagged presets and any human ear gates left  

## Related skills

`audio-plugin-coder`, `dsp-program-chain`, `dsp-parameter-mapping`, `dsp-global-state`, `dsp-surface-chain`, `plugin-build-verify`, `logue-hardware-port`
