# Disklordz Drum SaaS (website)

## Purpose

Prompt-driven **drum sample kit** web app: vibe → preview one-shots → ZIP download with manifest. Growth SKU per [docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md).

**Memphis VST launch funnel** (tripwire + freebies): [docs/DISKLORDZ_MASTER_LAUNCH_CANVAS.md](../../docs/DISKLORDZ_MASTER_LAUNCH_CANVAS.md) — routes under `/launch`, Web Audio Cassette Smash preview, opt-in API.

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
       → buildFactoryKit(): one_shot (6 hits), loop (`loop_main.wav`), or sfx (`sfx.wav`)
       → engine: studio vs creative render paths (WO-SAAS-013)
       → credit wallet: `creditCostForSpec` per batch (WO-SAAS-014)
       → WAVs → local temp + Supabase Storage `disklordz-kits` when service role set
       → manifest with /api/samples/<kitId>/<path>.wav URLs
  → POST /api/factory/batch → `DISKLORDZ_PRODUCT_PACK_MANIFEST` (01_KICKS … 04_PERC)
  → POST /api/factory/download { productPack } → storefront ZIP
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
| `src/app/launch/` | Kern funnel pages + `CassetteSmashSoundboard` + `MemphisVaultKitPromo` |
| `src/lib/audio/cassette-smash.ts` | Browser Drive/Wobble chain (plugin preview) |
| `src/app/api/launch/opt-in/route.ts` | Vocal vault email capture (+ optional Resend) |
| `src/lib/presets.ts` | Five presets (4 artist lanes + MPC neutral) |
| `src/lib/generation/generation-spec.ts` | WO-SAAS-007 spec parse + preset defaults |
| `src/lib/generation/engine-render.ts` | WO-SAAS-013 studio vs creative render + provenance |
| `src/lib/generation/loop-render.ts`, `sfx-render.ts`, `mode-utils.ts` | WO-SAAS-014 loop/SFX + credit tiers |
| `src/lib/generation/product-factory.ts` | WO-SAAS-015 storefront folder pack |
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
