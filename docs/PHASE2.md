# Phase 2 — Graphics & UI (completed foundation)

## New components

| Component | Purpose |
|-----------|---------|
| `HardwareShellFrame` | Walnut cheeks + matte bezel around mode content |
| `QLinkSliderStrip` | Four vertical Q-Link controls (values stored on `ValueTree`) |
| `MixerBank` | Four `MixerChannelStrip` instances (DRUM/BASS/LEAD/FX) |
| `SevenSegmentDisplay` | BPM and step readouts |

## Mode integration

- **MPC mode:** shell + LCD + grid + piano roll + mixer bank + Q-Link + 7-segment BPM.
- **Electribe mode:** 16-step row, part/mute rows, swing slider, accent (right-click step), BPM/step LEDs.

## Next (remaining Phase 2 polish)

- Match hardware mockup dimensions and studio emblem artwork.
- Q-Link → DSP parameter binding (Phase 5 automation).
