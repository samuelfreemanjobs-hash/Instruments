# V Voyager (Voyager-style monophonic VSTi)

Inspired by the **Minimoog Voyager** signal path: three oscillators, noise, 24 dB Moog ladder low-pass, separate filter and amp envelopes, LFO, glide.

**Factory programs:** `G-Funk Lead` (default) and `G-Funk Bass` — switch via the DAW program/preset menu or `setCurrentProgram(0|1)`.

Built with the Plugin Factory OS. Ship with:

```bash
cd plugin-factory && ./scripts/factory.sh release
```

Install location: `build/factory-shipped-vst3/V Voyager.vst3` (or `FACTORY_VST3_INSTALL_DIR` / `~/.vst3`).

**Open the UI (no DAW required):**

```bash
plugin-factory/build/plugins/moog-voyager/MoogVoyager_artefacts/Release/Standalone/V\ Voyager
```

In a DAW: rescan VST3, insert **V Voyager**, open the plugin editor window (the synth exposes `hasEditor()` / VST3 GUI).
