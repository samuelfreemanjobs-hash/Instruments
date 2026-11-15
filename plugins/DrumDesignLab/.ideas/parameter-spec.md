# DrumDesignLab — parameter spec (DPCS Phase 1)

**APVTS-only contract.** No C++ and no default literals in code until Phase 2 headers.
**SOP:** [SOP-VF-01 Intake & Dream](../../../docs/disklordz/sops/SOP-VF-01-intake-dream.md) · mapping rules: [dsp-architecture-specification.md §7](../../../docs/dsp-architecture-specification.md)

## Global

| Property | Value |
|----------|-------|
| Plugin code (4 chars, unique in repo) | `DlDL` |
| Manufacturer | `Inst` (`Instruments` / DiskLordz Audio per repo CMake convention) |
| Programs | 21 factory: `Init — Clean Kit`, `Detroit Street Lab`, `Classic Plugg Bank A`, `PluggnB Velvet Kit`, `Dark Plugg Industrial`, `Cloud Bounce Vault`, `Heavy ATL Brick Squad`, `Houston Trunk 16-Pad`, `Chicago Drill South Side`, `New Orleans Bounce 98`, `Baton Rouge Trench 162`, `Redline Rage Kit`, `Chaos & Alien Rage`, `Hypertrap — Digital Warfare`, `Hyperpop — Plastic Dreams`, `Hyperplugg — Crystal Bounce`, `Digicore — Internet Drums`, `HexD — Digital Decay`, `Memphis Tape Vault`, `Drift Phonk Overdrive`, `Southern Trap Heavy` |

## Parameters

IDs are lowercase snake_case with prefix `ddl_`. Never rename an ID after release (breaks saved projects). Per-pad full 16-pad recipe arrays, custom MIDI note maps, parameter-lock masks, and undo snapshots persist in a versioned ValueTree state child alongside these host-automatable APVTS controls (which target the focused pad or global rack).

| ID | Display name | Type | Range | Default | Unit | Automatable | Smoothing | Notes |
|----|--------------|------|-------|---------|------|-------------|-----------|-------|
| `ddl_mode` | Mode | choice | `0:SYNTH`, `1:DESIGN`, `2:FACTORY` | `0` (`SYNTH`) | — | no | — | Workspace tab selector |
| `ddl_profile` | Style Profile | choice | `0..23` (`Detroit Trap`, `Classic Plugg`, `PluggnB`, `Dark Plugg`, `Retro Plugg`, `Detroit Plugg`, `Ambient Plugg`, `Cloud Bounce`, `Heavy ATL Brick`, `Houston Trunk`, `Chicago Drill`, `New Orleans Bounce`, `Baton Rouge Trap`, `Rage Redline`, `Rage Chaos`, `Rage Alien`, `Rage Dark Matter`, `Rage Hyperdrive`, `Rage Nebula`, `Hypertrap`, `Hyperpop`, `Memphis`, `Drift Phonk`, `Southern Trap`) | `0` (`Detroit Trap`) | — | yes | — | Loads starting recipe & groove bank (`Hypertrap` & `Hyperpop` are distinct engines) |
| `ddl_hyper_family` | Hyper Series Bank | choice | `0:Off`, `1:Hypertrap`, `2:Hyperpop`, `3:Hyperplugg`, `4:Digicore`, `5:Glitchcore`, `6:HexD`, `7:Sigilkore`, `8:Scenecore`, `9:Dariacore`, `10:DigitalJerk`, `11:NewJazz`, `12:Dreamtrap`, `13:CloudRap`, `14:BubblegumBass`, `15:DeconstructedClub`, `16:NightcoreTrap`, `17:Cybertrap`, `18:ElectropopTrap`, `19:TranceTrap`, `20:ExperimentalDigitalTrap` | `0` (`Off`) | — | yes | — | Selects any of the 20 Hyper Series kit styles on shared DSP modules |
| `ddl_dna_x` | Hyper DNA X | float | `0 … 100` | `50` | % | yes | 20 ms | 2D DNA Morph Matrix horizontal coordinate (`0..1` normalized) |
| `ddl_dna_y` | Hyper DNA Y | float | `0 … 100` | `50` | % | yes | 20 ms | 2D DNA Morph Matrix vertical coordinate (`0..1` normalized) |
| `ddl_dna_influence` | DNA Influence | float | `0 … 100` | `0` | % | yes | 20 ms | Morph intensity from captured original recipe (`0%`) to XY target blend (`100%`) using single-pass log-ratio pitch/time/drive interpolation |
| `ddl_dna_seq_mod` | DNA Step Mod | float | `0 … 100` | `0` | % | yes | 15 ms | Per-step DNA Sequencer modulation depth across steps (e.g. Plugg step -> Digicore step) |
| `ddl_destruction_mode` | Destruction Tier | choice | `0:Clean`, `1:Mixed`, `2:Destroyed` | `1` (`Mixed`) | — | yes | 15 ms | Per-voice Clean / Mixed / Destroyed harmonic staging |
| `ddl_selected_pad` | Selected Pad | int | `1 … 16` | `1` | pad | no | — | Focuses pad for editing without interrupting active voices |
| `ddl_engine` | Synth Engine | choice | `0..11` (`Analog`, `Hybrid`, `FM`, `Physical`, `Additive`, `Subtractive`, `PhaseDist`, `KarplusStrong`, `Modal`, `Waveshape`, `SpectralOffline`, `Foley`) | `0` (`Analog`) | — | yes | — | Selects generator topology for focused pad |
| `ddl_bass_design` | 808 Reactor Type | choice | `0..11` (`Redline`, `Chaos`, `AlienSlide`, `Monster`, `Nuclear`, `Digital`, `Hollow`, `Elastic`, `ShortFuse`, `Ghost`, `Detuned`, `Hybrid`) | `0` (`Redline`) | — | yes | — | Rage Bass Reactor specialized 808 architecture |
| `ddl_length_tier` | Length Variation | choice | `0:Short`, `1:Medium`, `2:Long` | `1` (`Medium`) | — | yes | — | Scales envelope hold/decay per voice family |
| `ddl_sub_freq_hz` | Sub Frequency | float | `35 … 75` | `49` | Hz | yes | 15 ms | Fundamental tail pitch (`49 Hz = G1`) |
| `ddl_punch_freq_hz` | Initial Punch | float | `80 … 250` | `165` | Hz | yes | 10 ms | Pitch envelope start frequency |
| `ddl_pitch_sweep_ms` | Pitch Sweep | float | `5 … 100` | `24` | ms | yes | 10 ms | Exponential pitch decay constant |
| `ddl_amp_decay_ms` | Amp Decay | float | `20 … 2500` | `340` | ms | yes | 15 ms | Covers `28 ms` hats through `750 ms+` Rage/Plugg 808 tails |
| `ddl_glide_ms` | Glide / Portamento | float | `0 … 250` | `0` | ms | yes | 10 ms | Legato pitch slide (`90 ms` on `Alien Slide 808`) |
| `ddl_wave_morph` | Waveform Morph | float | `0 … 100` | `25` | % | yes | 15 ms | `0%` pure sine to `100%` band-limited triangle/pulse |
| `ddl_body_level` | Body Layer | float | `0 … 100` | `85` | % | yes | 15 ms | Fundamental tonal layer gain |
| `ddl_attack_level` | Attack Layer | float | `0 … 100` | `70` | % | yes | 15 ms | Click / snap / punch transient layer gain |
| `ddl_texture_level` | Texture Layer | float | `0 … 100` | `50` | % | yes | 15 ms | Noise / metallic / air / rattle layer gain |
| `ddl_filter_cutoff_hz` | Filter Cutoff | float | `40 … 18000` | `9500` | Hz | yes | 20 ms | Skewed logarithmic frequency mapping |
| `ddl_filter_reso` | Filter Resonance | float | `0.00 … 0.92` | `0.18` | — | yes | 20 ms | Clamped below self-oscillation |
| `ddl_fm_ratio` | FM Ratio | float | `0.50 … 8.00` | `2.50` | x | yes | 15 ms | Modulator-to-carrier ratio (PluggnB Bell default `2.5x`) |
| `ddl_fm_index` | FM Index | float | `0.00 … 6.00` | `2.20` | — | yes | 15 ms | Peak modulation index before exponential decay |
| `ddl_drive` | Drive | float | `0 … 100` | `35` | % | yes | 15 ms | Level-compensated nonlinear waveshaper/wavefolder/tape drive |
| `ddl_sub_crossover_hz` | Sub Crossover | float | `60 … 220` | `110` | Hz | yes | 20 ms | Phase-coherent split keeping clean mono sub intact below crossover |
| `ddl_dirt_blend` | Dirt Blend | float | `0 … 100` | `55` | % | yes | 15 ms | Parallel oversampled harmonic distortion mix on upper band |
| `ddl_sampler_grit` | 12-Bit Sampler Grit | float | `0 … 100` | `0` | % | yes | 15 ms | `26.04 kHz / 12-bit` ZOH hardware sampler + `11.5 kHz` reconstruction LPF wet mix |
| `ddl_oversample` | Oversampling | choice | `0:1x`, `1:4x`, `2:8x` | `1` (`4x`) | — | no | — | Anti-aliasing factor for nonlinear stage |
| `ddl_transient_punch` | Transient Attack | float | `-100 … +100` | `15` | % | yes | 15 ms | Transient sculptor attack emphasis |
| `ddl_transient_sustain` | Transient Sustain | float | `-100 … +100` | `0` | % | yes | 15 ms | Transient sculptor sustain trim |
| `ddl_stereo_width` | Stereo Width | float | `0 … 100` | `30` | % | yes | 20 ms | High-passed above `110 Hz` so sub stays mono |
| `ddl_reverb_mix` | Space Mix | float | `0 … 100` | `10` | % | yes | 20 ms | Bounded stereo plate/room blend |
| `ddl_choke_group` | Choke Group | int | `0 … 4` | `0` | group | yes | — | `0 = Off`, `1 = Hats`, `2 = 808`, `3..4 = User` |
| `ddl_seed` | Recipe Seed | int | `1 … 999999` | `313808` | — | no | — | Deterministic PRNG seed for synthesis & mutation |
| `ddl_mutate_intent` | Mutate Intent | choice | `0:Balanced`, `1:MoreViolent`, `2:MoreFuturistic`, `3:MoreExpensive`, `4:MoreMinimal`, `5:MoreExperimental` | `0` (`Balanced`) | — | no | — | Musically directed RAGE DNA mutation operator |
| `ddl_mutate_strength` | Mutate Strength | float | `0 … 100` | `35` | % | yes | — | DNA spread applied when `Mutate` action fires |
| `ddl_ab_slot` | A/B Compare | choice | `0:A`, `1:B` | `0` (`A`) | — | no | — | Instant recipe snapshot comparison |
| `ddl_seq_enabled` | Sequencer Play | bool | `false / true` | `false` | — | yes | — | Starts/stops internal bounce pattern playback |
| `ddl_seq_steps` | Pattern Steps | choice | `0:16`, `1:32`, `2:64` | `1` (`32`) | steps | no | — | Sequencer loop length |
| `ddl_bpm` | Internal BPM | float | `70 … 200` | `142` | BPM | yes | — | Fallback tempo when host transport tempo is unavailable |
| `ddl_swing` | Bounce Swing | float | `0 … 65` | `16` | % | yes | 20 ms | 16th-note Detroit/Plugg/Rage swing offset |
| `ddl_bounce_sync_ms` | Bounce Syncopation | float | `0 … 35` | `0` | ms | yes | 20 ms | Shifts off-beat hats/rims `0–35 ms` + applies Louisiana bounce ducking curve |
| `ddl_roll_mode` | Hat Roll Division | choice | `0:Off`, `1:1/16`, `2:1/32`, `3:Triplet`, `4:Stutter` | `0` (`Off`) | — | yes | — | Hyper-hat & ratchet roll generator |
| `ddl_humanize` | Microtiming | float | `0 … 100` | `12` | % | yes | — | Seeded velocity & sub-step pocket variation |
| `ddl_kit_tier` | Factory Kit Size | choice | `0:Starter`, `1:Standard`, `2:Mega600` | `1` (`Standard`) | — | no | — | Commercial batch tier (`Mega600` = 600 one-shots + 120 MIDI + 60 loops/stems across `01_808_BASS`..`16_DRUM_KIT_MAPPINGS`) |
| `ddl_export_ceiling_db` | Export Ceiling | float | `-12.0 … -0.1` | `-1.0` | dBTP | no | — | Offline `FACTORY` WAV 4x oversampled true-peak ceiling |
| `ddl_master_gain_db` | Master Output | float | `-24.0 … +6.0` | `0.0` | dB | yes | 20 ms | Live playable output trim (`gain = 10^(db/20)`) |
| `ddl_bypass` | Bypass | bool | `false / true` | `false` | — | yes | 5 ms | Host bypass parameter |

## Smoothing

| Parameter | Time | Domain |
|-----------|------|--------|
| `ddl_sub_freq_hz`, `ddl_punch_freq_hz`, `ddl_filter_cutoff_hz` | 10–20 ms | Hz (exponential/log) |
| `ddl_pitch_sweep_ms`, `ddl_amp_decay_ms` | 10–15 ms | linear ms |
| `ddl_wave_morph`, `ddl_body_level`, `ddl_attack_level`, `ddl_texture_level`, `ddl_drive`, `ddl_transient_punch`, `ddl_transient_sustain`, `ddl_stereo_width`, `ddl_reverb_mix`, `ddl_swing` | 15–20 ms | linear normalized |
| `ddl_master_gain_db` | 20 ms | dB -> linear amplitude |
| `ddl_bypass` | 5 ms | linear crossfade to silence |

## GUI binding (logical)

| Widget (design-system component) | Binds |
|----------------------------------|-------|
| `gui.key-selector` (header mode/profile selectors) | `ddl_mode`, `ddl_profile`, `ddl_engine`, `ddl_oversample`, `ddl_seq_steps`, `ddl_ab_slot` |
| `gui.pad-grid` (16-pad Bank A rack) | `ddl_selected_pad`, `ddl_choke_group`, per-pad MIDI note map & trigger preview |
| `gui.rotary-knob` (Kick/808 & Voice bank) | `ddl_sub_freq_hz`, `ddl_punch_freq_hz`, `ddl_pitch_sweep_ms`, `ddl_amp_decay_ms`, `ddl_wave_morph`, `ddl_filter_cutoff_hz`, `ddl_filter_reso`, `ddl_fm_ratio`, `ddl_fm_index`, `ddl_drive`, `ddl_stereo_width`, `ddl_reverb_mix`, `ddl_mutate_strength`, `ddl_bpm`, `ddl_swing`, `ddl_humanize`, `ddl_export_ceiling_db`, `ddl_master_gain_db` |
| `gui.adsr-panel` (Pitch & Amp trajectory editor) | `ddl_punch_freq_hz`, `ddl_sub_freq_hz`, `ddl_pitch_sweep_ms`, `ddl_amp_decay_ms`, `ddl_body_level`, `ddl_attack_level`, `ddl_texture_level` |
| `gui.xy-pad` (2D 5-Genre Hyper DNA Morph Matrix + transient pad) | `ddl_dna_x` (X), `ddl_dna_y` (Y), `ddl_dna_influence`, `ddl_transient_punch`, `ddl_transient_sustain` |
| `gui.waveform` (Async one-shot preview + drag WAV source) | Focused pad offline preview buffer + `ddl_seed` |
| `gui.meter` (Stereo peak/RMS output bar) | Output bus analysis + `ddl_bypass` state |

## State persistence

- All APVTS parameters plus the versioned `DrumRackState` ValueTree (16 pad recipes, MIDI note mappings `36..51` default, parameter-lock bitmasks, A/B snapshot, 64-step pattern grid, and `schema_version = 1`) save and restore in DAW project recall and preset files.

## MIDI CC

- Standard MIDI `All Sound Off` (CC 120) and `All Notes Off` (CC 123) honoured. Custom CC learn is out of scope for V1.

## Versioning

- `parameter-spec` version: **1**
- Any change requires a PRD delta + operator ack before Phase 2 header sync.
