# Night Circuit — bank integrity (2026-09-24)

## Environment
- OS: Linux-6.12.94+-x86_64-with-glibc2.39
- Compiler: g++-12 (Ubuntu 12.4.0-2ubuntu1~24.04.1) 12.4.0
- Git: `9228c08`

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
(no output)
exit code: 0
```

## Release recommendation
**PASS** (bank integrity only)

## pluginval (VST3 smoke)
- `scripts/vst/run_pluginval.py` on `Night Circuit.vst3` — **PASS**

### NOT TESTED in this pass
- Render-all-1028 automated audio
- Listening review by category
- Host project recall / DAW matrix
