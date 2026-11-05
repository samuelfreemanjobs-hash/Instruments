# Boom — style → 100 VST3 one-shots

**Start here (builds renderer if needed):**

```bash
./scripts/boom.sh \
  --style "YOUR STYLE" \
  --vst3 "/path/to/Plugin.vst3" \
  --out ~/Exports/my_pack
```

Re-render a frozen job (no rematch):

```bash
python3 tools/vsti-one-shot/boom.py --job ~/Exports/my_pack/.boom_*/job.json
```

| Flag | Purpose |
|------|---------|
| `--match-mode hybrid` | Default — preview + name matching |
| `--match-mode names` | Fast factory pick |
| `--count 100` | Slot count (default 100) |
| `--dry-run` | Recipe + assignments only |

Handoff: [docs/VSTI_ONE_SHOT_HANDOFF.md](../../docs/VSTI_ONE_SHOT_HANDOFF.md) · [ARCHITECTURE.md](ARCHITECTURE.md) · [docs/SAMPLE_RECIPE_BOOK.md](docs/SAMPLE_RECIPE_BOOK.md)
