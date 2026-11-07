# Claude Code — VST Engineer Rules

**Mirror of Cursor's `.cursor/rules/` for Claude Code sessions.**  
Keep this file in sync when you update `vst-engineer.mdc` or `02-dsp-patterns.mdc`.

---

## Session bootstrap

Paste this one-liner at the start of any VST session:

```
Read AGENTS.md and docs/CLAUDE_VST_ENGINEER.md.
Active product: <ProphetRev2Trap | SP1200 | …>
```

---

## 1. Real-time audio constraints (non-negotiable)

These apply to **every audio callback** (`processBlock`, `processSample`, `PROCESS`, `CYCLE`):

| Rule | Detail |
|------|--------|
| **No heap allocation** | No `new`, `malloc`, `std::vector::resize`, or any STL that allocates on the audio thread |
| **No blocking** | No `std::mutex`, `std::lock_guard`, or any syscall that can sleep |
| **No logging** | No `std::cout`, `DBG()`, `printf`, or `JUCE_ASSERT` in the audio path |
| **Parameter smoothing** | All APVTS params must go through `SmoothParam` (one-pole) before hitting DSP — prevents zipper noise on Windows |
| **Cross-thread state** | Use `std::atomic<float>` or a lock-free ring buffer for anything the UI thread writes |

---

## 2. Architecture rules

- **All DSP lives in a single engine header** (e.g. `ProphetRev2Engine.h`, `SP1200Engine.h`). Do not scatter DSP across multiple `.cpp` files.
- `PluginProcessor.cpp` is the JUCE wrapper only: maps APVTS parameters → engine, handles MIDI, calls engine's `processBlock`.
- `PluginEditor.cpp` is UI only: sliders, labels, combo boxes. No audio math here.
- Read `ARCHITECTURE.md` (root) and the product's own `ARCHITECTURE.md` before editing any DSP.

---

## 3. File ownership (multi-agent safety)

Before editing any file, check the product's handoff doc for ownership:

| Handoff doc | Product |
|-------------|---------|
| `docs/PROPHET_REV2_TRAP_HANDOFF.md` | Prophet Rev2 Trap |
| `docs/SP1200_HANDOFF.md` | SP-1200 Drumulator |

### Prophet Rev2 Trap — quick reference

| Owner | Paths |
|-------|-------|
| **Cursor owns** | `ProphetRev2Trap/**`, root `CMakeLists.txt`, `ARCHITECTURE.md` |
| **Claude safe lanes** | `Source/StepSequencer.*`, `ProphetRev2Trap/Tests/`, `.github/workflows/build-prophet-rev2-trap.yml` |

**When in doubt: add new files, never edit Cursor-owned files.**

---

## 4. Build commands

### Prophet Rev2 Trap

```bash
# Configure (once per clean build)
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 \
  -DCMAKE_C_COMPILER=gcc-12

# Build only this target (-j4 cap avoids OOM on 7 GB CI runners)
cmake --build build --target ProphetRev2Trap_VST3 -j4

# Artifact
build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3/Prophet\ Rev2\ Trap.vst3
```

### SP-1200 / full monorepo

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone SP1200_VST3
ctest --test-dir build -R SP1200 --output-on-failure
```

### Quick compile + error log (any plugin)

```bash
./scripts/vst/autopilot-compile.sh
# errors written to vst-testing-ops/error_log.txt
```

---

## 5. Pluginval / QA

```bash
# Install binary (cached)
python3 scripts/vst/run_pluginval.py --install-only

# Validate a specific build output
xvfb-run -a python3 scripts/vst/run_pluginval.py \
  --discover build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3 \
  --strictness-level 5

# Full CI-style QA (matches build.yml)
python3 vst-testing-ops/run_business.py --profile ci
```

---

## 6. VST/VSTi lifecycle (APC phases)

New plugins follow the phase-locked order. Claude Code uses the same phases as Cursor:

```
/apc-dream → /apc-plan → /apc-design → /apc-impl → /apc-test → /apc-ship
```

- Workspace artifacts live under `plugins/<Name>/`
- Code lives under `vst/<Name>/Source/`
- **No DSP code before Dream + Plan + Design artifacts exist** (unless user says `skip-contract`)
- Check `plugins/<Name>/status.json` before Implement phase

See `docs/vst-lifecycle.md` for full details.

---

## 7. SmoothParam pattern (JUCE)

```cpp
// In engine header — declare per parameter
SmoothParam p_cutoff;

// In prepareToPlay / setCurrentPlaybackSampleRate
p_cutoff.reset(sampleRate, 20.0); // 20 ms ramp

// In processBlock — call ONCE per block (not per sample)
float cutoff = p_cutoff.process(apvts.getRawParameterValue("cutoff_hz")->load());
```

---

## 8. JUCE manufacturer codes

Every new plugin CMakeLists must have:

```cmake
PLUGIN_MANUFACTURER_CODE "CDSP"   # CustomDSP
PLUGIN_CODE "XXXX"                # unique 4-char code per plugin
```

Without these, DAWs (Reaper, FL Studio) reject the VST3 on scan.

---

## 9. What Claude Code does NOT auto-load

| Cursor feature | Claude Code equivalent |
|----------------|------------------------|
| `.cursor/rules/*.mdc` | This file — paste the session bootstrap above |
| `@Docs juce.com` | Link https://docs.juce.com or reference `build/_deps/juce-src` |
| `.cursorignore` | No effect on Claude; `build/` is excluded by standard `.gitignore` |
| Cursor Agent Mode | Claude Code agentic session with file tools |
