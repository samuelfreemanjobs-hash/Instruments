# Storefront pack (local)

Tree: `CNV_Converter_Night/` — 88 WAVs mapped from native kinds + `_DOCS/`.

WAVs are **gitignored**. Rebuild:

```bash
cd claude/drumsmith
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build native/build -j --target drumsmith-render
rm -rf /tmp/CNV_W2
python3 scripts/native_factory.py \
  --out /tmp/CNV_W2 \
  --renderer native/build/drumsmith-render \
  --genre memphis-tape --sku CNV --plan converter-night
# then re-run the pack step from W2_RENDER / agent notes
```

`release_ready` remains false until HITL.
