# Vintage drum machine samples (local asset library)

This folder holds **extracted** samples from the Internet Archive item [Ultimate Vintage Drum Machines Sample Pack](https://archive.org/details/ultimate-vintage-drum-machines-sample-pack) (uploaded by OniBaku98). The upstream archive is about **5.5 GB** as a single `.7z` file.

## Layout

| Path | Purpose |
|------|---------|
| `scripts/fetch_and_extract.sh` | Download + extract into `samples/` |
| `samples/` | Extracted machine folders (not committed to git) |
| `_cache/` | Downloaded `.7z` (not committed) |

## Quick start (your machine or agent with Archive.org egress)

```bash
cd drummachines
./scripts/fetch_and_extract.sh
```

Requires: `curl`, `python3`, and `py7zr` (`pip install py7zr`).

Direct download URL (same file as the link you shared):

`https://archive.org/download/ultimate-vintage-drum-machines-sample-pack/Ultimate%20Vintage%20Drum%20Machines%20Sample%20Pack%20by%20Onibaku.7z`

## Git policy

**Do not commit** the `.7z` or extracted WAV/AIFF under `samples/`. They are build inputs for DiskLordz Sound Factory, not source code. Use this directory on disk (or object storage) and record provenance in Airtable **Sound Assets** / `disklordz/sound-dna` when you ingest.

## Cloud Agent note

Cursor Cloud Agents use an egress allowlist. If download fails with SSL or timeout errors, approve **archive.org** / **ia800808.us.archive.org** in your environment setup, then re-run the script.
