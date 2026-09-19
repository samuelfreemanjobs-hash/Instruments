# DISKLORDZ sample standard v1

Apply **before** assets enter the sample database.

## Recording

- 48 kHz (project standard; document exceptions)
- 24-bit WAV
- Stereo where appropriate; mono for mono sources
- Consistent gain staging; no clipping; no unnecessary brick-wall normalization on capture

## Editing

- Trim silence; remove unwanted noise
- Pitch / root detection (factory pipeline)
- Phase check (stereo); DC offset removal
- Peak + loudness analysis
- Loop detect → optimize → crossfade; fade in/out
- Write **SampleAsset** metadata (not sidecar-only)

## Naming convention

```text
DL_[CATEGORY]_[INSTRUMENT]_[NOTE]_[VELOCITY]_[RR].wav
```

Examples:

- `DL_PIANO_GRAND_C3_VEL03_RR01.wav`
- `DL_808_CLASSIC_C1_VEL05_RR01.wav`

Agents and import tools parse tokens; **database id** remains `DL_SAMPLE_*`.

## Sampling strategies

See [MULTISAMPLE_FORMAT.md](MULTISAMPLE_FORMAT.md) — deep / medium / lightweight / one-shot.

## Legal

Only original recordings, licensed redistribution, or in-repo synthesis. No third-party workstation ROM samples.
