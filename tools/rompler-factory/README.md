# Rompler factory (ASR-class content)

Offline generators for **Ensoniq-class rompler** content. Drum SaaS remains in [disklordz/sound-factory](../../disklordz/sound-factory/).

## Scripts

| Script | Purpose |
|--------|---------|
| `generate_asr_waves.py` | ASR-style short chromatic waves → WAV + `zones.json` |
| `fetch_third_party_libraries.sh` | Download VCSL / VSCO2 / Salamander **locally** (large; not for CI) |

Content root: [content/asr-class-v0/](../../content/asr-class-v0/)  
Plan: [docs/ASR_CLASS_CONTENT_V0.md](../../docs/ASR_CLASS_CONTENT_V0.md)

## Quick start (factory waves)

```bash
python3 tools/rompler-factory/generate_asr_waves.py --preset dark --seed-base 1000
python3 tools/rompler-factory/generate_asr_waves.py --preset bright --seed-base 2000
```

Import output under `content/asr-class-v0/factory/waves/<preset>/` into HISE using `zones.json`.

## Third-party libraries

```bash
# Optional: only when you have disk space (~4–8 GB total depending on picks)
DOWNLOAD_THIRD_PARTY=1 ./tools/rompler-factory/fetch_third_party_libraries.sh
```

See script header for manual URLs if automated fetch fails.
