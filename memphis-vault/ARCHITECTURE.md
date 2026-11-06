# Memphis Vault — batch generator

## Purpose

**Memphis Vault** is an offline **batch one-shot generator** for phonk/trap-style drum layers. It writes folders of **procedural** mono WAV files (no input samples) plus a small JSON manifest for packaging pipelines (“Generator Agent” in the Memphis Vault product story).

## Build & run

Requires **Python 3.9+** (stdlib only).

```bash
python3 memphis-vault/scripts/batch_generate.py --help
python3 memphis-vault/scripts/batch_generate.py --out ./out --count 8 --prefix memphis
```

### Options

| Flag | Meaning |
|------|--------|
| `--out` | Root directory; each batch is `out/<batch_id>/` |
| `--count` | Number of batches |
| `--prefix` | Batch id prefix (`{prefix}_0001`, …) |
| `--layers` | Comma list: `kick`, `snare`, `hat_closed`, `hat_open`, `rim`, `clap` |
| `--seed` | Optional global RNG seed (default: **deterministic per batch id**) |

## Data flow

```text
batch_generate.py → out/<batch_id>/wav/*.wav + manifest.json
```

`manifest.json` uses `format: MEMPHIS_VAULT_BATCH_MANIFEST` with SHA256 per file for integrity and storefront ingestion.

## Algorithm notes (v0)

- **Kick**: Pitch sweep sine + exponential decay.
- **Snare**: High-passed Gaussian noise + short tonal sine; lengths/decays randomized.
- **Hats**: Filtered noise; open hat uses longer decay.
- **Rim**: Ringing band + noise click.
- **Clap**: Sparse noise bursts mixed along a decay envelope.

All parameters are drawn from `random.Random` seeded per batch (or `--seed`).

## Extension points

- **Processor Agent**: post-process each WAV (tape hiss, bit crush, transient shaper) in a second script or JUCE offline tool; keep manifests chained by `batchId`.
- **Packager Agent**: zip each `batch_id` folder and attach genre tags from an external CSV.
- **Legal**: Document that output is synthesized; still avoid marketing claims that substitute for counsel.

## Related docs

- Monorepo index: `/ARCHITECTURE.md`
- Disklordz kit manifest cousin: `disklordz/sound-factory/ARCHITECTURE.md`
