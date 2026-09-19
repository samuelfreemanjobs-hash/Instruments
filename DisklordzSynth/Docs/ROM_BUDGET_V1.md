# Factory ROM budget v1 (~7 GB target)

Set **maximum** factory size before bulk recording. Adjust per release.

| Category | Approx. budget |
|----------|----------------|
| Piano | 1.5 GB |
| EP / Keys | 500 MB |
| Bass | 750 MB |
| 808 | 300 MB |
| Bells / Mallets | 500 MB |
| Guitar | 750 MB |
| Strings | 750 MB |
| Brass / Winds | 500 MB |
| Synth | 500 MB |
| Vocals | 250 MB |
| Textures | 300 MB |
| Atmospheres | 300 MB |
| FX | 200 MB |
| **Total** | **~7.1 GB** |

P0 procedural placeholder ROM is far smaller; this budget applies when real WAVs land.

**Streaming required** above ~1–2 GB installed — see [STREAMING_ARCHITECTURE.md](../../DisklordzRompler/Docs/STREAMING_ARCHITECTURE.md).

Tools should report `sum(wav bytes)` per category during import and fail CI if over cap.
