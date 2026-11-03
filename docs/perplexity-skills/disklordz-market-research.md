# Skill: Disklordz market research & competition

Use this skill when the user asks for market research, revenue intelligence, competitor analysis, or pricing benchmarks for **Disklordz** (drum sample SaaS) or the **Instruments** monorepo growth SKU.

## Product context (do not contradict)

- **Promise:** vibe → preview → ZIP download with JSON manifest and provenance (SHA / source ids).
- **Stack:** Next.js on Vercel, Supabase auth/storage, Stripe Pro + credits, parametric sound factory in-repo.
- **Not in scope:** full DAW, infinite catalog, AI training on user uploads (v0 policy).
- **Credibility SKU:** JD Upgraded JUCE plugin (separate from SaaS positioning).
- **Internal ladder:** v0 free limits → v1 Pro → v2 library/API (see repo `docs/DISKLORDZ_SAAS_V0.md` if accessible via connector).

## Audience

- **Primary:** Business Planner + Marketing (human gate before customer-facing claims).
- **Geo:** US/UK English-speaking producers first; note global only when sourced.

## Required deliverable structure

Every run must produce these sections:

1. **Executive summary** — 5 bullets max, actionable.
2. **Market research** — segments, drivers, TAM/SAM with **confidence labels**:
   - `VERIFIED` = vendor pricing page, SEC/press, official blog.
   - `ESTIMATE` = LinkedIn/Tracxn/LeadIQ/Grips/etc.
   - `REPORT` = paid industry report (DataIntelo, etc.) — treat as directional only.
3. **Revenue intelligence** — table: Company | Model | Price tiers | Revenue/funding (if known) | Source URL | Confidence.
4. **Competition analysis** — matrix: Catalog vs Generative vs Full-track; include **Splice**, **Loopcloud**, **Loudly**, **Output Arcade**, **Noiiz**, **Suno/Udio** (substitute threat).
5. **Recommendations** — exactly 5 items tagged `Pricing`, `GTM`, `Product`, `Risk`, or `Partnership`.
6. **Changelog** — “What changed since last run” (pricing, AI features, funding).

## Competitors to always check

| Name | Why |
|------|-----|
| Splice | Incumbent + AI (Variations, Craft, INSTRUMENT, Spitfire) |
| Loopcloud | Price leader + points |
| Loudly | Closest generative sample web comp |
| Output Arcade | $10/mo loop subscription norm |
| Tracklib | Dual catalog (optional — sample clearance angle) |
| Suno | Substitute for “quick beat” creators |

## Disklordz differentiation (score competitors against these)

- Drum-first / MPC-friendly packs (not generic full-track).
- Provenance manifest on every export.
- Artist preset lanes (not only generic genres).
- Parametric factory (spec: BPM, key, mode, engine) — not black-box only.
- Plugin credibility without requiring DAW for v0.

## Research rules

- Prefer sources **≤ 12 months old** for pricing; flag stale pricing explicitly.
- Never invent ARR; if unknown, say `unknown`.
- Do not copy marketing TAM numbers without labeling `REPORT` and naming the publisher.
- No legal advice on sampling — note “royalty-free claim” vs “cleared masters” distinction only.

## Output format

- Main: Markdown or Google Doc with headings matching sections above.
- Appendix: CSV `competitor_pricing_YYYY-MM-DD.csv` with columns: company, plan, price_usd, billing, credits_or_limits, url, date_checked.
- If Slack connected: post executive summary only (no long tables).

## Recurrence

When user sets a schedule:

- **Weekly:** pricing + competitor AI feature diff.
- **Monthly:** full doc refresh + SWOT paragraph.
