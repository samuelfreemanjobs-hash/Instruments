# JD Upgraded — architecture roadmap

## Phase 1 (implemented)

- **Audio pipeline**: `JDUpgradedAudioProcessor::processBlock` delegates to `VoicePool::render` with preallocated `outputScratch_` (no heap use on the audio thread).
- **Voice pool**: `kMaxPolyphony` (32) voices; each `SynthVoice` runs `kTonesPerVoice` (4) `Tone` instances.
- **Sample engine**: `SampleEngine` maps `PcmWaveform` single-cycle or looped PCM via pointer + length (no copies at runtime).
- **Envelopes**: `RateLevelEnvelope` advances on `kControlRateDivisor` (32) sample boundaries for pitch, cutoff, and amplitude.
- **Cleanroom waves**: `CleanroomWaveLibrary` supplies optional PCM for development without proprietary ROMs.

## Phase 2 (planned)

- `Source/DSP/Filter/ZdfTvf.h` — zero-delay feedback ladder with resonance limiting saturation.
- `Source/DSP/Modulation/` — PolyBLEP hard sync, ring mod, phase cross-mod, tone coupling modes.
- `Source/DSP/Effects/` — Group A distortion stages, Group B delay/reverb.

## Phase 3 (planned)

- `Source/Assets/RomLoader.h` — user-supplied JD-800/990 dump ingestion at runtime (installer ships no Roland ROM).
- `Source/Preset/SysExParser.h` — Roland SysEx checksum validation and parameter mapping.

## Phase 4 (planned)

- SIMD voice rendering (`Source/DSP/Simd/VoiceSimd.h`) — four tones per AVX2/NEON vector lane.
- Full 69-slider UI with palette linking and envelope editors; hardware A/B validation harness.
