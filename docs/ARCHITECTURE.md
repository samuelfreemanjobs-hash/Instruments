# Architecture

## Pattern: MVC + audio engine

```
┌─────────────────────────────────────────────────────────────┐
│  GUI thread (View + Controller)                             │
│  MainComponent, custom Components, VMpcLookAndFeel          │
│  AppController ──reads/writes──► ProjectState (ValueTree)   │
└───────────────────────────┬─────────────────────────────────┘
                            │ lock-free / Atomic / AsyncUpdater
┌───────────────────────────▼─────────────────────────────────┐
│  Audio thread (Model playback side)                         │
│  AudioEngine : AudioIODeviceCallback                        │
│  SequencerCore tick → MIDI/events → (future) sampler/DSP    │
│  NO: allocation, locks, UI, file I/O, logging               │
└─────────────────────────────────────────────────────────────┘
```

## Directories

| Path | Role |
|------|------|
| `Source/Model/` | Sequencer state, patterns, project ValueTree |
| `Source/View/` | JUCE Components and LookAndFeel only |
| `Source/Controller/` | Wires UI events to model; never runs on audio thread |
| `Source/Audio/` | `AudioEngine` and future DSP (real-time safe) |
| `Source/App/` | `JUCEApplication`, root layout |

## Real-time rules (`Source/Audio/RealTimeConstraints.h`)

Documented constraints for anything called from `audioDeviceIOCallbackWithContext`.

## Persistence

`ProjectState` wraps `juce::ValueTree` (`VMPC2000XL/Project`) for Phase 5 save/load.
