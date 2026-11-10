# DiskLordz Custom Bass Oscillators — product brief (internal + store draft)

**SKU idea:** `DLZ-CBOSC-14` · **Version:** 1.0.0

## Proposed product names (customer-facing — no third-party trademarks)

1. **DiskLordz Bass Forge: 14 Custom Oscillators** — clear, SKU-friendly  
2. **The 14 Bass Pack for logue & NTS-1 mkII** — search-oriented  
3. **Genre Bass Machine: Custom Oscillator Collection** — direct-response / genre hook  

**Recommendation:** Lead with **Bass Forge** on DiskLordz.com; use subtitle “14 loadable oscs for NTS-1, xd, Prologue, mkII.”

## Short store description (punchy)

> Stop scrolling presets. **14 bass oscillators** you load straight into your Korg — one file per vibe. Phonk 808, trunk subs, FM electric, G-funk, squelch stacks, 80s chorus poly, and more. **Four hardware folders** in one download: NTS-1, minilogue xd, Prologue, NTS-1 mkII. Build the low end for the track you're on **today**, swap the osc tomorrow.

## Long store description

> You bought the Korg. You still need **bass that sits in the mix** without a plugin tower.  
>
> **DiskLordz Custom Bass Oscillators** is a curated pack of **14** logue SDK user oscillators — each one a full custom osc binary, not a one-shot sample. Load **Phonk 808 Kick** for the drop, swap to **Memphis Trunk Sub** for the verse, throw **Analog Squelch Stack** when the filter needs to talk.  
>
> Every sound ships in **four formats**: `.ntkdigunit`, `.mnlgxdunit`, `.prlgunit`, and `.nts1mkiiunit`. Same DSP, your board.  
>
> Inside the zip: **Quick Start** (KONTROL Editor steps), **per-oscillator control charts** pulled from source (real param names and ranges), and **preset knob recipes** so you're not guessing.  
>
> This is for producers who live on hardware but move fast — **genre-first**, one osc at a time, the way the NTS-1 was built to be used.

## Feature bullets (grounded in actual DSP)

- **14 finished bass/kick oscillators** — subtractive stacks, FM-ish textures, 808 kick engine, wavetable sub, ensemble chorus bass
- **Analog Squelch Stack** — 3-osc saw/square stack, sub, drive, glide, envelope + host-resonance-aware squelch macro
- **Phonk 808 Kick** — sine/triangle morph, ADR, pitch sweep, LP modes, sub boost, tanh drive (kick range, not pad bass)
- **Memphis Trunk Sub** — phase-locked low oscillators for southern trunk bass
- **Dark Wavetable Sub** — embedded wavetables + dark filter path
- **Four device trees** in one product zip — mkI NTS, xd, Prologue, NTS-1 mkII
- **Preset tables** from `manifest.json` — documented knob values per style
- **Reproducible build script** — `./tools/package-disklordz-bass-product.sh` for the next drop

## Trademark / legal notes

- **Store copy above avoids** Moog, Voyager, Juno, Prophet, DX7, TR-808, Ensoniq, etc.
- **In-device display names** in some files may still show legacy internal titles (e.g. panel name from manifest). Before retail, Sam may want a header pass + Korg `dev_id` allocation.
- **Risk:** Genre marketing that implies endorsement by vintage brands — keep “inspired by” only in blog posts if needed, not product title.

## Suggested price range (suggestion only)

| Tier | Price (USD) | Reasoning |
|------|-------------|-----------|
| Launch intro | **$29–39** | 14 units × 4 platforms ≈ 56 binaries; comparable to niche sample packs but reusable on hardware |
| Standard | **$49** | After demo videos + hardware sign-off |
| Bundle upsell | **$69** with future DiskLordz kit | If sold beside phonk/808 content |

**Suggestion:** Launch at **$34** single SKU until demo audio and 2-minute install video exist.

## Assets still needed (Sam)

- [ ] 60–90s screen capture: load one `.ntkdigunit` into NTS-1 via KONTROL Editor  
- [ ] 14× **10-second audio demos** (one per osc, same MIDI riff, labeled by store name)  
- [ ] Product hero image (hardware + waveform, no trademark logos)  
- [ ] Stripe product + download delivery (Supabase or static CDN) on Disklordz.com  
- [ ] Approved **EULA** (replace `legal/EULA_PLACEHOLDER.md`)  
- [ ] Korg **developer IDs** registered if selling broadly  

## Hardware test checklist (Sam — before “tested” claim)

- [ ] NTS-1 mkI: load all 14 `.ntkdigunit`, note on/off, CPU dropouts  
- [ ] NTS-1 mkII: all 14 `.nts1mkiiunit`  
- [ ] minilogue xd: spot-check 3 units + one squelch/stack osc  
- [ ] Prologue: spot-check 3 units  
- [ ] Verify preset knob values match manifest on real panel order  
- [ ] Peak levels / clipping on master out  
- [ ] Confirm only one custom osc slot behavior documented for buyers  

## Rebuild bundle

```bash
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/package-disklordz-bass-product.sh
```

Output: `release/products/disklordz-custom-bass-oscillators-<version>.zip`
