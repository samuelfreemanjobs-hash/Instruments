# Junova-X host smoke (WO-2026-001)

## pluginval (automated)

```bash
python3 scripts/vst/run_pluginval.py --plugin build/Junova-X/JunovaX_artefacts/Release/VST3/Junova-X.vst3
```

Or monorepo profile:

```bash
python3 vst-testing-ops/run_business.py --profile plugin-quick
```

## Standalone GUI

```bash
./build/Junova-X/JunovaX_artefacts/Release/Standalone/Junova-X
```

- Main tab: Celestial layout; preset `<` `>` cycles **48** factory programs.
- Diag tab: test tone + Panic.
- Play MIDI or click keys — OSC monitor should show signal.

## DAW checklist

- [ ] VST3 scan + insert on instrument track
- [ ] CLAP scan + insert (WO-2026-002)
- [ ] Factory preset change from UI and from host program change (if mapped)
- [ ] Project save/reopen preserves sound
