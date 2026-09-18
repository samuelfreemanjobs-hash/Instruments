# Disklordz sound-factory (v0)

Offline generation scripts for the drum SaaS. The web app (`disklordz/website/`) calls into this tree as generation matures.

## v0

- `scripts/generate_stub_kits.py` — procedural one-shots per style preset into `website/public/samples/<preset>/`.

```bash
python3 scripts/generate_stub_kits.py
```

**WO-SAAS-004 (live):** Parametric generation runs in **`disklordz/website/src/lib/generation/`** (prompt + preset → unique WAV per kit). Python CLI `scripts/generate_kit.py` for offline/batch.

**Automation truth:** Factory is **not** fully unattended. Batch helper:

```bash
bash disklordz/sound-factory/scripts/batch_tripwire_sample.sh
```

Then **CROW / founder QC** → zip into `disklordz/launch-assets/tripwire-sample-v1/`. See [docs/DISKLORDZ_INTEGRATION_AUDIT.md](../../docs/DISKLORDZ_INTEGRATION_AUDIT.md).

**Next:** Archive-driven samples + async job queue (ILLUGEN 009) feeding the same manifest schema.
