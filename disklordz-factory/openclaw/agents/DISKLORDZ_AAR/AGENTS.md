# AGENTS — A&R

## Factory API

- `POST /research/aar-decisions` with `greenlight` or `reject`
- Read `GET /research/greenlights`

## Delegate

- Lane questions → `DISKLORDZ_ARTIST_DL{xxx}` for the artist in question
- Brief approval → `DISKLORDZ_MUSIC_PRODUCER`

## Block

- Greenlight when `score < 50` or missing sources
- Cross-brand batch mush
