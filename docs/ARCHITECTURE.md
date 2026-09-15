# JD Upgraded — architecture roadmap

## Phase 1 (implemented)

- **Audio pipeline**: `JDUpgradedAudioProcessor::processBlock` delegates to `VoicePool::render` with preallocated `outputScratch_` (no heap use on the audio thread).
- **Voice pool**: `kMaxPolyphony` (32) voices; each `SynthVoice` runs `kTonesPerVoice` (4) `Tone` instances.
- **Sample engine**: `SampleEngine` maps `PcmWaveform` single-cycle or looped PCM via pointer + length (no copies at runtime).
- **Envelopes**: `RateLevelEnvelope` advances on `kControlRateDivisor` (32) sample boundaries for pitch, cutoff, and amplitude.
- **Cleanroom waves**: `CleanroomWaveLibrary` supplies optional PCM for development without proprietary ROMs.

## Phase 2 (implemented)

- **ZDF TVF** (`Filter/ZdfTvf.h`) — trapezoidal two-pole lowpass with `tanh` resonance limiting on each `Tone`.
- **Modulation** — `PolyBlep.h` hard sync in `SampleEngine`; `ToneCouplingMode` in `SynthVoice` (ring, phase cross-mod, sync pairs).
- **Effects** — `GroupADistortion` (7-stage soft clip) and `GroupBSpatial` (fixed delay + diffuse feedback) on the master bus.

## Phase 3 (in progress)

- **Factory ROM** — `jdupg_cleanroom.rom` v2 (256 waves); MS sets 1–4 EP-biased, 5+ mapped by role.
- **128 factory programs** — four-tone stacks; `currentProgram` persisted in plugin state.
- **Per-tone mute** — `tone1Mute`…`tone4Mute` + UI M1–M4.
- `RomLoader` — embedded ROM + `JDUPGRADED_ROM_PATH` override.
- `SysExParser` — Roland 0x41 packet checksum validation (no patch mapping yet).
- User-supplied legacy dump translation (planned; not shipped).

## Phase 4 (planned)

- SIMD voice rendering (`Source/DSP/Simd/VoiceSimd.h`) — four tones per AVX2/NEON vector lane.
- Full 69-slider UI with palette linking and envelope editors; hardware A/B validation harness.
