# Serum / text-to-preset prompts (Trap & phonk drums)

Copy into **Pounding Systems**, **EMMA**, or Serum Forge `semantic.py` / LLM tool prompts. Pair with [`trapforge-preset.schema.json`](../../../disklordz/trap-forge/schema/trapforge-preset.schema.json) when targeting TRAP-FORGE JSON instead of `.fxp`.

## Hard Trap Kick Drum

Hard-hitting Trap kick drum patch. Pure sine wave with a rapid pitch envelope sweeping down from **+4 octaves to -2 octaves in 30ms**. Fast amplifier envelope with zero attack, short decay, and aggressive hard-clipping distortion in the FX rack.

## Deep Phonk / Trap 808 Bass

Deep Trap 808 bass, clean sine wave oscillator with a fast pitch-drop envelope at the start for a punchy transient kick. Add a second oscillator with subtle triangle wave tube saturation for upper-harmonic distortion and long decay.

## Crispy Trap Snare

Crispy Trap snare drum preset. White noise oscillator mixed with a snappy bandpass-filtered triangle wave. High-pass filter at **150Hz**, sharp envelope decay, and a bright plate reverb effect with short decay time.

Research: layer **808 clap +2–4 semitones** on snare body — [docs/SNARE_RESEARCH_PLAN.md](../../../docs/SNARE_RESEARCH_PLAN.md).

## 90s Memphis Phonk Cowbell

Classic 90s Memphis Phonk cowbell lead. High-pitched, metallic sounding square wave with short decay, high resonance bandpass filter, fast tape-delay effect, and a touch of vinyl noise distortion.

## Native parity

| Prompt intent | Monorepo target |
|---------------|-----------------|
| Kick pitch sweep | `tools/drum-synth-blueprint` · TrapForge `DrumRender` · `synth-trap.js` |
| 808 glide + sat | `render_trap_808()` · TrapForge 808 path |
| Snare noise + BP | SNARE_RESEARCH_PLAN · future `DrumRender` layers |
