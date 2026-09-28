# Disklordz sound-factory (v0)

Offline generation scripts for the drum SaaS. The web app (`disklordz/website/`) calls into this tree as generation matures.

## v0

- `scripts/generate_stub_kits.py` — procedural one-shots per style preset into `website/public/samples/<preset>/`.

```bash
python3 scripts/generate_stub_kits.py
```

**WO-SAAS-004 (live):** Parametric generation runs in **`disklordz/website/src/lib/generation/`** (prompt + preset → unique WAV per kit). Python CLI `scripts/generate_kit.py` for offline/batch.

**Next:** Archive-driven samples + `sound-factory` batch jobs feeding the same manifest schema.
