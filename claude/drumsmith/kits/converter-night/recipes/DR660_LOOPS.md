# DR-660–style loop technique (original blueprints)

**Not** a Boss ROM clone. Patterns under `loops/cnv-dr660-*.json` encode production language from public Memphis-production research + RATE bank notes.

## Technique checklist (from research `[inspected]`)

1. **Pitched 808 beds** — long-decay subs as bassline (re-triggers = choke/restart).
2. **Pitched cowbell / clave-bell** — melodic accents without piano/synth beds.
3. **Sparse kick + half-time snare** — slow, bass-heavy, not boom-bap.
4. **Thin hats** — 8th or quarter pulse; avoid modern trap rolls.
5. **Drums-first** — groove stands alone; texture FX optional elsewhere.

## Blueprints

| File | BPM | Idea |
|------|----:|------|
| `cnv-dr660-94-a.json` | 94 | Tape crawl + cowbell motif |
| `cnv-dr660-94-clave.json` | 94 | Clave-bell / cowbell lead |
| `cnv-dr660-94-thick.json` | 94 | Heavy 808 bed |
| `cnv-dr660-134-ht.json` | 134 | Half-time converter |
| `cnv-dr660-134-bell.json` | 134 | Cowbell lead, thin hats |
| `cnv-dr660-134-crawl.json` | 134 | 808 choke/restart crawl |

Render:

```bash
python3 scripts/render_loop_from_blueprint.py \
  --blueprint kits/converter-night/loops/cnv-dr660-94-a.json \
  --kit kits/converter-night --out /tmp/out.wav --variant 0
```

## Honest gap

True **DR-660 WAV** still missing — see `references/dr660/STAGED.md`. Loops use synthesized CNV masters guided by TR-808 lofi + technique text.
