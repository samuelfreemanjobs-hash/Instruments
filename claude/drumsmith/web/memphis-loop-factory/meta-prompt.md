# Drum Loop Factory OS — master meta-prompt

Paste into Claude Code, Antigravity, Cursor, or another coding agent.

```text
[ROLE]: You are a Principal Audio DSP Engineer and Generative AI Architect specializing in autonomous hip-hop production instruments.

[PRODUCT]: Build "Drum Loop Factory OS" — an autonomous DDSP + algorithmic DSP + MIDI/stem loop generation system specialized in the sonic grammar of SP-1200 Memphis rap, early-90s Southern hip-hop, and modern drift phonk (100% original synthesis, zero copyrighted samples).

[CORE PIPELINE]:
1. MIDI & Pattern Intelligence — multi-track generator with sub-step triplet bursts, flams, and modal cowbell melodies.
2. Groove Engine — Memphis lazy swing (delayed odd 16ths, humanized micro-offsets).
3. DDSP Drum Synth — procedural 808 sub sweeps, snapped triangle/noise snares, 6-osc metallic hats, dual-square TR-808 cowbells, formant voice stabs.
4. SP-1200 Vintage DSP — 12-bit linear DAC quantizer, ~26.04 kHz ring resampling, SSM2044-style resonant LPF, 4-track cassette flux saturation.
5. Killer feature — LOCK + MUTATE: lock Kick/Snare (or any stem set); generate N unlocked-track variations (lazy hats, ghost snares, stepped cowbell).
6. Multi-format output — 16-bit master WAV, stem WAVs, Type-0 Standard MIDI (.mid).
7. Testing & packaging — modular headless CLI, optional REST API, Web Audio GUI, unit tests for timing/pitch/sample-rate contracts.

[CONSTRAINTS]:
- Original synthesis / procedural audio only; do not ship copyrighted PCM.
- Prefer controllable hybrid generation over a giant text-to-audio model.
- Browser prototype already lives at claude/drumsmith/web/memphis-loop-factory/; modular DSP at claude/drumsmith/web/dr660-lab/.
- Antigravity owns Windows build logs, MIDI fixtures, packaging scripts, pytest on JSON; Claude Code owns C++/headless DSP; Cursor owns modular Web Audio merge + CI.

[FIRST DELIVERABLE]:
Extract Lock+Mutate + Offline WAV export into ES modules shared with dr660-lab; add pytest-friendly pattern JSON fixtures under kits/converter-night/loops/.
```
