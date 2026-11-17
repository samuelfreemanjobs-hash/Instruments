# Storefront pack (local)

Tree: `CNV_Converter_Night/` — **512** WAVs (drums @ 3× guide + full FX set) + `_DOCS/`.

WAVs are **gitignored**. Rebuild:

```bash
cd claude/drumsmith
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build native/build -j --target drumsmith-render
rm -rf /tmp/CNV_W2_512
python3 scripts/native_factory.py \
  --out /tmp/CNV_W2_512 \
  --renderer native/build/drumsmith-render \
  --genre memphis-tape --sku CNV --plan converter-night
```

Then re-pack into this tree (see kit-spec / W2_RENDER). `release_ready` stays false until HITL.
