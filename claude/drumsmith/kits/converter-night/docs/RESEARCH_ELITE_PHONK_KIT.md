# Research: How to ship an elite commercial phonk drum kit

**Question:** What product structure, sonic promises, and price band must Converter Night / DiskLordz hit to compete with elite Memphis/phonk drum kits — and what should we build next?  
**Success:** Sourced competitor map; ranked gap fills; CNV-specific wedge that does not clone hardware claims.  
**Non-goals:** Remixing competitor PCM; artist-named SKUs; setting final street price without Sam HITL; claiming authentic Boss/E-mu dumps.

## Verdict

Compete on **product structure buyers already pay for** (parallel RAW/TAPE/LOFI of the *same* drums, BPM-labeled construction loops, matched oneshot↔loop mini-kits) while owning a **different honest wedge**: original converter-crawl / RATE subs / EDGE hats / BIT12 grit — not “DR-5 kit” or “SP-1200 kit.” Price into the **$20–$34 drums-only** band; melody-heavy packs sit higher (~$50). Weakest tag on the verdict: **[inspected]** (public storefronts + prior in-repo competitor brief). Ear superiority remains **[assumed]** until HITL vs demos.

## Findings

| Claim | Tag | Source |
|-------|-----|--------|
| Loaded Samples MU V1 markets DR-5 authenticity, >1 GB, cassette/lo-fi parallel drums; best-seller framing | inspected | loadedsamples.com/products/memphis-underground-vol-1 scrape 2026-11-17; og:description |
| MU V1 street ~$24.99; V2 ~$29.99; Lo-Fi Memphis ~$49.99 | inspected | `docs/COMPETITOR_LOADED_SAMPLES.md` + collection pages |
| V1 counts: 220 original / 86 tape / 39 lo-fi drums + 82 instrument oneshots | inspected | COMPETITOR_LOADED_SAMPLES.md (shop 2026-11-17) |
| V2: SP-1200 + cassette story; 357/156/141 process lanes + 23 BPM loops; 700+ WAV | inspected | COMPETITOR_LOADED_SAMPLES.md |
| Buyer expectation #1 = same-hit parallel banks (clean/tape/lofi) | inspected | COMPETITOR_LOADED_SAMPLES.md gap map; shared L-0015 |
| ODD SMPLS Drift kit: $19.95 ZIP; 198 files; 24-bit/44.1; heavy **melody/cowbell loops**, only 13 drum oneshots + 18+18 drum/top loops | inspected | loopmasters.com Drift Phonk Sample Kit scrape |
| LANDR “Memphis Style Phonk Drums”: **10 complete kits** = 8 oneshots + matching loop; loops 65–80 BPM half-time (double for 130–160 drift) | inspected | samples.landr.com/packs/memphis-style-phonk-drums |
| Free YouTube/Soundpacks kits flood the market → paid kits must win on **organization, processing lanes, demos, license clarity** | assumed | search results (free kits dominate SERP) |
| CNV already has 512-hit design, 36 true loops @ 94/134, deep FX, RATE/BIT12 banks — but single-pass process story vs parallel lanes | inspected | kit-spec.md, status.json, COMPETITOR gap map |
| CNV unique wedge: rate-drop subs + converter snap + EDGE hats; original synth provenance | inspected | R2P.md, kit-spec.md |
| DR-660 WAV still unstaged → cannot honestly market “DR-660 dump” | executed | IMPROVEMENTS.md / references/dr660/STAGED.md |
| Elite = technical QA PASS ≠ commercial release; needs HITL ear/price/cover/legal | inspected | RELEASE_GATE / status.json `release_ready:false` |

### Conflict / dissent

- **Hardware authenticity vs originality:** Loaded Samples sells “exact rare drum machine sounds.” CNV must **not** copy that claim without ROM rights. Prefer “original synthesis inspired by converter crawl / 12-bit grit” `[inspected]` R2P.  
- **Drift packs vs Memphis kits:** Drift (Loopmasters) wins with cowbell/melody loops and sparse drum oneshots; Memphis kits win with deep drum banks. Elite DiskLordz kit should stay **drums-first** and optionally add a thin cowbell/instrument lane — not become a melody pack `[assumed]` product strategy.  
- **Bit depth:** Loopmasters markets 24-bit; CNV verify_kit is PCM16. Keep PCM16 for device honesty unless verify_kit upgrades `[inspected]` kit-spec.

## Competitive playbook (ordered ROI)

1. **Parallel RAW / TAPE / LOFI** of core kick/snare/hat/clap (match MU structure without their PCM).  
2. **Matched mini-kits** (oneshots + same-voice loop) with **BPM in filename** (LANDR pattern × CNV 94/134).  
3. **Demo beats + SoundCloud-style listen pack** before ask price.  
4. **Cowbell / clave oneshots** (drift crossover) — small count, high perceived value.  
5. **MIDI packs** next to loops (competitor gap CNV already queued).  
6. Stage DR-660 + SP refs for ear-honest RATE/BIT12 — analysis only.  
7. Price HITL: drums-only **$24–$34**; do not chase $50 melody SKUs unless instruments ship.

## Method

- Repo: `COMPETITOR_LOADED_SAMPLES.md`, `kit-spec.md`, `R2P.md`, `RESEARCH_MEMPHIS_TOP20.md`, `IMPROVEMENTS.md`, agent memory L-0015.  
- RAG: chunks missing (no chunks.jsonl).  
- Web: Firecrawl search Memphis/phonk + drift kits; scrape MU V1, Loopmasters Drift, LANDR Memphis Style Phonk Drums (2026-11-17).

## Open questions / HITL

- Final store title + street price + cover art.  
- Ear A/B vs MU V1 demos (SoundCloud) once competitor WAVs attached analysis-only.  
- Whether to ship optional `05_INSTRUMENT_ONESHOTS` or stay drums-only under $25.  
- License wording for indie vs major (V2-style clearance tiers).

## Next actions

1. **audio-dsp-engineering / this lane:** Render parallel RAW/TAPE/LOFI banks + BPM-matched mini-kits from MEMPHIS engines.  
2. **product-design:** Lock storefront tree + naming for elite pack zip.  
3. **customer-and-competition-intel:** Price recommendation card for Sam.  
4. Attach MU V1/V2 zips under `references/competitors/` for ear meters (analysis-only).
