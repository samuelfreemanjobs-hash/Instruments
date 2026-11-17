# Disklordz Drum SaaS (website)

## Purpose

**DiskLordz Drum Kit Factory** web product: DNA formula → 16-pad audition → WAV / product-pack export.
Also ships the prompt studio path (vibe → variations → ZIP). Growth SKU per
[docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md). Mirrors the JUCE Standalone kit-factory UX
in `vst/DrumDesignLab/`.

## Build & run

```bash
cd disklordz/website
npm install
npm run dev    # http://localhost:3100 preferred on shared Windows workers
npm run build && npm start
```

Regenerate stub WAVs:

```bash
python3 ../sound-factory/scripts/generate_stub_kits.py
```

## Routes

| Path | Role |
|------|------|
| `/` | Brand landing + **Kit Factory** (32 DNA formulas, 16 pads, export) |
| `/studio` | Prompt studio (`KitGenerator`) — presets, variations, product pack |
| `/daw-inbox` | Folder export + desktop watcher docs |
| `/login`, `/account` | Auth + credits / kit history |

## Data flow

```text
Browser KitFactoryLab (/)
  → DNA formula catalog (`src/lib/dna-formulas.ts`, 32 banks)
  → Tone.js pad audition (`src/lib/preview/pad-preview.ts`) before WAV build
  → POST /api/generate | /api/factory/batch (same backend as studio)
  → POST /api/download | /api/factory/download → ZIP

Browser KitGenerator (/studio)
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
| `src/components/KitFactoryLab.tsx` | DNA formula → 16 pads → kit/pack export |
| `src/components/SiteChrome.tsx` | Global nav (Factory / Prompt studio / DAW) |
| `src/lib/dna-formulas.ts` | 32 DNA formulas (Bang / Hitmaker / MelodyMotion / SouthernTriumph) |
| `src/lib/preview/pad-preview.ts` | Tone.js pad audition by family |
| `src/components/KitGenerator.tsx` | Prompt UI, presets, preview, download |
| `src/lib/presets.ts` | Five presets (4 artist lanes + MPC neutral) + SparkLE lanes |
| `src/lib/generation/generation-spec.ts` | WO-SAAS-007 spec parse + preset defaults |
| `src/lib/generation/engine-render.ts` | WO-SAAS-013 studio vs creative render + provenance |
| `src/lib/generation/loop-render.ts`, `sfx-render.ts`, `mode-utils.ts` | WO-SAAS-014 loop/SFX + credit tiers |
| `src/lib/generation/product-factory.ts` | WO-SAAS-015 storefront folder pack |
| `src/lib/generation/factory.ts` | Prompt-driven kit build + kit store |
| `src/components/GenerationSpecFields.tsx` | Spec UI (mode, engine, key, BPM, …) |
| `src/lib/rag/` | WO-SAAS-012 hybrid RAG + `POST /api/rag/suggest` |
| `src/inngest/` | Async generate + RAG reindex events |
| `src/lib/generation/engines/` | Parametric vs remote engine registry |
| `src/app/api/integrations/status` | Open-source integration health |
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

## DAW inbox (WO-SAAS-016)

- Browser: **Save WAVs to folder** on home (File System Access API).
- Desktop: [disklordz/daw-inbox/](../daw-inbox/README.md) watches Downloads for `disklordz-*.zip`.
- User doc route: `/daw-inbox`.

## Go live

- [docs/DISKLORDZ_GO_LIVE.md](../../docs/DISKLORDZ_GO_LIVE.md) — Vercel, Supabase migrations, Stripe, `npm run verify:go-live`.

## Related docs

- [DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md)
- Artist lane presets map to factory `DL001` / `DL002` / `DL004` / `DL006`
