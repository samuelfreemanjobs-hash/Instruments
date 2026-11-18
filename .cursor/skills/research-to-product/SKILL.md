---
name: research-to-product
description: Converts research and market intel briefs into a concrete product draft — INTAKE answers, promise, hit mix, non-goals, and DSP handoff stubs. Use when turning deep-research or customer-and-competition-intel into a kit/SaaS SKU, bridging research-to-product, or starting W1 SPEC from evidence.
---

# Research → Product

## Read first

| Stage | Skill / path |
|-------|----------------|
| Method | `deep-research` |
| Market | `customer-and-competition-intel` |
| Spec write | `product-design` |
| DRUMSMITH W1 | `claude/drumsmith/INTAKE.md`, `examples/tail-light-static/PRODUCT_SPEC.md` |
| DSP later | `audio-dsp-engineering` |

## Scope

**In**

- Map tagged findings → product decisions (lane, promise, size, mix, devices **UNVERIFIED**)
- Fill or patch INTAKE / PRODUCT_SPEC from evidence (keep source tags)
- List open HITL (price, cover, legal, ear)
- Queue DSP / reverse-engineering only as named follow-ups

**Out**

- New cold research (use `deep-research` / intel skills first)
- Writing DSP, rendering WAVs, shipping commercial packs
- Changing Stripe/Supabase/prod without confirmation

## Hard rules

1. **No orphan claims** — every product line that depends on research cites a finding + tag; drop or mark `[assumed]` if missing.
2. **Wedge over clone** — intel “they do X” becomes our lawful alternative, not a copy brief.
3. **Phase honesty** — default status `agent-demo` or `draft` unless Sam opens ship gates.
4. **One SKU per pass** — split multi-product ideas into separate specs.
5. Preserve weakest evidence tag when promoting a claim into the spec.

## Pipeline

```text
deep-research / customer-and-competition-intel
        ↓  (this skill)
decision table + INTAKE draft
        ↓
product-design → PRODUCT_SPEC / kit-spec.md
        ↓
audio-dsp-engineering | drum-sample-reverse-engineering (optional)
        ↓
audio-analysis / CALIPER (after render)
```

1. **Intake gate** — Require (or run) a research/intel brief with Verdict + Findings. If absent, stop and invoke those skills.
2. **Decision table** — For each finding: `use | park | reject` + product field affected.
3. **Draft INTAKE** — Answer DRUMSMITH 13 questions; unknowns = HITL or `[A]`.
4. **Call product-design** — Emit full PRODUCT_SPEC template (do not skip naming/folders/devices).
5. **Handoff list** — DSP recipes needed, analysis targets, Sam checkboxes.

## Decision table template

```markdown
# R2P: <SKU or working title>

## Source briefs
- path/url … (date)

## Decisions
| Finding (tag) | Action | Product field | Notes |
|---------------|--------|---------------|-------|

## Rejected / parked
…

## INTAKE draft
1–13: …

## Spec status
agent-demo | draft | ship-candidate(HITL)

## Next skills
- [ ] product-design (finalize spec)
- [ ] audio-dsp-engineering / RE …
- [ ] Sam: price · legal · ear
```

## Checklist

- [ ] Upstream brief exists with evidence tags
- [ ] Decision table covers all load-bearing findings
- [ ] INTAKE / PRODUCT_SPEC drafted without inventing meters or prices as facts
- [ ] Device rows UNVERIFIED unless tests exist
- [ ] No commercial “ready” language

## Related

`deep-research`, `customer-and-competition-intel`, `product-design`, `audio-dsp-engineering`, `drum-sample-reverse-engineering`
