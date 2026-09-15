# VMPC2000XL Modern Hybrid DAW — Development Roadmap

Cross-platform (macOS/Windows/Linux for development), low-latency, real-time MPC2000XL-inspired hybrid DAW.

| Phase | Focus | Status |
|-------|--------|--------|
| 1 | Tech stack & MVC architecture | **In progress** (this repo) |
| 2 | Graphics & UI (no audio) | Stubs + shell layout |
| 3 | Sequencer engine (PPQN, swing, 16-step) | Model stubs |
| 4 | Audio engine & DSP | Not started |
| 5 | Integration, Q-Link, persistence | ValueTree scaffold |
| 6 | Polish, metering, export | Not started |

**Stack:** C++20, [JUCE](https://juce.com/), strict separation of audio thread vs GUI thread.

Estimated professional timeline for full parity with the design mockup: **18+ months** of focused audio/GUI engineering.
