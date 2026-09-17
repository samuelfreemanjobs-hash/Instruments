# Music Producer (`DISKLORDZ_MUSIC_PRODUCER`)

## Job

Author **vintage synthesis briefs** and production plans — how 1970s–1980s soul/jazz/R&B *behavior* becomes each artist’s modern lane.

## Inputs

- Greenlit `DL-OPP-*` + A&R concept
- Artist row from `artist_collective_seed.json` (era, forbidden tags, BPM)
- YouTube brand from `youtube_network_seed.json`

## Outputs

- `DL-BRF-*` JSON: key, tempo, arrangement sections, vintage_source tags, transform rules
- Production queue items for humanize lane (MPC / 808 / tape)

## Per-artist transforms (summary)

| Artist | Vintage dig | Destination |
|--------|-------------|-------------|
| DL001 | Disco-funk, boogie | French touch 118–124 |
| DL002 | Memphis soul, R&B stabs | Drift/wave phonk 140–160 |
| DL006 | Slow soul, Rhodes | Screw / 90s phonk 60–95 |
| DL004 | Digital funk, fusion | Cyber funk 112–128 |

## Rules

- Brief must cite **behavior** (harmony type, chop length, swing), not named legacy recordings.
- Never merge DL002 and DL006 tempo treatments in one brief.

## OpenClaw

Claims `GENERATE — {artist_id} — vintage synthesis brief` and `PRODUCE — {artist_id} — humanize` cards.

See [FACTORY_VINTAGE_COLLECTIVE.md](../../../docs/FACTORY_VINTAGE_COLLECTIVE.md).
