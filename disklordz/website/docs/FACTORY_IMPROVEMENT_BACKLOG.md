# Factory improvement backlog (daily rotation)

Themes align with `FACTORY_DAILY_THEME` in [docs/DISKLORDZ_FACTORY_DAILY_AGENT.md](../../../docs/DISKLORDZ_FACTORY_DAILY_AGENT.md).

## kick_808_sub_click

- Tighter pitch envelope; less click on very low BPM
- Preset-specific kick pitch offsets (DL006 screw)
- Optional 909-style long tail mode at high `kickDecay`

## snare_clap_snap

- Bandpass snap tuning vs `snareSnap` param
- Clap stereo micro-delays pre-master
- Rim + snare layer balance in loops

## hats_metallic_motion

- Reduce harsh static; more 808 metal partial balance
- Open hat length vs `hatDecay` param coupling
- 32nd rolls sensitivity vs `wildness`

## master_bus_lanes

- Studio vs creative grit curves
- Parallel punch amount vs lane presets
- Quality gate thresholds (clip/silence)

## loops_patterns

- Boulevard vs screw bar patterns
- Swing amount vs BPM
- SFX impact stack vs length tiers

## product_pack_consistency

- Variation spread across folder counts
- Normalized loudness across pack WAVs
- Provenance / metadata consistency

## prompt_params_rag

- New prompt tokens → `DrumParams`
- RAG exemplars for factory prompts
- Wildness / stereo interaction tests

## Icebox (not daily-sized)

- Async job queue for long renders
- Stable Audio worker integration
- JUCE offline Wave909 render
- Golden WAV spectral regression in CI
