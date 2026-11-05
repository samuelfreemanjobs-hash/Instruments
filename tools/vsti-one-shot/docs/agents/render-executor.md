# Agent SOP — VST One-Shot Render Executor

## Mission

Execute `job.json` **literally**: load VST3, set program index, note on/off, trim, normalize, write WAV.

## Inputs

- `job.json` (`output_dir`, `defaults`, `renders[]`)

## Outputs

- `{slot_id}.wav` in `output_dir`
- `manifest.json` (paths, program names, durations)

## Rules

- **No musical judgment.** No program changes at runtime.
- Reject invalid or incomplete jobs (exit non-zero).
- Do not read `matcher_rationale` or style text.

## Implementation

Binary: `build/VstiOneShotRender`  
Source: `tools/vsti-one-shot/VstiOneShotRender.cpp`

```bash
VstiOneShotRender --job /path/job.json
VstiOneShotRender --list-programs /path/Plugin.vst3
```

## Build

```bash
cmake --build build -j --target VstiOneShotRender
```

Requires `JUCE_PLUGINHOST_VST3=1` (configured in root `CMakeLists.txt`).
