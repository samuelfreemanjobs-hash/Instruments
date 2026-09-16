# A/B offline render (manual harness)

Automated spectral diff is not wired yet. Use this workflow to compare JD Upgraded against reference audio:

## Render from the standalone

1. Build: `cmake --build build -j`
2. Run `build/JDUpgraded_artefacts/Release/Standalone/JD Upgraded`
3. Load factory program, set envelopes, record MIDI performance in a DAW or use the plugin in a host and export WAV.

## Compare

```bash
# Level-normalized difference (requires sox)
sox ref.wav plugin.wav -n trim 0 10 stat 2>&1 | head
```

For repeatable notes, send a short MIDI clip (e.g. C4, 2 s hold) through both the reference JD plugin and JD Upgraded with the same program index.

## Planned automation

- Headless `tools/OfflineRender` target (fixed MIDI, fixed program index, WAV out)
- FFT magnitude diff vs user-supplied reference clip
