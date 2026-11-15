# Skill 02 — DSP realtime & architecture

## Program vs surface chain

| Chain | Owns | Must not |
|-------|------|----------|
| **Program** | `Source/Dsp/`, engine headers, `processBlock` audio | UI widgets, file I/O |
| **Surface** | APVTS, `PluginEditor`, presets | Per-sample heavy math |

Orchestrator: `.cursor/skills/audio-plugin-coder/SKILL.md`.

## Non-negotiables (audio thread)

- No `malloc` / `new` / growing `std::vector`
- No mutexes, blocking queues, or logging
- Parameter changes: smoothed (one-pole / `SmoothedValue`) before DSP
- Bounded voices: static pool or fixed array
- Denormals: `juce::ScopedNoDenormals` at block entry

## Embedded (MPC Sample / Melodee)

- Default **low max voices**; document CPU cost in `plugins/<Name>/architecture.md`
- Avoid huge FFTs, unbounded oversampling, heavy analog modeling
- GUI: prefer simple layouts; Melodee may run plugin UI on constrained framebuffer hosts

## Complexity tier

Run `.cursor/skills/dsp-complexity-assessment/SKILL.md` for new engines. **L2+** on A55 needs explicit voice caps and bench notes.

## Shared code

- Reuse `shared/dsp/` where possible
- Do **not** include logue `userosc.h` in VST targets
- Port float algorithms deliberately; do not assume q31 on aarch64 VST (FPU available)

## Tests

- Processor contract tests in `vst/<Name>/tests/` when product already has them
- Golden audio: coordinate with `golden-wav-qa` before committing WAV changes