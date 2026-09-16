---
name: juce-plugin-scaffold
description: Creates new JUCE VST projects from vst/template via new-vst.sh, CMake codes, and folder layout. Use when starting any new VST/VSTi in this repo.
---

# JUCE Plugin Scaffold subagent

## Read first

- `.cursor/rules/06-vst-vsti-template.mdc`
- `vst/README.md`
- `vst/template/RENAME_CHECKLIST.md`

## Steps

1. Confirm plugin name PascalCase: `MySynth`, `Phonk808`.
2. Run from repo root:
   ```bash
   ./tools/new-vst.sh <PluginName>
   ```
3. Verify unique **4-char** codes in `CMakeLists.txt`:
   - `PLUGIN_MANUFACTURER_CODE`
   - `PLUGIN_CODE`
4. Ensure JUCE submodule:
   ```bash
   git submodule update --init vst/JUCE
   ```
5. Add stub `params.json` if tier ≥ L1 (global state skill).

## Synth vs effect

- Current template is **synth** (`IS_SYNTH TRUE`).
- For FX-only: set `IS_SYNTH FALSE`, `NEEDS_MIDI_INPUT FALSE`, adjust bus layout in Processor.

## Deliverables

- `vst/<Plugin>/` with CMake + Source
- `README.md` with build commands
- No edits inside `vst/template/` for shipping work

## Checklist

- [ ] Plugin name matches folder name
- [ ] `createPluginFilter` returns correct Processor class
- [ ] Rule 06 satisfied — project under `vst/`, not `src/oscillators/`
