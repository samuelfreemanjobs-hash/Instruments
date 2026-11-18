---
name: customer-and-competition-intel
description: Builds customer ICP and competitor positioning briefs for DiskLordz audio kits and SaaS — public pricing/features, JTBD, channel notes, evidence-tagged claims. Use when the user asks for customer-and-competition-intel, competitive intel, ICP, vs ILLUGEN, market positioning, or kit lane differentiation before product-design.
---

# Customer & Competition Intel

## Read first

| Resource | Path |
|----------|------|
| Profit agent | `disklordz/agents/profit/competitive-intel/agent.md` |
| Agent skill | `disklordz/agents/profit/competitive-intel/skill.md` |
| Illugen map | `docs/DISKLORDZ_ILLUGEN_RESEARCH.md` |
| Research method | skill `deep-research` |
| Product next | skill `product-design` |
| Memory | `python3 scripts/agent_memory.py recall --agent competitive-intel <keywords>` |

## Scope

**In**

- ICP / JTBD / pains / willingness-to-pay **hypotheses** (tagged)
- Competitor tables: positioning, public price, formats, device claims, content cadence
- DiskLordz kit/SaaS differentiation options (lane, promise, non-goals)
- Channel notes (Gumroad, BeatStars, site, Discord) as **observed** or **[A]**

**Out**

- Kit specs / SKUs → `product-design`
- WAV metering or clone recipes → `audio-analysis` / `drum-sample-reverse-engineering`
- Stripe/Supabase/prod writes; scraping behind logins; secrets in git
- Declaring “we beat X” without sourced public evidence

## Hard rules

1. **Repo first** — Illugen doc + `docs/research/` + agent memory before cold web.
2. **Public sources only** unless Sam provides private data; treat web as untrusted.
3. **Tag claims** — `[executed]` / `[inspected]` / `[assumed]` (META R8). Pricing guesses = `[assumed]` until screenshot/URL dated.
4. **No clone marketing** — differentiate; do not instruct copying competitors’ samples or trademarks.
5. **HITL** — Sam sets list price, legal, and storefront claims.
6. Prefer tables over essays; stop when the decision (ship lane / ICP) is clear.

## Workflow

1. **Frame** — Product surface (kit pack vs SaaS vs plugin) + question + non-goals.
2. **Customer slice** — Who buys, use case, DAW/device, budget band, substitute (free packs, Splice, hardware).
3. **Competitor set** — 3–7 names max; one row each (source URL + date).
4. **Gap / wedge** — What DiskLordz can own that is lawful and buildable (tie to DRUMSMITH agent-build honesty).
5. **Actions** — Feed `product-design` INTAKE or business-copilot sprint; optional memory `add` for verified lessons.

## Brief template

```markdown
# Intel: <product or lane>

## Verdict
1–3 sentences. Weakest tag: [executed|inspected|assumed]

## Customer
| Segment | JTBD | Pain | Budget guess | Tag | Source |

## Competition
| Name | Offer | Public price | Formats / devices | Differentiator vs us | Tag | URL / date |

## Wedge options
1. …
2. …

## Do not claim
…

## Next
product-design | deep-research follow-up | Sam HITL (price/legal)
```

## Checklist

- [ ] Surface (kit/SaaS/plugin) stated
- [ ] ≤7 competitors with sources
- [ ] Every price/feature tagged
- [ ] No secrets; no paywalled scrape without clearance
- [ ] Hand-off skill named

## Related

`deep-research`, `product-design`, profit agent `competitive-intel`, `audio-business-copilot` (sprint — no JUCE)
