# Project rules: JUCE 8 & C++20 VST3 instrument development

Normative for **Windows desktop** VST3/Standalone instruments. The **MPC Sample / Melodee** lane (`vst/Mpc*`, Linux aarch64) may use the pinned `vst/JUCE` submodule (often JUCE 7.x) until upgraded — RT rules in §2 apply to **all** plugins.

## 1. Environment & build commands (Windows)

- **Target standard:** C++20 (`CMAKE_CXX_STANDARD 20`, `CMAKE_CXX_STANDARD_REQUIRED ON`) with **JUCE 8+** and **CMake 3.22+**.
- **Plugin formats:** Build **VST3** and **Standalone** targets.
- **Configure:**

  ```powershell
  cmake -B build -G "Visual Studio 17 2022" -A x64
  ```

- **Build:**

  ```powershell
  cmake --build build --config Release
  ```

- **Verification loop:** After any change to `.cpp`, `.h`, or `CMakeLists.txt`, run the build and clear **all** MSVC errors and warnings before finishing.
- **System VST3 install:** Copy to `C:\Program Files\Common Files\VST3\` only when the user requests it or during final packaging — default **`COPY_PLUGIN_AFTER_BUILD=OFF`** (see `.cursor/rules/windows-worker.mdc`).

**Linux / Melodee (this repo):** `./tools/build-vst.sh <Name>`, `MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh <Name>` — see `docs/MPC_SAMPLE_VST3.md`.

## 2. Real-time audio thread safety (strict)

Applies to `processBlock()`, `renderNextBlock()`, and every helper called from the audio thread:

| Rule | Requirement |
|------|----------------|
| **Zero heap allocation** | No `new`/`delete`/`malloc`/`free`, no `vector::push_back`/`resize`, no `AudioBuffer::setSize`, no `juce::String` construction |
| **Zero blocking** | No mutex, `CriticalSection`, `lock_guard`, file I/O, or logging (`DBG`, `std::cout`) |
| **Denormals** | `juce::ScopedNoDenormals` at the top of `processBlock()` |
| **Parameters** | Read via `apvts.getRawParameterValue("id")` atomics; cache pointers in `prepareToPlay` if needed |
| **Smoothing** | Continuous controls (cutoff, pitch, resonance, gain, mix) through `juce::SmoothedValue<float>` with **10–20 ms** ramps |

## 3. Synthesizer voice & MIDI architecture

- **Preferred:** `juce::Synthesiser` + `juce::SynthesiserVoice` with clean voice stealing (no clicks).
- **Envelopes:** `adsr.noteOn()` in `startNote()`, `adsr.noteOff()` in `stopNote()`; clear the voice only when `!adsr.isActive()`.
- **juce::dsp:** Call `prepare(ProcessSpec)` on oscillators, filters, and effects in `prepareToPlay()` before audio runs.

**Melodee headless plugins** (`MpcMessenger`, `MpcDxFm`, `MpcCs80`, …) currently use fixed `std::array` voice pools — valid for embedded poly limits; migrate to `Synthesiser` when refactoring for JUCE 8 desktop targets.

## 4. UI & state management (JUCE 8)

- **APVTS** is the parameter source of truth; persist with `getStateInformation` / `setStateInformation` (XML via `copyXmlToBinary` / `getXmlFromBinary`).
- **UI thread:** No DSP in `paint()` / WebView; use `SliderAttachment`, `WebSliderRelay`, or lock-free FIFOs for meters.
- **APIs:** Prefer JUCE 8 signatures; confirm via docs/MCP before using deprecated JUCE 6/7 APIs.

## 5. Multi-agent division of labor

| Agent | Owns |
|-------|------|
| **Claude Code / terminal** | Multi-file scaffolding, CMake, MSVC builds, `pluginval` CLI |
| **Cursor (IDE / Cloud)** | Inline DSP edits, WebView UI, git review, Melodee aarch64 cross-build |
| **plugin-factory-liaison** | Prompts, gates — no C++ |
| **linux-vst3-dsp-engineer** | Linux JUCE `vst/Mpc*`, APC test/ship |

## Related docs

- `docs/CLAUDE_VST_ENGINEER.md` — RT rules mirror + session bootstrap  
- `.cursor/rules/windows-worker.mdc` — Windows machine limits  
- `docs/MPC_SAMPLE_VST3.md` — aarch64 Melodee bundles  
- `docs/dsp-architecture-specification.md` — program vs surface chain  