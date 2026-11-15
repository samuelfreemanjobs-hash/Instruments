# MpcLaSynth — APC plan

**Next:** `/apc-design MpcLaSynth`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 2 | 8 Melodee / 16 desktop |
| Oscillators/voice | 2 | PCM attack + loop body |
| Filters | 1 | TVF post-sum |
| Modulation | 1 | LFO pitch optional |
| FX | 1 | light chorus |
| Surfaces | 1 | headless + programs |
| Targets | 1 | |
| **Total** | **9** | |

**Tier:** **L2**

**Risk flags:**

- **Repitched PCM** quality — keep attacks short; cap repitch range.
- **Binary size** on SD — ship one plugin bundle; PCM inside `.so` is fine.
- **Memory** — 8 voices × longest loop buffer reference only (shared tables).

**Melodee fit:** Matches **90s stack** timbres without FM programming or heavy VA poly.

## UI framework

`headless` — K1=body, K2=attack, K3=cutoff.

## Strategy

1. Design: wave list + program table + embed script contract.
2. Impl: `LaVoice` before chorus; reuse chorus from **MpcJuno106** if already built.
3. Test: golden render House Piano 1s MIDI note; size check on aarch64 `.so`.

## Ecosystem

| SKU | Role on SD card |
|-----|-----------------|
| MpcLaSynth | House piano, choir, bells |
| MpcDxFm | FM EP / digital bells |
| MpcSh101 | analog acid bass |