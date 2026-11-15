# Trap Lord Synth — broken audition audio (root cause)

## Symptoms (Nov 2025 auditions)

- 21/24 WAVs were nearly identical: RMS ~0.67–0.68, peak ~0.95, spectral centroid ~10.7 kHz, strongest bin ~15.4 kHz.
- No note envelopes, no pitch content, no silence between notes — consistent with a full-scale noise hash, not synthesis.

## Root causes (verified)

1. **Uninitialized offline render buffers** — `processBlock` did not clear the output buffer before `synth.renderNextBlock` and FX. Headless blocks used stack-allocated `juce::AudioBuffer` parts that were never zeroed. Garbage was passed through delay (feedback), chorus, reverb, and a final `tanh` limiter, producing a constant hash across presets.

2. **Velocity never stored on voices** — `TrapVoice::startNote` did not assign the `velocity` member. Output was effectively `× 0`, so once buffers were cleared the engine went silent in QA.

3. **Broken 24-bit WAV export in headless tools** — `createWriterFor(..., stream.release())` left many 24/16-bit files as digital zero despite a non-silent float buffer. Fixed by using JUCE 8 `AudioFormatWriterOptions` and `std::unique_ptr<OutputStream>` (no manual `stream.release()`).

## Secondary contributors

- Delay feedback was reduced and wet levels capped to avoid runaway energy when garbage was present.
- `generateArp` is skipped during offline renders so MIDI timelines are not rewritten.

## Fixes

- `buffer.clear()` at the start of `processBlock`; offline parts explicitly cleared.
- `velocity = vel` in `TrapVoice::startNote`.
- Shared `writeKeygroupWav()` for MPC/keygroup exports; CLI `writeWavFile()` uses modern writer API.
- Category-specific audition MIDI in `OfflineAudition::buildTimeline(category, ...)`.
- `tests/test_trap_lord_audio_qa.py` CI gate + `diagnose` CLI for init saw / sine / middle C.
