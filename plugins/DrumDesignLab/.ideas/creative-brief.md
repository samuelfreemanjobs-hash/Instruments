# DrumDesignLab — creative brief (DPCS Phase 1 PRD)

**Track:** APC greenfield · **Category:** `synth.subtractive` · **Class:** synth
**Phase:** 1 — behaviour only. No C++, JUCE types, includes or file paths beyond the dependency list.
**SOP:** [SOP-VF-01 Intake & Dream](../../../docs/disklordz/sops/SOP-VF-01-intake-dream.md)

## Elevator pitch

`DrumDesignLab` is a 16-pad polyphonic drum synthesizer, three-layer sound-design workstation, and deterministic sample-kit factory covering Detroit Trap, Plugg (`Classic`, `PluggnB`, `Dark`, `Retro`, `Detroit`, `Ambient`), Rage (`Redline`, `Chaos`, `Alien`, `Dark`, `Hyper`, `Melodic`), Memphis, Drift Phonk, and Southern Trap. Every hit is synthesized from scratch with reproducible seeds so producers can play expressive MIDI kits in real time, mutate and A/B-compare drum recipes, drag-and-drop clean 24-bit WAVs into a DAW, and batch-render QC-verified commercial kits and bounce/roll MIDI.

## User stories

1. As a trap or plugg producer, I trigger 16 velocity-sensitive MIDI pads across Detroit, Plugg, Memphis, and Phonk recipes, sculpt Body/Attack/Texture layers, mutate a sound with parameter locks and undo, and drag the rendered WAV straight into my DAW arrangement.
2. As a factory operator, I run factory CI on `DrumDesignLab` and get exit 0 without subjective listening, verifying deterministic renders, polyphonic voice isolation, choke groups, DC/peak bounds, and tail-verified 808 tuning.
3. As a debugger, I send simultaneous overlapping MIDI notes, rapid knob automation, all-notes-off, zero-length blocks, and out-of-range/non-finite host state and observe click-free voice stealing, deterministic phase reset on one-shot export, and zero non-finite output samples.

## Audio behaviour

### Topology

- **Inputs:** MIDI event stream (16-pad Bank A GM-compatible default notes C1..D#2, user-remappable per pad; MIDI channel omni or 1–16; host transport BPM/position for step sequencer sync). No audio input required.
- **Outputs:** 1 stereo main bus (2 channels, 32-bit float host buffer). Low-frequency sub energy below 110 Hz is summed mono-compatible; stereo width applies only to mid/high Body harmonics and Texture layers.
- **Operating modes:**
  - `SYNTH`: Edit the selected pad's primary synthesis engine (12 engine slots; V1 core ships Analog Drum, Hybrid Analog/Digital, FM 2–6 Op, Physical/Modal Rim & Hat, Additive Metallic, and Waveshaping, with remaining slots mapped cleanly onto the same three-layer voice contract).
  - `DESIGN`: Balance and sculpt the three per-pad layers (`Body`, `Attack`, `Texture`), transient attack/sustain shaper, 4x/8x oversampled saturation, seeded DNA mutation with parameter locks, undo/redo history, and A/B slot comparison.
  - `FACTORY`: Off-thread deterministic renderer producing 16/24-bit PCM or 32-bit float WAV one-shots, 16/32/64-step bounce loops, isolated stems, MIDI groove files, QC reports, JSON/CSV manifests, and ZIP archives using the exact same voice recipe model as live playback, followed by an offline export ceiling stage.
- **Processing order (per triggered voice):**
  1. Sample-accurate MIDI trigger splits the block at event offsets and allocates an independent voice slot from a 32-voice pool without disturbing already-playing hits.
  2. Choke-group check (e.g., closed hat choking open hat, or mono-glide 808 choke) applies a 2.5 ms linear fast-fade to choked voices rather than hard-zeroing samples.
  3. Three-layer generator (`Body` + `Attack` + `Texture`) runs with phase-aligned start phases and deterministic per-pad/per-variation PRNG seed.
  4. Multi-stage pitch envelope (initial punch Hz -> root sub Hz over sweep ms, plus secondary sag) and curved amplitude envelopes (exponential, linear, or logarithmic).
  5. Per-voice multimode filter (low-pass, band-pass, high-pass) + phase-coherent split-band crossover (`ddl_sub_crossover_hz`, `60–220 Hz`) on 808s so the clean fundamental sub stays mono and intact while the parallel Dirt layer enters oversampled asymmetric clipping, saturation, or wavefolding (`ddl_dirt_blend`), with matched delay/phase alignment when recombining paths.
  6. Oversampled (4x realtime default, 8x optional/offline) nonlinear waveshaper / wavefolder / tape saturation + Transient Violence sculptor (click, thump, attack, sustain) + DC blocker.
  7. Voice velocity curve, pad level, pan (above sub crossover), sum into stereo bus, smoothed master output gain, and non-finite/hard-safety guard (`±4.0` linear clamp in live mode; offline `FACTORY` export applies DC removal, micro-fades, and 4x oversampled true-peak static ceiling).

### Controls

- **Mode, Style Profile & Destruction Tier (`ddl_mode`, `ddl_profile`, `ddl_destruction_mode`, `ddl_selected_pad`):** Selects `SYNTH` / `DESIGN` / `FACTORY`, switches starting sound-design profile (`Detroit Trap`, `Classic Plugg`, `PluggnB`, `Dark Plugg`, `Retro Plugg`, `Detroit Plugg`, `Ambient Plugg`, `Cloud Bounce`, `Heavy ATL Brick`, `Houston Trunk`, `Chicago Drill`, `New Orleans Bounce`, `Baton Rouge Trap`, `Rage 01 Redline`, `Rage 02 Chaos`, `Rage 03 Alien`, `Rage 04 Dark Matter`, `Rage 05 Hyperdrive`, `Rage 06 Nebula`, `Memphis`, `Drift Phonk`, `Southern Trap`), switches every voice between `Clean` / `Mixed` / `Destroyed`, and focuses pad 1–16 for editing. Switching pad focus must never cut off voices already ringing.
- **Rage Bass Reactor & Engine (`ddl_engine`, `ddl_bass_design`, `ddl_length_tier`, `ddl_sub_freq_hz`, `ddl_punch_freq_hz`, `ddl_pitch_sweep_ms`, `ddl_amp_decay_ms`, `ddl_glide_ms`, `ddl_wave_morph`):** Covers the 12 Rage Bass Reactor designs (`Redline`, `Chaos`, `Alien Slide`, `Monster`, `Nuclear`, `Digital`, `Hollow`, `Elastic`, `Short Fuse`, `Ghost`, `Detuned`, `Hybrid`) across `Short` / `Medium` / `Long` tiers, `35–75 Hz` fundamental, `80–250 Hz` initial punch, `5–100 ms` pitch sweep, `20–2500 ms` amplitude decay, `0–250 ms` legato portamento (`90 ms` on `Alien Slide 808`), and `0–100%` sine-to-triangle/PD morph. Tuned 808s synthesize directly at real musical frequencies rather than pitch-renaming a single WAV.
- **Three-Layer Mixer (`ddl_body_level`, `ddl_attack_level`, `ddl_texture_level`):** 0–100% gain per layer (`−inf` at 0%, unity at 80%, `+3 dB` at 100%). At 0% all three layers produce exact digital silence (`0.0`).
- **Tone, Filter & FM (`ddl_filter_cutoff_hz`, `ddl_filter_reso`, `ddl_fm_ratio`, `ddl_fm_index`):** Filter spans 40 Hz–18 kHz with self-oscillation prevented (`resonance <= 0.92`). FM ratio spans `0.5x–8.0x` and FM modulation index `0.0–6.0` with exponential decay for PluggnB bells, digital rims, and lasers.
- **Drive, Split-Band Dirt, Vintage Sampler Grit, Transient & Space (`ddl_drive`, `ddl_sub_crossover_hz`, `ddl_dirt_blend`, `ddl_sampler_grit`, `ddl_oversample`, `ddl_transient_punch`, `ddl_transient_sustain`, `ddl_stereo_width`, `ddl_reverb_mix`):** Drive runs through 4x or 8x anti-aliased oversampling with level compensation within `±1.5 dB` at mid-drive and phase-coherent parallel blending above `ddl_sub_crossover_hz` so extreme Rage distortion never destroys fundamental sub weight. `ddl_sampler_grit` (`0–100%` wet) blends a `12-bit / 26.04 kHz` zero-order-hold vintage hardware DAC stage followed by stereo `11.5 kHz` (`Q = 0.707`) analog reconstruction low-pass filters. Stereo width never decorrelates frequencies below 110 Hz (mono-correlation `>= +0.98` below 110 Hz). Bounded plate/room tail decays cleanly below `−80 dBFS` within 600 ms.
- **Seeded RAGE DNA Mutation, 2D Hyper DNA Morph Matrix & Locks (`ddl_seed`, `ddl_hyper_family`, `ddl_dna_x`, `ddl_dna_y`, `ddl_dna_influence`, `ddl_dna_seq_mod`, `ddl_mutate_intent`, `ddl_mutate_strength`, `ddl_ab_slot`):** Deterministic integer seed (`1..999999`), 20-family Hyper Series bank selector (`ddl_hyper_family`), **2D 5-Genre Hyper DNA XY Morph Matrix** (`ddl_dna_x`, `ddl_dna_y` inverse-distance `1 / (d + 0.08)^2.6` weighting across `Plugg (0.12, 0.16)`, `Rage (0.85, 0.14)`, `Hypertrap (0.77, 0.68)`, `Hyperpop (0.22, 0.85)`, and `Digicore (0.51, 0.47)` + `ddl_dna_influence` `0–100%`), per-step DNA Sequencer modulation (`ddl_dna_seq_mod`), and **drum-specific invariant preservation** (808 preserves root Hz with single-pass log-ratio pitch-start interpolation; Kick preserves fundamental thump; Snare preserves main body resonance; Clap preserves burst count; Hat preserves metallic/noise core; Rim/Bell preserve modal/pitch center) plus independent locks for `root`, `transient`, `decay`, and `drive`. Mutating updates recipe snapshots on the message thread with full undo/redo and never triggers disk WAV rendering on knob drag.
- **Sequencer, Bounce Syncopation & Hyper-Hat Rolls (`ddl_seq_enabled`, `ddl_seq_steps`, `ddl_bpm`, `ddl_swing`, `ddl_bounce_sync_ms`, `ddl_roll_mode`, `ddl_humanize`):** 16/32/64-step Detroit/Plugg/Rage/Chicago/Louisiana groove engine with exact rational beat-to-sample event calculation (no cumulative step rounding drift), `ddl_bounce_sync_ms` (`0–35 ms` off-beat hat/rim pocket shift + rhythmic volume ducking curve for Louisiana bounce), per-step probability, `1/16`, `1/32`, triplet, and stutter-roll ratchets, and microtiming.
- **Factory Export, Kit Tiers & Output (`ddl_kit_tier`, `ddl_export_ceiling_db`, `ddl_master_gain_db`, `ddl_bypass`):** Configures `Starter`, `Standard`, or `Mega` 9-category commercial batches (`44.1 kHz / 16-bit`, `48 kHz / 24-bit`, `96 kHz / 24-bit`, or `32-bit float`). Live `ddl_master_gain_db` (`−24..+6 dB`) governs DAW playback level; `ddl_export_ceiling_db` (`−12.0..−0.1 dBTP`, default `−1.0 dBTP`) governs offline WAV export gain using 4x oversampled true-peak measurement and Mix Translation QA (loudness, DC, spectral balance, mono sub compatibility).

### Genre starting recipes (normative defaults for factory verification)

1. **Detroit Punch 808:** `49.00 Hz` (`G1`) fundamental, `165 -> 49 Hz` pitch attack over `24 ms`, `340 ms` amplitude decay, sine/triangle morph `28%`, short click Attack layer, asymmetric oversampled saturation, mono sub below 110 Hz.
2. **Redline 808 (Rage):** `46.25 Hz` (`F#1`) fundamental, `175 -> 46.25 Hz` pitch sweep over `32 ms`, `480 ms` amplitude decay, sine + `20%` triangle, `3.5x` asymmetric oversampled saturation above clean sub crossover.
3. **Alien Slide 808 (Rage):** `55.00 Hz` (`A1`) fundamental, `140 -> 55 Hz` pitch sweep over `55 ms`, `750 ms` amplitude decay, `90 ms` portamento glide, `2.2x` saturation drive.
4. **Chaos 808 (Rage):** `41.20 Hz` (`E1`) fundamental, `220 -> 41.20 Hz` pitch sweep over `22 ms`, `360 ms` amplitude decay, clean sub + parallel asymmetric clipped dirt layer.
5. **Plugg Crystal Hat / Hyper Hat:** 6 inharmonic metallic partials + white/pink filtered noise Texture, `7.5–8.0 kHz` high-pass filter, `24–28 ms` exponential decay constant `tau`, velocity-dependent brightness (quieter hits soften high-pass/partial tilt, not just amplitude), closed-hat choke group 1.
6. **Memphis Tape Snare / Chrome Snare:** Tonal Body resonators (`190 Hz + 335 Hz` dual resonator `tau = 45 ms` on `Chrome Snare`; `175–210 Hz` on `Memphis Tape Snare`) + band-passed noise (`tau = 95–110 ms`), asymmetric clip or warm tape saturation.
7. **Apocalypse Kick & Chaos Clap:** `Apocalypse Kick` sweeps `145 -> 54 Hz` (`pitch tau = 18 ms`, `amp tau = 105 ms`, `3 ms` click, `2x` saturation); `Chaos Clap` fires four `1.8 kHz` band-passed bursts at `0, 9, 19, 31 ms` into a `65 ms` tail `tau`.
8. **PluggnB Digital Bell / Alien Laser:** 2-op FM (`880 Hz` carrier, `2.5x` ratio, `220 ms` decay for Bell; `960 Hz` swept carrier, `2.7x` ratio, `pitch tau = 42 ms`, `amp tau = 110 ms`, resonant soft clip for `Alien Laser`).
9. **Cloud Bounce Trio (Woody Spinz 808, Razor Nasal Clap, Reverse Open-Hat Swell):** `Woody Spinz 808` (`48.0 Hz` fundamental, `165 -> 48 Hz` over `22 ms`, `360 ms` bouncy decay, `145 Hz` `Q = 1.4` woody mid resonance before `1.65x` warm `tanh` saturation); `Razor Nasal Clap` (3 staggered bursts `9 ms` apart, `380 Hz` high-pass + `1.85 kHz` presence peak, `78 ms` dry tail); `Reverse Open-Hat Swell` (6 band-limited TR-style metallic oscillators `205.3..800 Hz * pitchScale`, `7.2 kHz` high-pass, concave `(t / 110 ms)^2.4` suction swell + `15 ms` hold + `2 ms` click-free choke ramp).
10. **Heavy ATL / Brick Quartet (Brick 808, Beater Hard Kick, Layered Snare/Clap, 909 Metallic Hat):** `Brick 808` (`40–55 Hz` fundamental, `3.5x` ~2-octave drop over `35 ms`, `1.4 s` decay, `28 Hz` sub-rumble high-pass, 4x oversampled asymmetric `tanh` drive `4.5x`); `Beater Hard Kick` (`300 -> 60 Hz` sine sweep over `18 ms` + `5 ms` band-passed white-noise beater click into 4x oversampled clipper); `Layered Snare/Clap` (`180 Hz + 330 Hz` dual triangle/sine body + 3 micro-bursts spaced `10–11 ms` apart through `120 Hz` HPF, `380 Hz` `-4.5 dB` scoop, and `4.5 kHz` `+5.5 dB` snap shelf); `909 Metallic Hat` (6 band-limited non-harmonic pulse oscillators `205.3..800 Hz` -> `6.8 kHz` HPF -> `10.5 kHz` `+4 dB` air peak).
11. **Houston Trunk / Screw Slab 16-Pad Bank:** `Warm Trunk 808` (`C1 32.70 Hz`..`G1 49.00 Hz`, `1.8–2.4 s` sustain, `3 ms` click-free subwoofer attack, `sine + 12% 2nd harmonic`, soft-knee tube `x / (1 + |0.65x|)` + even-order transformer warmth, `24 Hz` HPF + `260 Hz` LPF), round punch kicks (`52–62 Hz`, `punchRatio 2.5–3.2`), pitched-down snares/claps (`145–175 Hz`, `14–16 ms` spread), vintage hats, and dedicated `540/800 Hz` band-passed `808` cowbells (`0.65x` low / `0.78x` high), each routed through per-pad `12-bit / 26.04 kHz` `VintageSamplerStage` wet mix (`0.15..0.90`).
12. **Chicago Drill 16-Pad Bank (`125–145 BPM` South Side):** `Chicago Drill 808` (`41.2–49.0 Hz`, `2.8x` `26 ms` drop, `140 Hz` `+3 dB` mid presence, `4x` oversampled symmetric `tanh` + programmable `+7 / +12 st` mid-note `35 ms` portamento glides), punch kicks (`58–68 Hz`, `-10..-14 st` fast punch glide), `Dry Chop Snare/Clap` (`4–9 ms` tight spread, `0.12–0.21 s` dry tail, `3.2 kHz` crack boost), two-step/triplet closed hats, and `ChicagoCrash` (`218..845 Hz` 6-oscillator bank + stereo noise -> `3.6 kHz` HPF -> `8.8 kHz` `+4.5 dB` shimmer).
13. **Louisiana Expansion (`New Orleans Bounce` & `Baton Rouge Trap`):** `Baton Rouge 808` (`+18..+20 st` fast sweep into `4x` oversampled cubic/tanh soft clipper `tanh(x - 0.15x^3)` + `150–300 Hz` phone-speaker cut), `Plucked BooBass` (band-limited saw + octave-down sub through `1600 -> 240 Hz` `65 ms` resonant TPT lowpass sweep), `NOLA Dual-Mode Rimshot` (`465 Hz` + `1720 Hz` resonant modes), `Baton Rouge Crack Snare` (`195 Hz` shell + `1.1 kHz` HPF wire), `NOLA Formant Chant Synth` (`Hey_EH` `530/1840/2480 Hz`, `Oh_OW` `570/840/2410 Hz`, `Uh_AH` `730/1090/2440 Hz` parallel `Q=6.5` formant filters over a pitch-scooped glottal source — zero vocal recordings), and `Triggerman Pattern Generator` (`960 TPQ` syncopated kicks, `1.75 / 2.25 / 3.75` rolling rims, `1/32` velocity-ramped machine-gun hat rolls).
14. **Hyper Series (Separate `Hypertrap` & `Hyperpop` Engines + 20 Kit Families):** `Hypertrap Nuclear 808` (`48 Hz` sub, `180 -> 48 Hz` over `28 ms`, `550 ms` decay `tau`, parallel clean sub + wavefolded saturation), `Hyperpop Bubble Kick` (`110 -> 62 Hz` springy rubber pitch sweep over `45 ms`, triangle/sine morph, resonant plastic click), `Glitch Glass Snare` (`220 Hz` body + `2.8 kHz` band-passed noise + short metallic partials + gated tail), `Hyperplugg Crystal Hat` (6 inharmonic oscillators, `9 kHz` high-pass, `32 ms` decay `tau`), and `Digital Alien Perc` (`950 Hz` FM carrier, `3.2x` ratio, falling FM index, `120 ms` decay `tau`), plus release-wave banks `Digital Warfare (Hypertrap)`, `Plastic Dreams (Hyperpop)`, `Crystal Bounce (Hyperplugg)`, `Internet Drums (Digicore)`, and `Digital Decay (HexD)`.

### Silence and extremes

| Condition | Expected output / display |
|-----------|---------------------------|
| No MIDI notes & sequencer off for >= 1 s | Exact zero output (`peak == 0.0`, no idle hiss or DC drift) |
| 32 simultaneous velocity-127 pad hits | Zero NaN/Inf, oldest-voice or lowest-envelope steal with 2.5 ms fade, output bounded within `±4.0` linear (`<= +12 dBFS`) |
| NaN / Inf in state restore or parameter automation | Sanitized to parameter default at boundary; output replaces any non-finite sample with `0.0` |
| Sample rates 44.1, 48, 88.2, 96, 192 kHz; block sizes 0, 1, 16, 64, 512, 4096 | Block size 0 is a clean no-op; block size 1 and 512 render identical one-shot energy within `±0.1 dB` and identical 808 tail pitch within `±15 cents` |

### Bypass

When `ddl_bypass` is true, incoming MIDI triggers and internal sequencer triggers are muted, active voices ramp to zero over `5 ms` (or immediately on reset), and the stereo output bus writes clean digital silence (`0.0`). UI meters show `−inf dB` and a bypassed badge.

### Latency

`0 samples` reported host latency (`getLatencySamples() == 0`) using minimum-phase IIR / polyphase half-band oversampling in live `SYNTH`/`DESIGN` playback. Maximum tail length reported to host is `4.0 s` to accommodate long Space/Ambient 808 decays.

## UI behaviour

- Header bar: `DISKLORDZ DRUM DESIGN LAB` title, 6-screen workspace selector (`DNA MATRIX` | `DRUM DESIGNER` | `MUTATION LAB` | `DNA SEQUENCER` | `SOUND BROWSER` | `KIT FACTORY`, grouped under `SYNTH` / `DESIGN` / `FACTORY`), sound-profile selector (`gui.key-selector`), A/B toggle, Undo/Redo, `CAPTURE DNA` button, Seed badge, and output `gui.meter`.
- Left column: 16-pad velocity-sensitive `gui.pad-grid` (Bank A, MIDI note badge, choke indicator, mute/solo, per-pad activity LED).
- Center panel:
  - `SYNTH` (`Drum Designer` + `Sound Browser`): Engine selector, Kick & 808 / Percussion primary `gui.rotary-knob` bank, parametric pitch/amplitude `gui.adsr-panel`, and live `gui.waveform` one-shot preview rendered asynchronously on a background worker (never on every raw mouse-drag pixel on the audio thread).
  - `DESIGN` (`DNA Matrix` + `Mutation Lab`): 5-genre 2D `gui.xy-pad` **Hyper DNA Morph Matrix** (`Plugg`, `Rage`, `Hypertrap`, `Hyperpop`, `Digicore` live percentage readouts + `DNA Influence`), Three-layer (`Body`, `Attack`, `Texture`) strips, parameter-lock toggles (`Root`, `Transient`, `Decay`, `Drive`), `CAPTURE -> EVOLVE -> EXPORT` flow, and dual native drag handles (`DRAG WAV` + `DRAG MIDI`) backed by a managed temp directory whose files persist until external drag completion or session cleanup.
  - `FACTORY` (`DNA Sequencer` + `Kit Factory`): 16/32/64-step bounce & per-step DNA morph lane (e.g., Plugg hat on step 1 morphing to Digicore hat on step 5), `1 -> 231` Evolution batch mode (`1` original + `5` genre versions + `25` curated mutations + `75` velocity layers + `125` XY morph grid renders, filtered through similarity & QC gates), format controls (16/24-bit PCM, 32-bit float, 44.1/48/96 kHz), and `BUILD KIT / EXPORT ZIP`.

## MIDI / sidechain

- **MIDI input:** Required. Bank A defaults to GM drum notes `36..51` (`C1..D#2` in C3=60 convention), user-remappable per pad and persisted in plugin state. Below the pad octave, **Roll Trigger Keyswitches** on MIDI notes `24..27` (`C0..D#0`: `1/16`, `1/32`, `1/32 Triplet`, and `Triggerman Fill`) retrigger any held hat or rim pad sample-accurately inside `processBlock` without heap allocation (`std::vector` is used only in offline `exportToMidiFile`, never on the audio thread). Note-On velocity `1..127` scales amplitude and Attack/FM index modulation; Note-Off is honoured for gated/choked voices; `All Sound Off` (CC 120) and `All Notes Off` (CC 123) fade all active voices cleanly.
- **Sidechain:** None in V1.

## Presets

- Factory programs:
  1. `Init — Clean Kit`
  2. `Detroit Street Lab` (Detroit Punch 808, dry clap, woody rim, 6-osc hat, laser, bounce 142 BPM)
  3. `Classic Plugg Bank A` (Soft Plugg 808, crisp clap, Plugg Crystal Hat, PluggnB Digital Bell)
  4. `PluggnB Velvet Kit`
  5. `Dark Plugg Industrial`
  6. `Redline Rage Kit` (Redline 808 `F#1`, Hard Rage Kick, Digital Snare, Hyper Clap, Razor Hat, Alien Perc)
  7. `Chaos & Alien Rage` (Chaos 808 `E1`, Alien Slide 808 `A1`, Digital Rim `400/1200 Hz`, Rage FX, 1/32 stutter hats)
  8. `Memphis Tape Vault` (Memphis Tape Snare, gritty cowbell, saturated 808)
  9. `Drift Phonk Overdrive`
  10. `Southern Trap Heavy`

## Acceptance (Phase 4 — forward reference)

```bash
tools/build-vst.sh DrumDesignLab
python3 vst-testing-ops/run_business.py --profile factory --plugin DrumDesignLab
```

**Behavioural checks (numbered, measurable, with tolerances):**

1. **Deterministic seed & phase reset:** Calling offline one-shot render twice with identical recipe and seed produces bit-identical float buffers (`max_abs_diff == 0.0`), including hi-hat and noise layers.
2. **Polyphonic voice isolation:** Triggering Pad 1 (`808`) at sample `0` and Pad 4 (`Clap`) at sample `128` produces sample-accurate onset at index `128` while Pad 1 samples `0..127` match a solo Pad 1 render bit-for-bit.
3. **Choke group click-freedom:** Triggering Open Hat followed `40 ms` later by Closed Hat in the same choke group attenuates Open Hat by `>= 60 dB` within `3.5 ms` with no sample-to-sample discontinuity exceeding `0.25` full-scale.
4. **808 tail pitch & clean-sub retention:** Rendering `Detroit Punch 808` (`49.00 Hz`), `Redline 808` (`46.25 Hz`), `Chaos 808` (`41.20 Hz`), and `Alien Slide 808` (`55.00 Hz`) yields stable-tail autocorrelation pitch within `±25 cents` of target at `44.1 kHz` and `48 kHz`, while sub-band (`25–90 Hz`) mono correlation remains `>= +0.98` even at maximum Dirt blend.
5. **Offline WAV export & QC bounds:** Atomic 24-bit WAV export via `AudioFormatWriter::writeFromAudioSampleBuffer` reopens cleanly, contains zero NaN/Inf, `|DC offset| <= 0.001`, and 4x oversampled true peak `<= ddl_export_ceiling_db + 0.05 dBTP`.
6. **Cumulative-drift-free sequencer:** Scheduling 64 sixteenth-note steps at `142 BPM` and `48 kHz` places step 63 within `±1 sample` of `round(63 * (60 / 142 / 4) * 48000)`.

## Out of scope (V1)

- Loading user WAV/AIFF files into pads (synthesis-only architecture).
- Live audio-input resynthesis or sidechain compression.
- Multi-bus 16-stereo-out DAW mixer routing (scheduled after V1 stereo instrument + offline stem exporter ship).

## Dependencies (conceptual only)

- DSP: `osc.sine`, `osc.saw`, `osc.square`, `osc.wavetable`, `env.adsr`, `lfo.sine`, `filter.ladder`, `filter.biquad`, `filter.hpf`, `fx.saturation`, `fx.delay`, `dyn.compressor`, `analysis.peak_rms`
- GUI: `gui.pad-grid`, `gui.rotary-knob`, `gui.waveform`, `gui.adsr-panel`, `gui.meter`, `gui.key-selector`, `gui.xy-pad`

## Links

- Constitution: [../DPCS_PHASE0_CONSTITUTION.md](../DPCS_PHASE0_CONSTITUTION.md)
- Parameters: [parameter-spec.md](parameter-spec.md)
