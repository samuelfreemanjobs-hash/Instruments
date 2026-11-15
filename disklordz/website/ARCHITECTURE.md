# DiskLordz website

## Purpose

One Next.js app with two jobs.

| Surface | Who it is for | Job |
|---------|----------------|-----|
| **Store** | A visitor shopping for sounds and tools | Stay fast. Sell kits and point at plugins. |
| **Drum Factory** | A signed-in producer | Generate original drums, preview them, export WAV. |

The store is the public face. The factory is the app. A later hostname such as `app.disklordz.com` can front the factory. That hostname is not registered here and is not live.

Brand and screen rules: [docs/WEB_DESIGN_OPS.md](docs/WEB_DESIGN_OPS.md). Do not print a founding year, invent reviews, or show a price that billing has not set.

## Build & run

```bash
cd disklordz/website
npm install
npm run dev    # http://localhost:3000
npm run build && npm start
```

On the Windows laptop, prefer port **3100** and run one heavy job at a time. Do not copy builds into `C:\Program Files`.

```powershell
npx.cmd next dev --turbopack -p 3100
```

Catalog pack (offline, not served by Next yet):

```bash
python disklordz/sound-factory/scripts/render_memphis_vault.py
python disklordz/sound-factory/scripts/kit_qa.py disklordz/sound-factory/kits/memphis-vault-vol-1
```

## What is running

| Capability | Status | Where |
|------------|--------|--------|
| Prompt → preview → ZIP | Live | `/` `KitGenerator`, `POST /api/generate`, `POST /api/download` |
| Product-pack folders | Live API | `POST /api/factory/batch`, `POST /api/factory/download` |
| Magic-link account | Live when Supabase env is set | `/login`, `/account`, `saved_kits` |
| Credit ledger and Stripe Pro | Live when Stripe env is set | `/api/stripe/checkout`, `/api/stripe/webhook` |
| Guest daily limit | Live, in-memory | `SAAS_DAILY_GEN_LIMIT` (default 20) |
| Memphis Vault Vol. 1 | Files on disk, not a store page | `disklordz/sound-factory/kits/memphis-vault-vol-1/` |
| Kit technical QA | Live script | `sound-factory/scripts/kit_qa.py` |
| Store homepage, product page, cart | Not built | — |
| Checkout for a sample pack | Not built | No SKU price is set |
| Chord Engine installer | Not public | In development in `DiskLordz-Audio/` |

Memphis Vault Vol. 1 is 56 original synthesized one-shots, 44.1 kHz, 16-bit, mono, plus `cover.jpg`. `qa.json` can pass while the pack still needs a human listen before it is for sale. It is not a 24-bit archival transfer and it is not sampled from another catalog.

## Surfaces

```text
Store (public)                         Drum Factory (app)
─────────────────────────              ──────────────────────────────
/            marketing home            /drum-factory    KitGenerator
/sounds      catalog                   /account         kits, credits
/sounds/memphis-vault-vol-1            /login
/plugins     honest status             /daw-inbox
/free        generator, within limits
/learn       short guides
/legal/*     draft policies
```

Today `/` is the generator. The store shell replaces that page. The generator moves to `/drum-factory` and keeps its API. Do not offer Add to Cart, Instant Checkout, or a dollar amount until a product row has a server-side price and the pack has been auditioned.

Plugins on the store: Chord Engine is **in development**. Other plugin names are **coming soon**. No download and no payment for those.

## Data flow

Catalog pack, offline:

```text
render_memphis_vault.py
  → kits/memphis-vault-vol-1/  (WAV, cover, manifest, README)
  → kits/Memphis-Vault-Vol-1.zip
kit_qa.py
  → qa.json   clip, silence, DC, edge click, duplicate
human audition
  → only then eligible for a store draft
```

Live generator, unchanged:

```text
Browser (KitGenerator)
  → POST /api/generate { prompt, presetId, spec? }
       → rate limit (IP, in-memory)
       → buildVariationBatch(): 2 (studio) or 3 (creative) kits
       → buildFactoryKit(): one_shot, loop, or sfx
       → credit wallet when Supabase admin is set
       → WAVs in local temp, or Supabase Storage `disklordz-kits`
       → manifest with /api/samples/<kitId>/<path>.wav
  → POST /api/factory/batch → folders 01_KICKS … 04_PERC
  → POST /api/factory/download → ZIP
  → POST /api/download → ZIP with SHA-256 check
```

Payments, when a pack is actually sold:

```text
Store button
  → server creates Stripe Checkout (price id from server config)
  → Stripe webhook, signature verified
  → entitlement row
  → short-lived signed URL for the private ZIP
```

The browser does not confirm payment. Client-side success is not an entitlement.

## Threading / realtime

Generation on the site is synchronous in the request (v0). Inngest routes exist for async generate. Audio rendering for catalog packs runs offline in Python, not on the Next server. The store must not call the renderer on a page view.

No web worker or AudioWorklet ships in the store. Factory preview playback uses the existing sample URLs.

## Key modules

| Path | Role |
|------|------|
| `src/app/page.tsx` | Today: generator. Becomes the store home. |
| `src/components/KitGenerator.tsx` | Factory UI. Moves under `/drum-factory`. |
| `src/lib/generation/` | Parametric drums, packs, WAV encode |
| `src/lib/presets.ts` | Style presets, including Spark lanes |
| `src/app/api/generate/route.ts` | Generate |
| `src/app/api/download/route.ts` | ZIP |
| `src/app/api/stripe/` | Checkout, portal, webhook |
| `src/lib/credits.ts` | Free vs Pro credit ledger |
| `supabase/migrations/` | `saved_kits`, billing, storage, pgvector |
| `public/brand/` | Crest and crowned-floppy marks |
| `../sound-factory/` | Offline packs and `kit_qa.py` |
| `docs/WEB_DESIGN_OPS.md` | Design and ops contract |

## Catalog record

One pack until the next auditioned kit exists.

| Field | Memphis Vault Vol. 1 |
|-------|----------------------|
| SKU | `memphis-vault-vol-1` |
| Title | Memphis Vault Vol. 1 |
| Tagline | Underground phonk samples |
| Origin | Original parametric synthesis |
| Count | 56 |
| Format | 44.1 kHz, 16-bit, mono WAV |
| Cover | `cover.jpg` in the kit folder |
| Price | Unset |
| Store status | Draft. Not published. |
| Related SKUs | None |

Do not list Memphis Vault Vol. 2, Trap Lordz 808 Chassis, or Drift Phonk Essentials.

## Database

Already migrated when Supabase is applied: `saved_kits`, `user_billing` credits, kit storage bucket, prompt-knowledge vectors. See `supabase/migrations/`.

Not migrated. Add only with an explicit schema confirmation:

| Table | Needed for |
|-------|------------|
| `products`, `product_assets` | Store catalog and private ZIP pointer |
| `orders`, `order_items`, `payments` | Stripe Checkout, idempotent webhooks |
| `entitlements`, `download_events` | Who may download which file |
| `pricing_plans`, `subscriptions` | Factory tiers, limits in data, not in the UI |
| `generated_sounds` | Sound DNA for factory saves beyond `saved_kits` |

Row-level security on every user-owned table. Service role stays on the server.

## Security

- Stripe secret, webhook secret, and Supabase service role stay off the client.
- Webhooks are signature-checked and idempotent.
- Download URLs for paid files expire.
- Guest generation is rate-limited on the server.
- Public pages may show cover art and short previews. Full WAVs of a paid pack stay private after the pack has a price. Until then the pack is a repo artifact, not a public download on the marketing site.

## Build order

1. **Store shell.** Header, footer, home. Crest in the hero, maroon floppy in the nav. Palette in `WEB_DESIGN_OPS.md`. Generator leaves `/`.
2. **Memphis Vault page.** Cover, 56-sound facts, real filenames, preview only if the audio file is the real WAV. No price until one is set. No sibling products.
3. **Factory route.** `/drum-factory` serves `KitGenerator` with the current APIs.
4. **Sell the pack.** Human audition, owner sets the Stripe price, license text is final, webhook grants the ZIP.
5. **Repeatable factory.** Same path as Memphis Vault for the next recipe: render, `kit_qa.py`, listen, draft.
6. **Account library.** Purchases and generated kits on one account.
7. **Factory plans.** Free, Pro, and Lordz as rows with limits. Sample prices stay labeled sample.
8. **Later.** Beat Lab, Sound Vault, synth engines, MPC program export for customers, plugin hub, marketplace. Shared C++ DSP (`shared/dsp/`) is the long-term core for browser and VST. It does not yet render these drums.

## Extension points

- New catalog pack: a folder under `disklordz/sound-factory/kits/`, a passing `qa.json`, then a store draft. No second pack on the site before that.
- New factory engine: `src/lib/generation/` first. A VST of the same engine comes after the recipe is stable.
- New store section: add a route only when the content is true. Planned modules get a planned label or stay off the page.

## Agents

| Agent | Role |
|-------|------|
| `web-design` | Stitch and layout. Does not implement the app. |
| `web-ops` | Preview, kit checks, deploy prep. No production deploy without confirmation. |
| `product-factory` | Batch folder packs through the website API. |
| `conversion-qa` | Generate → preview → checkout smoke. |
| `billing-ops` | Stripe. Writes need confirmation. |

## Related docs

- [docs/WEB_DESIGN_OPS.md](docs/WEB_DESIGN_OPS.md)
- [docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md)
- [docs/DISKLORDZ_GO_LIVE.md](../../docs/DISKLORDZ_GO_LIVE.md)
- [disklordz/sound-factory/ARCHITECTURE.md](../sound-factory/ARCHITECTURE.md)
- [DEPLOY.md](DEPLOY.md)
