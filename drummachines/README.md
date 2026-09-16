# Vintage drum machine samples (Sound Factory library)

On-disk source library for DiskLordz **Sound Factory**, from the Internet Archive [Ultimate Vintage Drum Machines Sample Pack](https://archive.org/details/ultimate-vintage-drum-machines-sample-pack) (~5.5 GB `.7z` → many machine folders of WAVs).

Provenance: [`SOURCE.md`](SOURCE.md). Factory config: [`../disklordz/sound-factory/config/drum_machine_library.yaml`](../disklordz/sound-factory/config/drum_machine_library.yaml).

## Layout

| Path | Purpose |
|------|---------|
| `scripts/fetch_and_extract.sh` | Download + extract into `samples/` |
| `scripts/verify_sample_library.py` | List top-level machine folders + WAV counts |
| `scripts/commit_samples_lfs.sh` | `git add` samples via **Git LFS** |
| `samples/` | **Committed** via LFS (not the `.7z` cache) |
| `_cache/` | Downloaded archive only (gitignored) |

## Acquire end-to-end

```bash
cd drummachines
./scripts/fetch_and_extract.sh
python3 scripts/verify_sample_library.py
cd ..
./drummachines/scripts/commit_samples_lfs.sh
git commit -m "Add vintage drum machine sample library (LFS)"
git push
```

Requires **Archive.org egress** on Cloud Agents (allowlist + **new agent run** after approval). The `.7z` stays in `_cache/`; only `samples/` is tracked.

## Git LFS

Root [`.gitattributes`](../.gitattributes) routes `drummachines/samples/**` audio through LFS. Ensure your GitHub account has enough **LFS storage/bandwidth** (extracted size is often **8–10+ GB**).

## Cloud Agent blocker (if download fails)

If `curl` to `archive.org` fails with SSL errors, this pod does not have Archive.org on its egress allowlist yet. Approve `archive.org` / `ia800808.us.archive.org` in environment network settings, then **start a new Cloud Agent run** (running pods do not hot-reload egress).
