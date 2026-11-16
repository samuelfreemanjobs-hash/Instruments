---
name: vst-ui-prototyper
description: Builds a hardware-style JUCE layout and a 128-frame knob filmstrip from Theme.h. Use for plugin UI, knobs, or resized() scaling.
---

# VST UI prototyper

1. Read colors and the design size from `disklordz/drum-engine/include/Theme.h`.
2. Generate the filmstrip:

```bash
python3 disklordz/drum-engine/python/render_filmstrip.py --out /tmp/knob.png --frames 128 --size 128
```

Pillow writes PNG. Without Pillow the script writes PPM.

3. In `resized()`, divide `getLocalBounds()` into a grid. Call `setSize(designWidth * scale, designHeight * scale)` for 100%, 125%, and 150%. Labels use a font height derived from the cell, not a constant that clips.
4. Wire controls with APVTS attachments. Do not open a WebView unless `juce8-webview.mdc` says the flag is on.
5. Do not add `PluginEditor` layout until the plugin's design spec exists (`10-vst-lifecycle-mandatory`).
