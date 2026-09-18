---
name: brand-voice
description: Apply the brand voice and writing style for Instruments and Disklordz to any external-facing or customer-touching content. Use whenever generating copy, creative concepts, social posts, ad copy, email content, landing page text, blog posts, or any written material that will represent the brand — including drafts, revisions, and approval reviews. Also use when evaluating whether existing content sounds on-brand or when asked to make content sound "more like us."
---

# Brand Voice — Instruments + Disklordz

> **Status:** Active (repo v1.0)
> **Owner:** Marketing / owner
> **Review cadence:** Quarterly
> **Used by:** Copy specialist, Creative specialist, Marketing Director (synthesis), `business-content` teammate

**Canonical path:** `docs/business-agents/skills/brand-voice/SKILL.md`  
**Snapshot in brand memory:** `tools/marketing-director/brand_memory/instruments_disklordz.json`

## The 30-second test

Before publishing anything, ask: *Does this sound like a producer tool we’d actually use in the session, or could any generic plugin/SaaS have posted it?* If you can't tell, run the rewrite test at the bottom of this doc.

## Voice attributes

Instruments + Disklordz sound like:

1. **Musician-first** — we talk to people who make beats, not to "users" or "stakeholders"
2. **Honest about the gear** — we describe what the DSP, workflow, or kit actually does; no fake nostalgia
3. **Direct** — short paths to the point: sound, workflow, export, install
4. **Studio-culture fluent** — trap, phonk, pluggnb, MPC Sample, DAW-native — without forced slang
5. **Underground-respectful** — confident, not corporate; never punch down at other producers or brands

## Vocabulary

### Words we use freely

- wavetable, sampleless, preset, kit, one-shot, 808-style, MPC Sample, export WAV
- trap, phonk, pluggnb (when the product lane matches)
- demo, beta, waitlist, preset pack, synth, plugin, PWA, generate a kit
- in your DAW, in the browser, on your phone, session, mix, bounce
- documented, verified compatibility (only with matrix evidence)
- draft — verify before publish (internal drafts)

### Words we avoid

- revolutionary, game-changing, best-in-class, world-class, industry-leading
- seamless, unlock, empower, next-generation, cutting-edge, synergy
- analog warmth / vintage mojo (unless spec-backed and qualified)
- AI magic / powered by AI (without saying what the product does)
- guaranteed hits, chart domination, viral, #1 producer secret
- trash talk toward named competitors

### Words we never use

- Guaranteed chart, streaming, or income outcomes
- "RELEASE READY" or "ship ready" as marketing claims (internal gates only)
- DAW names in compatibility claims without a verified matrix
- Conflating **TR-808 PWA**, **NovaDrum/JUCE drum products**, and **Disklordz kit SaaS** as one product
- Medical, financial, or legal promises in product copy

## Sentence patterns

### Default rhythm

Short hooks. Medium explanation. Long only when teaching a workflow step-by-step.

### Things we do

- Lead with the sound or workflow outcome, then the mechanism
- Use **you** more than **we**; **we** is fine for brand posts when owning a launch
- Name the lane (plugin vs SaaS vs PWA vs Vital pack) in the first screen
- Active voice; concrete nouns (OSC, filter, kit slots) over abstract "experience"
- Put CTAs as verbs: install, download demo, generate kit, export WAV

### Things we don't do

- Rhetorical question stacks in paid ads
- Hype adjectives without a checkable claim
- Exclamation points in email subjects or landing H1 (social: sparingly)
- Three-adjective stacks ("powerful, intuitive, professional")
- Competitor comparisons without research_agent sourcing

## Channel adaptations

| Channel | Voice adjustment | Example |
|---|---|---|
| Email subject lines | Punchy, specific sound/workflow hook, &lt;50 chars | `Three 909-style leads without samples` |
| Long-form blog | Patient, teach the workflow; link to ARCHITECTURE/spec | Explain MPC Sample export steps, not hype |
| Paid social | Hook in first 5 words; one idea | `Sampleless 909 weight. In your DAW.` |
| Landing page H1 | Specific benefit; product name OK | `Trap/phonk wavetable synth — no samples required` |
| Customer email | Helpful, name the issue, no cute | `Your kit export failed — here’s the retry path` |
| LinkedIn post | Professional but human; founder voice OK | `We shipped X because producers asked for Y` |
| Push notification | One idea, &lt;60 chars | `New WAVE-909 preset bank live` |
| Sales / deck | Evidence-led; screenshots, spec bullets | Feature list tied to documented behavior |

## Examples: on-brand vs. off-brand

### Example 1: announcing a new synth feature

❌ **Off-brand:**

> We're thrilled to unveil our revolutionary wavetable engine, empowering you with cutting-edge trap vibes to unlock your full potential!

Why it misses: generic SaaS launch tone; no mechanism; uncheckable hype.

✅ **On-brand:**

> WAVE-909’s new wavetable bank hits harder on 808-weight subs without loading samples. Open the preset menu, pick **Phonk Stack 03**, tweak filter drive. Demo build is in the release notes.

Why it works: specific product, actionable steps, honest scope.

### Example 2: Disklordz kit SaaS

❌ **Off-brand:**

> Our AI magic generates infinite hits guaranteed to blow up your beats!

Why it misses: forbidden outcome claim; "AI magic" with no description.

✅ **On-brand:**

> Generate a drum kit from your prompt, preview one-shots, export WAVs for your DAW or MPC Sample workflow. Kits stay in draft until you approve — nothing posts live without you.

Why it works: describes workflow; no guaranteed outcomes; human publish gate.

### Example 3: TR-808 PWA vs other products

❌ **Off-brand:**

> Download our 808 app — the full Disklordz studio in your pocket!

Why it misses: conflates PWA with SaaS/plugin ecosystem.

✅ **On-brand:**

> **TR-808 PWA** — browser 808-style drums, export WAV. Not the Disklordz kit site; not the NovaDrum plugin. Install from the PWA link in the docs.

Why it works: clear product boundary; reduces compliance risk.

## The rewrite test

Take any sentence the team generates. Apply these five checks:

1. **Generic test:** Could a random plugin or beat SaaS have written this? If yes, rewrite.
2. **Vocabulary test:** Does it use our lane-specific words, or generic marketing-speak?
3. **You/we test:** Is there a **you** or only **we**?
4. **Specificity test:** Did you make a claim? If yes, is it checkable against spec/docs?
5. **Read-aloud test:** Read it out loud. Person in the studio, or brochure?

**If it fails 2+ checks, rewrite.** If it fails 4+ checks, start over from the brief.

## When to escalate to brand team

Flag for human brand/owner review when:

- Content targets a new audience segment (e.g. first retail box, first enterprise)
- Sensitive topics: pricing changes, layoffs, crisis, politics
- Deliberate departure from guidelines ("be more aggressive")
- Specialist output fails the rewrite test across multiple revisions
- Regulated-adjacent claims (health, finance, minors)

Document escalations in `business-agents/marketing-director/runs/<slug>/escalation.json` or PR comment.

## Versioning

Pin this path in run `manifest.json` as `brand_voice_skill_version` when auditing campaigns.

- `v1.0.0 — 2026-09-18` — Initial repo skill for Instruments + Disklordz; aligned with brand memory JSON and content-marketing prompt.
