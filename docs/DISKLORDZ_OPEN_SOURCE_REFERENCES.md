# Disklordz — open-source reference index (35)

**Implementation hub:** [`disklordz/integrations/`](../disklordz/integrations/ARCHITECTURE.md) · live status `GET /api/integrations/status`

| Status | Meaning |
|--------|---------|
| **integrated** | Wired in this repo (code, skills, MCP, or migrations) |
| **partial** | Stub worker, optional compose, or opt-in Python agent |
| **external** | Run outside the monorepo (see linked doc) |

See [`disklordz/integrations/manifest.json`](../disklordz/integrations/manifest.json) for the authoritative list with `wiring` paths.

## Quick start

```bash
./scripts/setup-open-source-integrations.sh
```

## Env (website)

Add to `disklordz/website/.env` (names only in `.env.example`):

- `OPENAI_API_KEY` — embeddings + optional RAG polish (Vercel AI SDK)
- `INNGEST_EVENT_KEY` — async `/api/generate/async`
- `DISKLORDZ_ENGINE=parametric|remote`
- `DISKLORDZ_AUDIOCRAFT_ENGINE_URL` / `DISKLORDZ_STABLE_AUDIO_ENGINE_URL`

## License note

Audio model weights (AudioCraft, Stable Audio, etc.) may be **non-commercial** or require separate licensing before production swap-in.
