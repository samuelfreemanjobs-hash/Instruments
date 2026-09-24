# Night Circuit — bank integrity (2026-09-24)

## Environment
- OS: Linux-6.12.94+-x86_64-with-glibc2.39
- Compiler: g++-12 (Ubuntu 12.4.0-2ubuntu1~24.04.1) 12.4.0
- Git: `8b8f1bd`

## Acceptance criteria (automated)
- Total factory presets: **1028** — **PASS**
- Bass: 195 — **PASS** (validated via NightCircuitTests)
- Synth: 245 — **PASS** (validated via NightCircuitTests)
- Lead: 196 — **PASS** (validated via NightCircuitTests)
- Pad: 196 — **PASS** (validated via NightCircuitTests)
- Pluck/Keys: 196 — **PASS** (validated via NightCircuitTests)
- Unique (category, name): **PASS**
- Parameter finite + in-range: **PASS**

## NightCircuitTests
```
Includes: analog helpers, 1028 bank quotas/uniqueness/ranges, MIDI note-on peak smoke, APVTS state round-trip.
exit code: 0
```

## Build baseline
- JUCE FetchContent tag: **8.0.15** (resolved commit `91ad83ae34a8`)
- Editor minimum width: **960** px
- Standalone: launched under xvfb (3s smoke, no audio assertion)

## Release recommendation
**PASS** (bank integrity only)

### NOT TESTED in this pass
- Render-all-1028 automated audio
- Listening review by category
- Host project recall / DAW matrix
- pluginval (run separately per NIGHT_CIRCUIT_QA_SUBAGENT.md)
