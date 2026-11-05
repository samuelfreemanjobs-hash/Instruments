# Boom — style → 100 VST3 one-shots

**Start here:**

```bash
python3 tools/vsti-one-shot/boom.py \
  --style "YOUR STYLE" \
  --vst3 "/path/to/Plugin.vst3" \
  --out ~/Exports/my_pack
```

Build the renderer first:

```bash
cmake --build build -j --target VstiOneShotRender
```

See [ARCHITECTURE.md](ARCHITECTURE.md) and [docs/SAMPLE_RECIPE_BOOK.md](docs/SAMPLE_RECIPE_BOOK.md).
