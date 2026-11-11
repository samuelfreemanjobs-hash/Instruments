# Disklordz sound-factory (v0)

Offline generation scripts for the drum SaaS. The web app (`disklordz/website/`) calls into this tree as generation matures.

## v0

- `scripts/generate_stub_kits.py` — procedural one-shots per style preset into `website/public/samples/<preset>/`.

```bash
python3 scripts/generate_stub_kits.py
```

**WO-SAAS-004 (live):** Parametric generation runs in **`disklordz/website/src/lib/generation/`** (prompt + preset → unique WAV per kit). Python CLI `scripts/generate_kit.py` for offline/batch.

**Phonk Drum Studio → factory kits**

```bash
python3 scripts/build_studio_kit.py \
  --preset midnight-circuit \
  --prompt "memphis phonk vae kit" \
  --samples-dir /path/to/wavs \
  --out /path/to/kit-out \
  --fill-stubs
```

Or via `generate_kit.py --studio-samples-dir …`. WAVs are matched to slots (`kick.wav`, aliases, or `studio_kit_map.json`). Manifest uses per-sample `factory_studio_v1` provenance where sourced from your folder.

**Next:** Archive-driven samples + batch jobs feeding the same manifest schema.
