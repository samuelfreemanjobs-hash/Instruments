# DiskLordz Grim-MIDI — creative brief (APC / Phase 4)

**Product:** MIDI effect · **Lane:** DiskLordz urban launch plan Phase 4  
**Tagline:** Dark Minor & Phrygian Chord Generator with 1-Key Strum

## Elevator pitch

Beatmakers who hit theory walls press **one key** and get **mix-ready dark minor / phrygian voicings** with optional strum. **100 progression presets** (drill/trap bias, many Phrygian Dominant) export as `.mid` for YouTube / Splice content loops.

## User stories

1. As a drill producer, I hold one MIDI note and get a strummed minor 9 with humanized velocity.
2. As a beginner, I use **Degree Pads** (white keys = scale degrees) instead of guessing roots.
3. As a content creator, I drag a progression MIDI clip into my DAW from the plugin GUI or the factory **midi-pack** folder.
4. As a preset shopper, I pick program **47** and the plugin loads that progression's default scale and chord color.

## Non-goals (v1)

- Audio synthesis, arpeggiator sequencer UI, custom scale editor, paid license DRM.

## Success metrics

- Processor contract tests + pluginval 5 green on Windows CI
- 100 exportable MIDI files under `release/GrimMIDI/midi-pack/`
- FL Studio + Ableton smoke notes in `BUILD_WINDOWS.md`
