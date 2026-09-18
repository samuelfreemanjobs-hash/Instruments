# DISKLORDZ Master Launch Canvas & Engineering Blueprint

**Purpose:** Single source of truth for the phonk **VST + funnel** product line (freebies, tripwires, flagship plugins) and how it connects to this monorepo.  
**Related:** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) (drum kit SaaS) · [DISKLORDZ_PHONK_KIT_AI_SOP.md](DISKLORDZ_PHONK_KIT_AI_SOP.md) (kit creation SOP) · [DISKLORDZ_MARKETING_HANDOFF.md](DISKLORDZ_MARKETING_HANDOFF.md) · [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) · [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md)

---

## Repo mapping (what exists vs blueprint)

| Blueprint tier | SKU | Repo status |
|----------------|-----|-------------|
| Freebie #1 | 1994 Memphis vocal stems | **Planned** — asset pipeline (Demucs/UVR) documented below; no stem bundle in repo yet |
| Freebie #2 | FL/Ableton mixer racks | **Planned** — deliver as `.zip` under `disklordz/launch-assets/` when recorded |
| Tripwire #1 | Tascam 4-track IR pack ($9–17) | **Planned** — 50 IR WAVs + storefront SKU |
| Tripwire #2 | Cassette Smash Lite ($17) | **In progress** — browser DSP preview on `/launch`; JUCE SKU **Track E** (see plugin tracks) |
| Flagship | Memphis Tape FX ($47/$79) | **Planned** — full DSP chain; JUCE **Track E** |
| Vault | Full expansion ($97) | **Planned** — bundle + R2 delivery |
| **Catalog (legacy)** | **Memphis Vault Vol. 1** phonk drum kit | **Shipped (prior release)** — cover + copy on `/launch`; WAVs live outside repo until `launch-assets/` or R2 |
| Marketing web | Interactive soundboard + funnel | **Shipped (v0)** — `disklordz/website/src/app/launch/` |
| Drum SaaS | Prompt kits (growth SKU) | **Shipped** — `/` on same Next.js app |
| Licensing API | Cloudflare Worker + JWT | **Planned** — Lemon Squeezy/Stripe webhooks; SaaS already has Stripe |
| Email funnel | Loops / Resend 4-day sequence | **Stub** — `POST /api/launch/opt-in` + env hooks |

---

## Hyperresearch (optional deep audit)

Run outside CI when investigating DSP competitors, pricing, or funnel benchmarks:

```bash
pip install hyperresearch && hyperresearch install
```

Example prompt (commercial + technical audit for Memphis tape emulation, competitor teardown, Kern funnel benchmarks) lives in product planning notes; chunk outputs into `disklordz/rag/corpus/` if you want RAG-backed copy generation.

---

## Product lineup (5-tier funnel)

```text
[ Freebie: Vocal stems ] ──> [ Tripwire: Cassette Smash Lite $17 ] ──> [ Flagship: Memphis Tape FX ]
         │                              │
         ▼ (declined)                   ▼ OTO / backend
[ Freebie: DAW racks ]     ──> [ Tripwire: IR pack $9 ] ──> [ Vault $97 ]
                                      │
                                      └── includes / upsells: Memphis Vault Vol. 1 (legacy phonk kit)
```

### Legacy phonk drum kit — Memphis Vault Vol. 1

| Field | Detail |
|-------|--------|
| **Customer-facing name** | **Memphis Vault Vol. 1** — *Underground Phonk Samples* (cassette cover art) |
| **Earlier codename** | **Graveyard Shift** — OG Memphis lane from sample-pack brand notes (same aesthetic: skulls, tape, 12-bit) |
| **Credit line** | Produced by **AJ OG** |
| **Folder spec** | `01_Kicks`, `02_808s`, `03_Snares_Claps`, `04_Perc`, `06_FX` (see historical brand chat in git `4461164`) |
| **Launch role** | **Anchor sample SKU** — sell standalone ($27–47 typical) or **bundle inside Vault $97** with IR pack + flagship plugin discount |
| **Web** | Promo block on `/launch`; `LAUNCH_MEMPHIS_VAULT_URL` → Gumroad / Stripe / LS product |
| **SaaS tie-in** | Generator preset **Midnight Circuit** (`DL002`) matches drift/phonk lane for *new* kits; Vault Vol. 1 is the *hand-curated* predecessor |

If you recover the original ZIP, drop it under `disklordz/launch-assets/memphis-vault-vol1/` and point entitlements + download API at that SKU.

### Freebie #1 — Vocal chops

- **Problem:** Mud, 808 bleed, phase issues on old rips.
- **Pipeline:** Archive audio → Demucs v4 / UVR MDX-Net → pitch lock (F♯ min, C♯ min, A min) → micro-fades → `BPM_KEY_VocalChop_Name.wav` ZIP.

### Freebie #2 — “Zero-Dollar Memphis” racks

- FL Patcher: PEQ2 (10.5 kHz cut, 80 Hz bump) → WaveShaper → Chorus (0.3 Hz wobble).
- Ableton rack: Utility mono sub → Saturator → EQ Eight → Shifter + macros **Tape Age** / **Preamp Drive**.

### Tripwire #1 — IR collection

- Hardware: Portastudio 424 MkIII, Fostex X-15, Walkman; sweeps at 96 kHz/24-bit; fresh / worn / +6 dB OD tape.
- Output: ~50 minimum-phase WAV IRs for standard convolvers.

### Tripwire #2 — Cassette Smash Lite (plugin target)

| Control | DSP |
|---------|-----|
| Drive | Asymmetric waveshaper + ~70 Hz transformer bump |
| Wobble | Fractional delay; 0.5 Hz wow + 6 Hz flutter blend |
| Latency | Zero lookahead |

**Browser parity:** `disklordz/website/src/lib/audio/cassette-smash.ts` (Web Audio preview, not bit-identical to shipping VST).

### Flagship — Memphis Tape FX

Full chain: switchable preamp stages → Type-I ferric profiler (8–13 kHz shelf + hiss) → motor drift (wow/flutter/azimuth) → dropouts → vocal dirt matrix (300 Hz–3.5 kHz).

---

## Systems architecture (build vs buy)

```text
                    [ Traffic ]
                         │
                         ▼
              [ Next.js marketing + /launch ]
                         │
         ┌───────────────┴───────────────┐
         ▼                               ▼
  [ Loops / Resend ]              [ Lemon Squeezy / Stripe ]
         │                               │
         ▼                               ▼
  [ 4-day email sequence ]        [ License API (planned) ]
                                         │
                                         ▼
                                  [ Cloudflare R2 signed URLs ]
```

| Function | Strategy | Stack |
|----------|----------|--------|
| Email | Buy | Loops.so or Resend + React Email |
| Checkout | Buy | Lemon Squeezy (MoR) or existing **Stripe** in SaaS |
| Landing + soundboard | Build | Next.js 15 + Web Audio (`/launch`) |
| Licensing | Build | Cloudflare Workers + Supabase (future) |
| Downloads | Assemble | R2 signed URLs |
| Support | Buy | Crisp / Tidio |

---

## Frank Kern funnel (web routes)

| Step | Route | Behavior |
|------|-------|----------|
| Squeeze | `/launch/vocals` | Email → `POST /api/launch/opt-in` → redirect thank-you |
| Results in advance | `/launch` | Three-stem soundboard + **Cassette Smash** toggle |
| Thank-you / tripwire | `/launch/thank-you` | Copy + CTAs (URLs from env) |

Env: see `disklordz/website/.env.example` (`LAUNCH_*`, optional `RESEND_*`).

---

## Phased execution

| Week | Focus | Engineering |
|------|--------|-------------|
| 1 | Stems, presets, IRs | Offline audio; `launch-assets/` zips |
| 2 | Plugins + web | JUCE Track E; `/launch` soundboard (v0 done) |
| 3 | Store + email | Lemon Squeezy products, Resend/Loops sequences |
| 4 | Launch | Short-form clips; open freebie; cart + urgency emails |

---

## Agent workflow

1. Read `/ARCHITECTURE.md`, then this file, then [DISKLORDZ_PHONK_KIT_AI_SOP.md](DISKLORDZ_PHONK_KIT_AI_SOP.md) for kit/factory work, then [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md).
2. Plugin DSP changes: propose **`[Plugin][JUCE] Track E — Memphis Tape`** work order before large `Source/` edits.
3. Funnel/copy changes: `disklordz/website/src/app/launch/` only unless shared components needed.
4. Do not deploy production or send live email without explicit user request.

---

## Extension points

- Replace procedural demo stems with real WAVs in `public/launch/stems/`.
- Wire `RESEND_API_KEY` + audience for double opt-in and Day 1–4 templates.
- Add Supabase `launch_leads` table if you need CRM export without a third party.
- Implement Cloudflare Worker license JWT validation for VST installers.
