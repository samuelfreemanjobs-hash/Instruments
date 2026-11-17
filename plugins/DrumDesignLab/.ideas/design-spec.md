# DrumDesignLab — design spec (APC Design)

**UI framework:** `juce-native`
**Design system:** theme **`disklordz` v1.0.0**, contract **factory-spec/v2** (`vst/factory-lib/design-system/`). All colours, type and sizes cite design-system role names (graphite/obsidian chassis, violet accent role for DNA, crimson/danger role for Rage/Cinematic aggression, cyan/info role for Hyperpop; never raw hex in spec).
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [architecture.md](architecture.md)
**SOP:** [SOP-VF-02 UI design](../../../docs/disklordz/sops/SOP-VF-02-ui-design.md)

Coordinates below are reference proportions at `1120 × 720` logical px (`100%` scale, resizable `960×620` to `1920×1200`, `100–200%` Windows DPI scale via responsive `FlexBox`/`Grid` — no brittle hardcoded pixel walls).

## 1. Window

| Property | Value |
|----------|-------|
| Default size | `1120 × 720`, resizable (`960×620` .. `1920×1200`) |
| Background | `bg.chassis` |
| Panel surface | `bg.panel` / `bg.elevated` with `border.subtle` |
| Padding | `size.windowPadding` |
| UI scale | `100% – 200%` host & standalone DPI scaling |
| Keyboard | `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` |

## 2. Layout

```text
+--------------------------------------------------------------------------------------------------+ (0,0 .. 1120,52)
| DISKLORDZ DRUM DESIGN LAB | [SYNTH][DESIGN][DNA][GROOVE][FACTORY] | Profile | A/B | Seed | Meter |
+-------------------------------+------------------------------------------------------------------+ (0,52 .. 1120,668)
| LEFT RACK (w: 320)            | MAIN WORKSPACE (responsive Flex/Grid, w: 800)                    |
| +---------------------------+ | +--------------------------------------------------------------+ |
| | 4x4 PAD GRID (Pads 1..16) | | | WAVEFORM / SPECTRUM SCOPE + PITCH/TAIL READOUT               | |
| | Note | Choke | Vel LED    | | | [CAPTURE DNA]  [MUTATE]  [DRAG WAV -> DAW]  [DRAG MIDI ->]   | |
| +---------------------------+ | +--------------------------------------------------------------+ |
| | SELECTED PAD SUMMARY      | | | MODE VIEWPORT:                                               | |
| | Engine | Destruction Tier | | |  - SYNTH/DESIGN: 3 Horizontal Layer Rows (BODY/ATTACK/TEX)   | |
| | Root Hz | Choke Group     | | |    [Enable][Mute][Solo][Lock] + Pitch/Decay/Drive/Filter     | |
| +---------------------------+ | |  - DNA: 5-Node 2D XY Matrix (Plugg/Rage/Hypertrap/Pop/Digi)  | |
|                               | |  - GROOVE: 16/32/64-Step Grid + Swing + Bounce Sync + Rolls  | |
|                               | |  - FACTORY: Cinematic/Genre Pack Builder + QA Report + ZIP   | |
|                               | +--------------------------------------------------------------+ |
+-------------------------------+------------------------------------------------------------------+ (0,668 .. 1120,720)
| STATUS FOOTER: Active Voices / CPU % | Sample Rate | Export Path | Bypass | Master Output Knob   |
+--------------------------------------------------------------------------------------------------+
```

### 2.1 Regions

| Region | Default Rect | Component | Notes |
|--------|--------------|-----------|-------|
| Header | `0, 0, 1120, 52` | `HeaderBar` + Mode Tab Strip | Brand title, 5 mode buttons, Profile ComboBox, A/B, Undo/Redo, output `LevelMeter` |
| Left Pad Rack | `12, 60, 304, 600` | `PadGridPanel` | 4×4 velocity-sensitive pads (Bank A `36..51`), choke badge, selected-pad summary |
| Waveform & Export Bar | `328, 60, 780, 168` | `WaveformScopeBar` | Async one-shot oscilloscope (`0.45 * sr` window), root/TP readout, `MUTATE`, `DRAG WAV`, `DRAG MIDI` |
| Mode Workspace | `328, 238, 780, 422` | `ModeViewport` | Switches cleanly between 3-Layer Designer, 2D Hyper DNA XY Matrix, Step Sequencer, and Kit Factory |
| Footer | `0, 668, 1120, 52` | `FooterStatusStrip` | Voice count, CPU load %, `ddl_bypass`, `ddl_master_gain_db` |

## 3. Control and display map

| Widget | Binds to | Component | Tooltip |
|--------|----------|-----------|---------|
| Mode Tabs | `ddl_mode` | Segmented tab bar | Workspace mode (`SYNTH`, `DESIGN`, `DNA`, `GROOVE`, `FACTORY`) |
| Profile Selector | `ddl_profile`, `ddl_hyper_family` | `ComboBox` (`gui.key-selector`) | Selects Cinematic Shadows mood or regional/Hyper engine bank |
| Destruction Switch | `ddl_destruction_mode` | 3-way pill switch | Clean / Mixed / Destroyed harmonic staging |
| 16-Pad Grid | `ddl_selected_pad`, `ddl_choke_group` | `PadGridPanel` (`gui.pad-grid`) | Click velocity audition; right-click MIDI note remap |
| Synth & Bass Reactor Knobs | `ddl_engine`, `ddl_bass_design`, `ddl_length_tier`, `ddl_sub_freq_hz`, `ddl_punch_freq_hz`, `ddl_pitch_sweep_ms`, `ddl_amp_decay_ms`, `ddl_glide_ms`, `ddl_wave_morph` | `RotaryKnob` | Fundamental, punch sweep, glide, decay, and waveform morph |
| Layer Strips (`BODY`, `ATTACK`, `TEXTURE`) | `ddl_body_level`, `ddl_attack_level`, `ddl_texture_level` + layer Mute/Solo/Lock | `LayerRowComponent` | Independent 3-layer gain, mute, solo, and seed/param lock |
| Tone, FM & Grit Knobs | `ddl_filter_cutoff_hz`, `ddl_filter_reso`, `ddl_fm_ratio`, `ddl_fm_index`, `ddl_drive`, `ddl_sub_crossover_hz`, `ddl_dirt_blend`, `ddl_sampler_grit`, `ddl_oversample` | `RotaryKnob` + selector | Split-band crossover, oversampled drive, FM, and 12-bit sampler grit |
| Transient & Space | `ddl_transient_punch`, `ddl_transient_sustain`, `ddl_stereo_width`, `ddl_reverb_mix` | `RotaryKnob` | Dual-envelope transient shaper and mono-sub-safe stereo space |
| Hyper DNA 2D Matrix | `ddl_dna_x`, `ddl_dna_y`, `ddl_dna_influence`, `ddl_dna_seq_mod` | `HyperDNAXYPad` (`gui.xy-pad`) | 5-genre 2D morph surface with live weight percentages & lock mask |
| Mutation Controls | `ddl_seed`, `ddl_mutate_intent`, `ddl_mutate_strength`, `ddl_ab_slot` | Seed box + `RotaryKnob` + buttons | Deterministic seeded mutation, A/B compare, Undo/Redo |
| Groove Sequencer | `ddl_seq_enabled`, `ddl_seq_steps`, `ddl_bpm`, `ddl_swing`, `ddl_bounce_sync_ms`, `ddl_roll_mode`, `ddl_humanize` | `SequencerPanel` | 16/32/64-step pattern grid, swing, Louisiana bounce shift, hat rolls |
| Factory & Master | `ddl_kit_tier`, `ddl_export_ceiling_db`, `ddl_master_gain_db`, `ddl_bypass` | `FactoryPanel` + `FooterStatusStrip` | Batch kit size, offline dBTP ceiling, live master trim, host bypass |

## 4. States

| State | Visual (roles) |
|-------|----------------|
| Normal | `bg.chassis` / `bg.panel`, active profile accent (`accent.primary` for DNA/Plugg, `status.danger` for Rage/Cinematic Aggressive, `accent.secondary` for Hyperpop) |
| Bypassed | Dimmed workspace (`text.muted`), header `BYPASSED` badge, output meter at `-inf` |
| Layer Muted / Soloed / Locked | Explicit text + icon badges (`M`, `S`, `LOCK`) — never colour alone |
| Exporting / Drag Ready | Progress bar + `READY TO DRAG` pill when temp WAV/MIDI file is flushed on disk |

## 5. Interaction & External Drag-and-Drop

- `DRAG WAV` and `DRAG MIDI` write atomically to a managed session subfolder via `AudioFormatWriter::writeFromAudioSampleBuffer` / SMF writer on the background worker, then invoke `juce::DragAndDropContainer::performExternalDragDropOfFiles` on mouse drag. Files remain alive until session teardown so FL Studio, Reaper, or Ableton never read a deleted path.
- Keyboard navigation: full Tab order across header -> pad grid -> layer rows -> active mode panel -> footer; all interactive targets `>= 28 px`.

## 6. Accessibility

- Every knob, pad, toggle, and XY pad sets explicit `setTitle` / `setDescription` screen-reader strings.
- Mute, solo, lock, choke, and clipping states pair text/shape indicators with colour roles.
