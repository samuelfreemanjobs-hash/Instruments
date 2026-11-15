# Overnight Drum Sampler (JUCE)

Loads `../output/best_kit/*.wav` at runtime. Rebuild only when plugin code changes — not every overnight iteration.

`COPY_PLUGIN_AFTER_BUILD` is forced **OFF**. Copy the `.vst3` manually from `build/...` if you want it in a DAW sandbox folder (not Program Files).
