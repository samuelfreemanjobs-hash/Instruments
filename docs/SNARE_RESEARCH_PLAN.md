# Snare research plan (TRAP-FORGE / Memphis)

Research backlog for layered trap snares and clap stacks. Use this when extending browser TRAP-FORGE, Memphis Architect, or the TRAP-FORGE JUCE shell.

## Layers to validate

1. **Membrane + noise body** — band-limited noise with short amp decay; HPF ~100–120 Hz to keep sub clean.
2. **808 clap pitch stack on snare** — layer a short 808-style clap burst **+2 to +4 semitones** above the snare body for Southside / Metro width (default research target: **+3 st**). Tune per preset; document final semitone offset in factory JSON `studio.snareClapSemi`.
3. **Flam / stereo spread** — dual-hit clap with 8–15 ms offset and ±15–25 ms pan for phonk stacks.
4. **Phase alignment** — align clap transient peak to snare noise attack (same `phaseAlignMix` contract as kick layers in TRAP-FORGE elite JS).
5. **Export parity** — compare browser WAV peak/RMS to native VST3 offline render within golden tolerances once snare path lands in `DrumRender.cpp`.

## References

- [disklordz/trap-forge/schema/trapforge-preset.schema.json](../disklordz/trap-forge/schema/trapforge-preset.schema.json) (when present on branch)
- [disklordz/memphis-architect/ARCHITECTURE.md](../disklordz/memphis-architect/ARCHITECTURE.md)
