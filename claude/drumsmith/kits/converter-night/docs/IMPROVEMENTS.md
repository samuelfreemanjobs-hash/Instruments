# How to make Converter Night better (priority)

## Blocker: DR-660 WAV still not on this agent

This Cloud workspace’s uploads folder has **Boss DR-5** and **Boss DR-55** only — **no `Boss_DR-660*.zip`**.

Closest Boss files present:
- `Boss_DR-5__2__0615.zip` / `Boss_DR-5_9f45.zip` (duplicate)
- `Boss_DR-55_5dce.zip`

**Please re-attach** the DR-660 zip in this chat (or drop it into the agent uploads). Once it lands we will:
1. Stage under `references/dr660/samples/` (analysis-only)
2. Meter 808 BD / cowbell / clave / hats
3. Retune RATE recipe + DR-660 loops from real envelopes
4. Flip `references/dr660/STAGED.md` → staged

Until then RATE stays **technique-led** with TR-808 lofi + DR-5 proxies.

## Ranked improvements (do next)

| # | Move | Why | Status |
|---|------|-----|--------|
| 1 | **Stage real DR-660** | Unlocks authentic converter crawl meters | blocked — re-attach zip |
| 2 | **Distinct FX DSP families** | Crash / alias / reverse / stab / texture sound different | **done** (native `fxFamily` 0–4) |
| 3 | **BIT12 alias stage** | Sample-and-hold + 12-bit quantize on aliased FX | **done** (fam 1) |
| 4 | **Competitor gap fill** | Loaded Samples MU V1/V2 → parallel tape/lofi banks, instruments, BPM names | **analysis + elite preview pack** — see `RESEARCH_ELITE_PHONK_KIT.md`; `build_elite_phonk_kit.py` ships RAW/TAPE/LOFI + matched mini-kits w/ BPM filenames — HITL before storefront merge |
| 5 | **Archetype MIDI packs** | Ship groove MIDI next to loops for DAW users | research ready — `RESEARCH_MEMPHIS_TOP20.md`; impl queued |
| 6 | **SP-1200 / Drumulator refs** | Lock BIT12 honesty | blocked — WAV missing |
| 7 | **HITL listening pass** | Price/cover/ear — only path to `release_ready` | open |
| 8 | **Product rename** | Still awaiting locked store title | blocked — need title |

## Already shipped

- 512-hit ceiling mix · five FX family DSP branches · BIT12 S&H on aliased FX · Memphis archetype loops · DR-660-technique blueprints · listen-web preview
- Technical QA PASS on masters · diversity PASS · `release_ready` false until HITL
