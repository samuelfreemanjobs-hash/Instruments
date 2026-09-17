# Research brief: TR-808 PWA × prompt SaaS × mobile → sampler workflow

**Status:** Discovery — not approved for build.  
**PM:** Business Planner + Marketing · **Star SKU candidate:** growth / lead magnet alongside JD Upgraded (credibility) and TR-808 **plugin** (pro).  
**Related:** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) (on PM branch) · [vst-tr808/](../vst-tr808/) (circuit plugin, separate SKU)

---

## One-line pitch (hypothesis)

A **Progressive Web App** that combines (1) a **playable TR-808-class web instrument**, (2) **prompt-driven generation** of one-shots or niche loops, and (3) a **phone-first export path** so producers can get sounds into **MPC / pad workflow** without a desktop DAW — free tier as **lead magnet**, paid tier as **SaaS**.

---

## Why this might work

| Signal | Implication |
|--------|-------------|
| You already have (or plan) a **TR-808 PWA** | Reuse UI + Web Audio engine as the “toy” and credibility demo |
| **Disklordz SaaS v0** was “prompt → preview → ZIP” | Same backend; PWA is the **front door** and retention surface |
| **MPC / 16-pad culture** | Users want **short, loopable, MPC-ready** WAVs (trimmed, labeled, 44.1 kHz) |
| **Niche loops** (genre/subgenre/bpm) | Differentiates from generic “AI drums” — fits **factory archive + prompts** |
| Competitors (BAP Studio, mloop, web MPC clones) | Prove **browser beat-making** demand; few combine **gen + 808 identity + export UX** |

---

## Product shapes (pick one after research)

### A — Lead magnet (fastest)

- Free PWA: 808 play + **3 prompt gens/day** → single WAV or 4-pad mini-pack  
- Email capture before download  
- Upsell: plugin (TR-808 recreation), JD Upgraded, or **Pro SaaS**  
- **Success metric:** waitlist conversion, cost per lead, % who open email #2

### B — SaaS v0 (revenue path)

- Extends v0 table in DISKLORDZ_SAAS_V0: account, limits, ZIP packs  
- PWA = primary client (not “native app later”)  
- **Success metric:** D7 return, gens/user, first paid conversion

### C — Hybrid (recommended to research)

- **Free:** 808 PWA + 1 niche loop style + watermark / short length  
- **Pro:** unlimited gens, full packs, **“MPC kit” export preset** (naming, trim, velocity layers doc)  
- Plugin track stays **premium accuracy** (WDF circuit); web stays **speed + fun + leads**

---

## User journeys to validate

```text
Journey 1 — Phone producer
  Prompt ("dirty 90s boom bap kick + rim") → preview in PWA → Export WAV
  → iOS Share / Android download → Files → (MPC Beats desktop drag) OR
  → USB-C / ¼" analog into MPC Live 3 sampler

Journey 2 — Niche loop
  Pick tag (e.g. "Detroit electro 130") → 2-bar loop preview → loop WAV + stems ZIP
  → chop in BAP-style browser or MPC

Journey 3 — Lead magnet only
  Play 808 in browser → "Generate your kit" CTA → email → magic link pack
```

**Research must answer:** which journey matches *your* audience (MPC hardware vs MPC Beats desktop vs phone-only beatmakers).

---

## Mobile → MPC: reality check (2026)

| Path | Works? | Notes |
|------|--------|--------|
| **Download WAV from PWA** → transfer to MPC | **Yes** (manual) | Most reliable: Files, AirDrop, USB stick, SD |
| **MPC Beats on phone** | **No** | Desktop-only DAW |
| **MPC Live 3 USB-C sample from phone** | **Partial** | Often **iOS > Android**; rate/format quirks; analog **¼"** more reliable than USB audio |
| **Play PWA on phone speaker → MPC mic/line in** | **Yes** | Latency/noise; good for sketch, not mastering |
| **Web MIDI to MPC** | **Case-by-case** | Useful on desktop; mobile Safari limited |

**Product implication:** Market **“MPC-ready files”** (format + length + naming), not **“one-tap into MPC app”** unless you ship a **companion native bridge** (out of v0). Optional v1: **share sheet** + short tutorial for Live 3 / Force.

---

## Technical stack (proposed)

| Layer | Choice | Notes |
|-------|--------|--------|
| PWA | Next.js or Vite + **vite-plugin-pwa** | Installable, offline shell for 808 UI |
| Audio | **Web Audio API** + AudioWorklet | Share DSP concepts with plugin; not same binary |
| 808 engine | Your existing PWA code → extract `packages/tr808-web` or WASM later | Plugin = WDF; web = simplified VA acceptable for v0 |
| Generation | API route → `disklordz/sound-factory/` (batch jobs) | Start with **curated + parametric**, then ML |
| Export | WAV encoder (client or server), ZIP, manifest JSON | MPC kit = 16 files max, 44.1kHz, consistent peak |
| Auth / billing | Supabase or Clerk + Stripe (post–lead-magnet) | Per SaaS v0 WO sequence |

**Realtime constraint:** Generation is **async** (poll/WebSocket); preview plays **cached** buffer — do not block audio thread.

---

## Differentiation vs plugin

| | **PWA + SaaS** | **TR-808 plugin (vst-tr808)** |
|--|----------------|-------------------------------|
| Accuracy | Good enough / stylized 808 | Circuit emulation target |
| CLAP/VST3 | No | Yes |
| Prompt kits | **Core** | Presets / future |
| Lead gen | **Core** | Upsell from PWA |
| Mobile | **Core** | No |

---

## Niche loops (ideas to test in interviews)

- Single **genre × tempo × era** packs (e.g. “Chicago footwork 160 one-shots”)  
- **Machine-flavored** tags without trademark abuse (“808-style”, “909-style”)  
- **Loop length:** 1 bar vs 2 bar; **stem count:** kick-only vs full kit  
- **Royalty / license** line in manifest (commercial use tier)

---

## Competitive scan (browser)

- [BAP Studio](https://bap.studio/) — MPC pads, mic/drop sample, WAV export; extension to sample other tabs  
- [mloop](https://github.com/gdamdam/mloop) — pads, looper, mobile-minded PWA patterns  
- Web sampler sequencers (16-pad, no account) — commodity UI; your edge is **808 brand + gen**

---

## Open research questions (PM to assign)

1. **Audience:** MPC hardware owners vs desktop beatmakers vs mobile-only?  
2. **Legal:** Prompt training data, style tags, “808” marketing, commercial license tiers  
3. **Quality bar:** Is simplified web 808 enough if plugin is “pro truth”?  
4. **Gen tech:** Parametric + factory scripts vs external API cost/latency  
5. **Lead magnet:** Email only vs account; which CRM (existing stack)  
6. **Pricing:** Free gens/day, Pro price vs Splice/Loopcloud perception  
7. **Existing PWA:** Repo URL, license, gap list vs this brief  
8. **MPC export UX:** Download only vs guided “send to hardware” wizard  

---

## Suggested research WO sequence (Airtable)

| WO | Title | Owner |
|----|--------|--------|
| `WO-2026-SAAS-R-001` | [Research][PWA] 5 user interviews — mobile → sampler workflow | Marketing |
| `WO-2026-SAAS-R-002` | [Research][PWA] Competitive matrix + positioning one-pager | Marketing |
| `WO-2026-SAAS-R-003` | [Research][PWA] Tech spike — PWA + async gen + WAV export on iOS Safari | Cursor / web |
| `WO-2026-SAAS-R-004` | [PM][PWA] Decide A/B/C product shape + update DISKLORDZ_SAAS_V0 | business-planner |

Implementation WOs stay **blocked** until R-004.

---

## Repo target (when approved)

```text
disklordz/website/          # Next.js PWA shell
disklordz/sound-factory/    # generation + manifests (future)
packages/tr808-web/         # optional shared 808 DSP with PWA
```

---

## Decision log

| Date | Decision |
|------|----------|
| 2026-09-17 | Brief created; NovaDrum naming not used; plugin vs PWA explicitly split |
