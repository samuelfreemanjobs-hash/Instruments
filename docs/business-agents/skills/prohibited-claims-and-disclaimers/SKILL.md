---
name: prohibited-claims-and-disclaimers
description: Verify that external-facing marketing content complies with legal restrictions, substantiation requirements, regulatory disclosures, and the company's prohibited claims list. Use before publishing or approving ANY customer-, prospect-, regulator-, or public-facing content — ads, emails, landing pages, social, press, sales decks, product copy. Also use for competitor comparisons, performance claims, AI/automation statements, DAW compatibility, or quantitative assertions. The Compliance Agent must treat this skill as its primary rulebook.
---

# Prohibited Claims and Required Disclaimers — Instruments + Disklordz

> **Status:** Active (repo v1.0) — **not** a substitute for licensed legal review on retail, fundraising, or regulated launches.
> **Owner:** Owner + Marketing (legal sign-off for retail/expansion)
> **Review cadence:** Quarterly, and when adding lanes (retail box, new SaaS billing region, health-adjacent wellness copy)
> **Used by:** Compliance specialist (`compliance_agent`), Marketing Director, **business-ship-eval** (customer-facing accuracy)

**Canonical path:** `docs/business-agents/skills/prohibited-claims-and-disclaimers/SKILL.md`  
**Quick list:** `tools/marketing-director/brand_memory/instruments_disklordz.json` → `prohibited_claims`

## How the Compliance Agent uses this skill

For every review, output JSON matching `docs/business-agents/schemas/compliance-verdict.schema.json`:

```json
{
  "schema_version": "1",
  "severity": "NONE",
  "issues": [
    {
      "claim": "exact text from the content",
      "rule_violated": "section reference from this skill",
      "severity": "MEDIUM",
      "suggested_fix": "specific revision or required disclaimer"
    }
  ],
  "required_disclaimers": [],
  "ready_to_publish": false
}
```

Set `ready_to_publish` to **false** if **any** issue is MEDIUM or higher, or if severity is MEDIUM+.

## Severity rubric

| Level | Meaning | Director's required response |
|---|---|---|
| NONE | No issues found | Proceed (human still publishes) |
| LOW | Minor — recommend revision | Note in output; may proceed if owner accepts |
| MEDIUM | Should fix before publishing | Revise and re-run compliance |
| HIGH | Significant legal/brand risk | Block; escalate to owner |
| CRITICAL | Hard stop — misleading product identity, forbidden outcome claims | Refuse to ship copy; escalate immediately |

Director and teammates **cannot override HIGH or CRITICAL** by re-prompting.

## Hard prohibitions (always CRITICAL)

Never permissible in external content:

1. **Guaranteed creative or commercial outcomes** — chart placement, streaming revenue, viral success, “guaranteed hits,” income from beats/kits/plugins.
2. **False product identity** — claiming one SKU is another (especially **TR-808 PWA** vs **Disklordz SaaS** vs **NovaDrum/JUCE drum plugins** vs **Junova-X**); see `docs/TR808_JUNOVA_PRODUCT_MATRIX.md`.
3. **“RELEASE READY” / retail-ready / ship-ready** as **marketing** language — internal gate language only; requires **business-ship-eval ACCEPT** + human gates per `docs/plugin-factory/GATES.md`.
4. **DAW/OS compatibility** without a **verified matrix** in repo docs or eval artifact (no “works in all DAWs”).
5. **Fake endorsements** — implying endorsement by artists, labels, MPC/DAW vendors, or regulators without written permission on file.
6. **Competitor defamation** — statements about a named competitor’s bankruptcy, fraud, executive conduct, or litigation outcomes.
7. **Pricing or availability** that contradicts the live site, store, or dated price sheet in the PR/intake.

## Substantiation required (MEDIUM if missing, HIGH if contradicted)

Before approving, verify evidence exists in repo or linked owner artifacts. If no record, default **MEDIUM** and request substantiation from author.

| Claim type | What's required | Repo / process location |
|---|---|---|
| DAW / OS compatibility | Verified matrix with versions tested | Product `ARCHITECTURE.md`, `PRODUCT_SPEC`, ship eval |
| “Sampleless” / specific DSP behavior | Spec-backed description | Plugin spec + factory docs |
| “AI-generated” / kit generation | What is automated vs human-reviewed; data use summary | `disklordz/website/` privacy + product ARCHITECTURE |
| “#1” / “best” / “only” in category | Third-party ranking or narrow defined category + date | Owner-provided citation in PR; else MEDIUM |
| “Faster than [competitor]” | Replicable benchmark + date + methodology | Owner substantiation file in PR; else block comparison |
| “Trusted by N producers” | Verifiable count + as-of date | Owner CRM/analytics export referenced in PR |
| Customer quote / testimonial | Signed release or public link with permission | Owner-held release; else MEDIUM |
| Uptime / SLA for SaaS | Measured window + status page | Disklordz ops docs when published |
| Preset count / feature list | Matches shipped build or spec | Release notes / spec |
| Vital / third-party host claims | Vital trademark usage per host guidelines | Pack README + Vital brand rules |

**Substantiation stub (until legal folder exists):** attach evidence paths in the marketing run (`director-notes.md`) or PR description — do not invent `/legal/substantiation/` paths.

## Required disclaimers by category

Place disclaimers **adjacent to the claim** (not only footer), unless noted.

### AI / automated generation (Disklordz, any “AI” marketing)

- **Required text (minimum):** “AI-assisted output varies by prompt; preview kits before use in commercial releases. You are responsible for clearing samples/loops for your projects.”
- **Placement:** First mention of AI generation on page/email.

### Beta / demo / preview builds (plugins, SaaS features)

- **Required text:** “Beta/demo — features and sound may change before general availability. Not for final commercial masters without your own QA.”
- **Placement:** First mention of beta/demo on each asset.

### Third-party trademarks (DAW names, Vital, MPC, 808)

- **Required text:** “[Trademark] is a trademark of its respective owner. Not affiliated with or endorsed by [owner].”
- **Placement:** Footnote or adjacent first use in store/listing copy.

### Comparative / competitive copy

- **Required text:** “Comparison as of [YYYY-MM-DD] based on [public source or our documented test]. Features change; verify before purchase.”
- **Placement:** Adjacent to comparison table or claim.

### Preset / sound packs (no medical/wellness crossover)

- If copy borrows wellness/skincare metaphors for unrelated products, flag **MEDIUM** (“off-lane metaphor — clarify product category”).

### Retail / boxed product (when applicable)

- **Escalate** to owner legal review before external copy — do not auto-approve via agent alone.

## Competitor claims rules

When referencing a competitor by name or clear implication:

1. Frame **what we do**, not “they can’t / they fail.”
2. Side-by-side comparisons need same metric, **as-of date**, and **public** competitor source.
3. No competitor trademarks in **paid ad headlines/H1** without owner legal clearance.
4. No implied partnership or endorsement by competitor.
5. **Escalate to owner** before naming a top competitor in comparative ads — even favorable comparisons.

## Quantitative claim review checklist

For number-based claims, verify:

- [ ] Source cited and dated (<24 months unless evergreen spec)
- [ ] Methodology documented or available on request
- [ ] No misleading rounding
- [ ] Time frame stated (“in 2025” not vague “always”)
- [ ] Population stated (“in our beta cohort” not “all producers”)
- [ ] Fair baseline for comparisons
- [ ] “Up to X” reflects typical range, not a one-off max

Any unchecked item → **MEDIUM** minimum.

## Common patterns (auto-flag)

| Pattern | Severity if unsubstantiated | Why |
|---|---|---|
| “Revolutionary” / “game-changing” | MEDIUM | Hype without proof |
| “Analog warmth” / “true vintage” | MEDIUM | Often unsubstantiated DSP claim |
| “Save $X” / “free forever” | MEDIUM | Define scope; SaaS terms |
| “AI magic” / “instant hit” | MEDIUM–HIGH | FTC-style clarity on automation |
| “Limited time” / fake scarcity | HIGH | Must be true at impression time |
| “Works with every DAW” | HIGH | Matrix required |
| Statistics without citation | MEDIUM | Cite or remove |
| “Used by [famous artist]” | HIGH | Permission required |
| “Patented algorithm” | MEDIUM | Verify status |
| Skincare/beauty claims in instrument copy | CRITICAL | Wrong category + regulated claims risk if extended |

## Product-lane integrity (Instruments-specific)

| Lane | CRITICAL if violated |
|---|---|
| `juce-factory` (e.g. WAVE-909) | Described as SaaS kit generator or browser PWA |
| `disklordz` | Implies offline plugin install or TR-808 PWA |
| `tr808-pwa` | Implied to include SaaS accounts or NovaDrum plugin |
| `content-vital` | Implied to include full synth product |

Reference: `docs/DISKLORDZ_PLUGIN_TRACKS.md`, `docs/TR808_JUNOVA_PRODUCT_MATRIX.md`.

## Escalation paths

| Trigger | Path | Target SLA |
|---|---|---|
| CRITICAL | Owner + stop publish | Same business day |
| HIGH | Owner review in PR / run `escalation.json` | 24 hours |
| 3+ MEDIUM from same campaign in a week | Brand voice + compliance re-read | 1 week |
| New market (EU billing, retail box) | Owner legal **before** copy | Pre-launch |
| Named competitor comparison ad | Owner review | 48 hours |
| Crisis / public incident mentioning brand | Owner + comms | Hours |

## Out of scope (flag MEDIUM + route)

- Trademark clearance for **logos/visuals** — owner/legal
- Privacy/GDPR/CCPA for forms/analytics — `disklordz/website` data team
- WCAG accessibility — design QA
- Per-country localization law — separate market review
- Securities / investment disclosures — never automated

Note in issues: `"out of scope — route to [team]"`.

## Versioning

Pin in run `manifest.json` as `compliance_skill_version` when auditing.

- `v1.0.0 — 2026-09-18` — Initial repo skill for Instruments + Disklordz; aligned with brand memory prohibited list and ship-eval gates.
