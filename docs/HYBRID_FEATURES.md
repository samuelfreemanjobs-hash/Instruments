# Hybrid feature map (Maschine + Ableton + FL + Electribe)

## Multi-monitor UI

On launch, the app opens **five windows** (Electribe, MPC, Maschine, Session, Pattern). Each is placed on a connected display (round-robin); multiple modes on one monitor are tiled. **Click or focus a window** to set the active audio engine path (Electribe vs MPC sequencer). Closing all windows quits the app.

## Modes

| Mode | Primary UX | Model |
|------|------------|--------|
| **Electribe** (default) | 16-step row + 8 parts, motion toggle | `ElectribeSong`, `ElectribeSequencer` |
| **MPC** | LCD + 4×4 grid + piano roll | `SixteenStepPattern`, `SequencerCore` |
| **Maschine** | 8 groups, 16 pads, scenes | `MaschineKit` |
| **Session** | 8×8 clip matrix, launch state | `SessionMatrix` |
| **Pattern** | Pattern buttons + playlist blocks | `PatternPlaylist` |

## Feature highlights (roadmap depth)

### Maschine (Native Instruments)
- Groups A–H with 16 pads each
- 16 scenes recalling pattern indices per group
- Keyboard mode flag (UI hook)
- *Next:* choke groups, macro knobs, sampling from pads

### Ableton Live
- Session clip grid with play state
- Launch quantization (beats)
- Session record flag
- *Next:* follow actions, clip envelopes, Arrangement view

### FL Studio
- Multiple step patterns
- Playlist blocks with color by pattern
- Song vs pattern mode toggle
- *Next:* channel rack, piano roll per pattern, audio clips in playlist

### KORG Electribe
- **Main mode:** step sequencer drives transport
- 8 parts × 16 steps (velocity, accent fields in model)
- Motion sequence enable (automation latch — Phase 5)
- All parts fire on each step (unmuted)

Audio routing: Electribe sequencer runs for all modes except **MPC**, which uses the legacy 16-step core.
