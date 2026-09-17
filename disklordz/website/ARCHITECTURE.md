# Disklordz Drum SaaS (website)

## Purpose

Prompt-driven **drum sample kit** web app: vibe → preview one-shots → ZIP download with manifest. Growth SKU per [docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md).

## Build & run

```bash
cd disklordz/website
npm install
npm run dev    # http://localhost:3000
npm run build && npm start
```

Regenerate stub WAVs:

```bash
python3 ../sound-factory/scripts/generate_stub_kits.py
```

## Data flow

```text
Browser (KitGenerator)
  → POST /api/generate { prompt, presetId, spec? }
       → rate limit (IP, in-memory v0)
       → buildVariationBatch(): 2 (studio) or 3 (creative) kits per request
       → buildFactoryKit(): prompt-params + synth → /tmp/disklordz-kits/<kitId>/*.wav
       → WAVs → local temp + Supabase Storage `disklordz-kits` when service role set
       → manifest with /api/samples/<kitId>/<file>.wav URLs
  → GET /api/samples/...  (preview audio)
  → POST /api/download { manifest }
       → ZIP(manifest.json, README.txt, WAVs) with SHA-256 provenance check
```

## Threading / realtime

Generation is **sync stub** in v0 (no WebSocket). Future factory jobs will be async (poll) per SaaS research brief.

## Key modules

| Path | Role |
|------|------|
| `src/components/KitGenerator.tsx` | Prompt UI, presets, preview, download |
| `src/lib/presets.ts` | Five presets (4 artist lanes + MPC neutral) |
| `src/lib/generation/generation-spec.ts` | WO-SAAS-007 spec parse + preset defaults |
| `src/lib/generation/factory.ts` | Prompt-driven kit build + kit store |
| `src/components/GenerationSpecFields.tsx` | Spec UI (mode, engine, key, BPM, …) |
| `src/lib/rag/` | WO-SAAS-012 keyword RAG + `POST /api/rag/suggest` |
| `src/app/api/generate/route.ts` | Generate endpoint |
| `src/app/api/download/route.ts` | ZIP export |
| `../sound-factory/` | Offline WAV generation |

## Auth (WO-SAAS-002)

Magic-link email via Supabase Auth (`@supabase/ssr`). Apply migration:

```bash
# Supabase SQL editor or CLI
cat supabase/migrations/20260917190000_saved_kits.sql
```

Signed-in users auto-save each generated kit to `public.saved_kits` (RLS). `/account` lists searchable history (WO-SAAS-011) via `GET /api/kits?...`.

## Extension points

- **WO-SAAS-002:** Done when env + migration applied on your Supabase project.
- **WO-SAAS-004:** Parametric v1 shipped in-process; next: archive/batch pipeline from `sound-factory/`.
- **WO-SAAS-006:** Redis-backed rate limits; error UX polish.

## Related docs

- [DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md)
- Artist lane presets map to factory `DL001` / `DL002` / `DL004` / `DL006`
