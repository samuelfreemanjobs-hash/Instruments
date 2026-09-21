# SP-1200 Drumulator — QA checklist (v1)

Manual pass against [SP1200_STANDALONE_SPEC.md](SP1200_STANDALONE_SPEC.md). Automated: `ctest --test-dir build -R SP1200`.

## Audio & memory

- [ ] Import WAV/AIF/FLAC → mono sum, **26.040 kHz**, 12-bit; memory label stays **/ 7:00 TOTAL**
- [ ] Input record ARM → commit creates segment; cap enforced at 7:00
- [ ] **VINYL 33→45 import** toggle: brighter import + pad tune ~−5 st on assign
- [ ] 16 voices; same-pad retrigger; choke group cuts siblings

## Performance

- [ ] 16 pads trigger samples; faders VOL / PITCH / DECAY modes
- [ ] Multi-pitch cap (16 slots) when enabled
- [ ] Bus SSM2044 cutoff/resonance + step filter stack affects playback
- [ ] Hi-trim on pads 3–4 and 11–12 (brighter hats/rims vs kick row)

## Sequencer

- [ ] Patterns 1–99, bars 1–4; piano roll toggle steps; step stacks (vel/pitch/pan/filter)
- [ ] Console step record; swing + BPM affect timing
- [ ] Song chain **END** stops; loop wraps slots 0–7 when enabled
- [ ] MIDI clock **slave**: external F8 advances steps (6 per 1/16)
- [ ] MIDI clock **master**: F8 emitted while playing

## MIDI & keyboard

- [ ] SQ-1 default ch **10**, notes **36–51**, CC **20–35**
- [ ] **Learn NOTE / Learn CC** per pad; **Reset MIDI maps**
- [ ] Omni + channel filter in **10 SETUP**
- [ ] Keyboard: Space/Esc/R/Tab; Q–I / A–K pads

## Project

- [ ] Save/load `.sp12p` round-trip (samples, patterns, song, MIDI maps, setup)

## UI modules

- [ ] **MOD 11** chop modal: truncate, auto 8/16, preview
- [ ] **11 CHOP** tab embeds editor; console overlay still works
- [ ] Swing changes odd/even 1/16 spacing (96 PPQN ±12 ticks); slave MIDI uses 3/9 clocks at max swing
- [ ] Tabs: **10 CONSOLE**, **12–14 PROG**, **MOD 20**, **24 SONG**, **15 SSM2044**, **10 SETUP**
- [ ] **MOD 12/13/14**: faders retarget pitch/decay/mix; LCD scrub on selected pad
- [ ] **MOD 15**: bus SSM sliders; pad 3/4/11/12 hi-trim label; LCD ±1 + YES/NO scrub
- [ ] **Clear pattern** prompts CONFIRM on LCD before erase
- [ ] **BANK A–D** selector; imports land in active bank; memory shows **bank / 1:45** quota
- [ ] **LCD** two-line status: module, bank, selected pad + segment name
- [ ] Keys **1–4** select bank on console; pad click selects pad for MOD 11
- [ ] **MOD 30 COMBINE**: two pads with segments → one longer segment; memory decreases
- [ ] **SEG → BANK** moves segment bank tag; bank quota line updates
- [ ] **Keypad** / digits: ENTER pattern (seq tab) or bank 1–4 (console)
