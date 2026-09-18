# WAVE-909 Validation Report (v0.1.0)

## Build gate

| Check | Result |
|-------|--------|
| Release configure + build (Linux g++-12) | VERIFIED locally |
| VST3 bundle present | VERIFIED |
| CLAP binary present | VERIFIED (via clap-juce-extensions) |
| Standalone executable | VERIFIED |

## Automated QA

| Test | Result |
|------|--------|
| `Wave909Tests` (wavetable, voice render, filter impulse) | VERIFIED |
| `Wave909StateTests` (APVTS + program restore) | VERIFIED |
| `Wave909OfflineRender` determinism (A/B SpectralDiff) | VERIFIED |
| Golden WAV regression (`tests/golden/verify_golden.sh`) | VERIFIED after golden commit |

## Plugin validator / DAW matrix

| Host / tool | Result |
|-------------|--------|
| pluginval | NOT VERIFIED in CI (tool not bundled) |
| REAPER / Ableton / FL | NOT VERIFIED |

## Performance

| Metric | Result |
|--------|--------|
| CPU @ 16 voices | NOT VERIFIED (no profiler run in cloud) |

## Overall

**Release candidate 0.1.0** for developer/early tester use. Commercial “RELEASE READY” still requires host validation and CPU profiling.
