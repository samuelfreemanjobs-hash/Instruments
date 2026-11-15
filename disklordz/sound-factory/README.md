# Disklordz sound-factory (v0)

Offline generation scripts for the drum SaaS. The web app (`disklordz/website/`) calls into this tree as generation matures.

## v0

- `scripts/generate_stub_kits.py` — procedural one-shots per style preset into `website/public/samples/<preset>/`.

```bash
python3 scripts/generate_stub_kits.py
```

**WO-SAAS-004 (live):** Parametric generation runs in **`disklordz/website/src/lib/generation/`** (prompt + preset → unique WAV per kit). Python CLI `scripts/generate_kit.py` for offline/batch.

**Catalog kit:** `python disklordz/sound-factory/scripts/render_memphis_vault.py` writes Memphis Vault Vol. 1 to `kits/memphis-vault-vol-1/` and `kits/Memphis-Vault-Vol-1.zip`.

**Technical QA:** `python disklordz/sound-factory/scripts/kit_qa.py disklordz/sound-factory/kits/memphis-vault-vol-1` writes `qa.json` and exits non-zero on clip, silence, DC, edge clicks, or duplicates. Listen before calling the pack ready to sell.

**Next:** Archive-driven samples + `sound-factory` batch jobs feeding the same manifest schema.
