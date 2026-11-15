# Web design and web ops

Two fleet agents split the DiskLordz website. Design specifies screens. Ops keeps the running site and catalog kits honest.

| Agent | Id | Owns | Does not own |
|-------|----|------|----------------|
| Web Design | `web-design` | Stitch prompts, page layout, brand application | Next.js implementation, Stripe, audio DSP |
| Web Ops | `web-ops` | Local preview, deploy prep, kit file checks | Visual redesign, payment writes, production deploy |

Roster entries live under `disklordz/agents/profit/web-design/` and `disklordz/agents/profit/web-ops/`.

## Brand

Marketing pages use the storefront palette: obsidian `#141311`, antique gold `#D1B77D`, burgundy `#541D2B`, forest green `#173F30`, ivory `#F1E8D5`, charcoal `#242321`.

Plugin UI stays on the digital palette in [docs/DISKLORDZ_BRAND_IDENTITY.md](../../../docs/DISKLORDZ_BRAND_IDENTITY.md): black, purple, lime, with gold only for crests.

| Art | Path | Use |
|-----|------|-----|
| Ornate crest | `docs/assets/brand/disklordz-crest-badge.png` | Storefront hero |
| Maroon crowned floppy | `docs/assets/brand/disklordz-floppy-crown-maroon.png` | Navigation mark |
| Memphis Vault cover | `disklordz/sound-factory/kits/memphis-vault-vol-1/cover.jpg` | First kit |

Do not print a founding year. The crest artwork is used as a mark, not as a historical claim. Do not invent reviews, customer counts, or endorsements.

## What is real today

- Drum kit generator in `disklordz/website/` (prompt, preview, ZIP).
- Memphis Phonk Factory, the Dust VST, in `vst/MemphisPhonkFactory/`. The site describes it. It does not host an installer.
- Memphis Vault Vol. 1: 56 synthesized one-shots plus cover, at `disklordz/sound-factory/kits/memphis-vault-vol-1/` and `kits/Memphis-Vault-Vol-1.zip`.
- Stripe Pro credits only when server env is configured. Prices on a mockup are sample figures until billing-ops publishes a live price.

Beat Lab, Sound Vault, Synth Factory, marketplace, academy, and the other workstation modules are planned. A mockup may show them only with a planned label.

## Store and app

The public site sells sounds and tools. Drum Factory Pro and the later labs belong in a dedicated app experience, not on a crowded homepage. A future host such as `app.disklordz.com` is a plan, not a live domain.

Homepage job: Memphis Vault Vol. 1, a short Drum Factory invitation, and plugins marked in development or coming soon.

Flagship line, for the app and for pricing mockups labeled sample: "Your own underground drum kit factory." Do not publish a final subscription price.

## 3.0 map

These stay planned until a route or script actually does the job. Do not draw them as working product.

| Area | Ideas | Now |
|------|--------|-----|
| Intelligent sound design | Text-to-drum, reference analyzer, sound DNA, variations, genre morph, kit curator | Website prompts already steer the live generator. Memphis Vault stores seed and recipe per file. Analyzer, morph, and curator are planned. |
| Signature engines | Memphis 1994, Trap 808, Cassette Lord, Southern Synth Lab | Memphis Vault is one synthesized pack, not those four engines, and not a VST. |
| Commercial kit factory | One-click pack, naming, audio QA, artwork, packaging, draft publish | Memphis Vault is pack 001: named WAVs, approved cover, ZIP, manifest. `kit_qa.py` is the technical gate. Artwork generation, 100-sound one-click, and store draft publish are planned. A human still auditions before a pack is for sale. |
| DAW and hardware | DAW bridge, MPC program builder, hardware transfer, preset cloud, project backup | MPC500 card export exists for lab kits. A customer VST browser does not. |
| Business | Profiles, starter kits, bundle suggestions, demand dashboard | Planned. Suggestions must use real compatibility, not invented popularity. |

## Web Design loop

1. Read this file and the brand identity doc.
2. Attach the crest, the maroon floppy, and the kit cover when a tool accepts images.
3. Homepage sells the kit and the tools. Put the drum machine on its own screen.
4. Hand the approved layout to the website implementation lane. Do not treat a Stitch export as production.

## Web Ops loop

1. Read [../ARCHITECTURE.md](../ARCHITECTURE.md) and [../DEPLOY.md](../DEPLOY.md).
2. On this Windows machine, run one Next process on port 3100. Do not install into `C:\Program Files`.
3. Before calling a kit ready, confirm `manifest.json`, `cover.jpg`, the WAV count, and a passing `kit_qa.py` report. Technical QA does not replace listening.
4. Production deploy, secret changes, and Stripe writes wait for an explicit yes. Funnel smoke stays with `conversion-qa`. Payments stay with `billing-ops`.
