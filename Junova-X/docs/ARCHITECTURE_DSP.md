# Junova-X DSP architecture

## Pipeline

```mermaid
flowchart LR
  MIDI[MIDI input] --> ARP[Arpeggiator]
  ARP --> ENG[SynthEngine]
  ENG --> OUT[Stereo out + ScopeFifo]
  APVTS[APVTS params] --> ARP
  APVTS --> ENG
  HOST[PlayHead BPM] --> ARP
```

| Stage | Class | Thread | Allocations |
|-------|--------|--------|-------------|
| MIDI arp | `Source/DSP/Arpeggiator.*` | audio | none in `process` |
| Voices + VCF | `Source/DSP/SynthEngine.*` | audio | none in `render` |
| Chorus | `Source/DSP/BbdChorus.*` | audio | none |
| Test tone | `Source/DSP/DiagTone.*` | audio | none |
| Scope | `Source/DSP/ScopeFifo.h` | audio → UI timer | lock-free FIFO |

## Arpeggiator (WO-2026-004)

- **Bypass:** `arpRate ≤ 0.01` passes MIDI unchanged.
- **Held notes:** up to 16; sorted upward pattern.
- **Range:** `arpRange` 1–4 octaves (UI fader 1–4).
- **Rate:** normalized fader maps to step length vs **host BPM** (fallback 120).
- **Future:** sample-accurate step placement; sync to PPQ / swing; patterns (down, random).

## SynthEngine

- **8 voices** — poly / mono (glide) / unison (detune spread).
- **DCO** — saw + PWM mix, sub sine, noise.
- **VCF** — `StateVariableTPTFilter` per voice; cutoff from base + filter ADSR + LFO + key track.
- **Master** — width, HPF (optional), `BbdChorus`, smoothed gain.

## Parity roadmap (iPlug2 reference)

| Subsystem | MVP | Next |
|-----------|-----|------|
| DCO waveforms | saw/pwm/sub | full Juno wave blend |
| VCF | SVF ladder-ish | IR3109-style nonlinear |
| Chorus | dual delay BBD | clock-noise + stereo spread |
| Env | dual ADSR | cross-mod, velocity |
| Arp | up, BPM | host sync PPQ, latch |

## Tests

```bash
cmake --build build -j --target JunovaXTests
ctest -R JunovaXArpeggiator --test-dir build --output-on-failure
```
