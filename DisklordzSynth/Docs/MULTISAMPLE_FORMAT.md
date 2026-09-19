# Multisample format

A **multisample** is a set of **SampleAsset** zones (key + velocity + optional RR) that play as one instrument source.

## ID

`DL_MULTI_000001` — stable; filenames may change.

## Zone

| Field | Description |
|-------|-------------|
| sampleId | `DL_SAMPLE_*` |
| rootMidiNote | Zone root |
| keyLow / keyHigh | Mapping range (optional; engine may use root-only P0) |
| velocityLow / velocityHigh | Layer range |
| crossfadeFrames | Optional |

## Builder manifest (P0)

Line-oriented `.manifest` in instrument folder — see [FactoryContent/README.md](../../DisklordzRompler/FactoryContent/README.md).

Future: JSON export from sample DB with full zone matrices.

## Sampling philosophy → zone count

| Strategy | Name in spec | Typical zones |
|----------|--------------|---------------|
| A Deep | `SamplingStrategy::deep` | 8 vel × 5–7 keys × RR2 |
| B Medium | `medium` | 4 vel × 4–6 keys |
| C Lightweight | `lightweight` | 1–3 samples |
| D One-shot | `oneShot` | 1–2 samples |

Set on multisample metadata; ROM Factory validates against [ROM_BUDGET_V1.md](ROM_BUDGET_V1.md).

## Output

Compiles to DLRROM01 waves sharing one `multisampleSetId` on a given `pcm_bank_XX.dlrrom`.
