---
name: product-design
description: Designs sellable audio product specs — kit/SKU concept, hit lists, naming, folder trees, device-export plans, provenance, and channel notes without writing DSP. Use when starting a drum kit or sample pack, filling DRUMSMITH INTAKE, writing PRODUCT_SPEC, planning MPC/FL/Maschine exports, or when the user mentions product-design, kit spec, SKU taxonomy, or storefront pack structure.
---

# Product Design (audio kits & packs)

## Read first

| Area | Path |
|------|------|
| DRUMSMITH agent | `claude/drumsmith/project.md`, `claude/drumsmith/INTAKE.md` |
| Kit skill (package) | `claude/drumsmith/.claude/skills/kit-product-spec/SKILL.md` |
| Example product | `claude/drumsmith/examples/tail-light-static/PRODUCT_SPEC.md` |
| Release gate (later) | `claude/drumsmith/docs/RELEASE_GATE.md` |
| DSP (not this skill) | skill `audio-dsp-engineering` |

## Scope

**In**

- Concept, positioning, genre lane, promise line
- SKU / naming / folder taxonomy
- Hit-list counts and category mix
- Device export **plan** (UNVERIFIED until tested)
- Provenance stub + channel/mix notes for humans
- Price hypothesis only as **HITL** (Sam decides)

**Out**

- Voice DSP, render CLIs, `processBlock` → `audio-dsp-engineering`
- WAV peak/pitch QA → `verify_kit.py` / CALIPER / `sample-delivery-qa`
- Commercial “ready to ship” claims without named release gate

## Hard rules

1. **Phase awareness** — Agent-build kits may ship technical examples; do not imply storefront certification.
2. **Device honesty** — Mark FL Studio / MPC / Maschine / MV-1 / Lofi12 as **UNVERIFIED** until primary docs or device tests exist (`docs/device-tests.md`).
3. **No clone claims** — Differentiate without copying uncleared commercial packs.
4. **Provenance** — Synthesized / recorded / sliced must be stated; “original” only when true.
5. **Numbers** — Durations/BPM/counts that come from math are **C**; market guesses are **A**.

## Workflow (W1-style)

1. Fill or update **INTAKE** (13 questions) — block on required blanks for ship-bound kits; agent demos may proceed with tagged `[A]` defaults.
2. Write **PRODUCT_SPEC** / `kits/<slug>/kit-spec.md`:
   - One-line promise
   - Lane + BPM anchor
   - Hit list table (category × count × naming)
   - Folder tree
   - Device export table (status column)
   - Open questions / HITL
3. Name masters: `<SKU>_<CATEGORY>_<DESCRIPTOR>_<KEY-or-NA>_<INDEX>.wav`
4. Layout: `masters/`, `exports/<verified-profile>/`, `docs/`, `previews/`
5. Hand off synthesis to DSP lane; keep recipe IDs / seeds in the manifest when present.

## Naming & folders

```text
<kit-root>/
  masters/
  exports/          # only after profile TESTED
  docs/
    provenance/<kit>.md
    device-tests.md
  previews/
  manifest.json     # machine source of truth after render
```

## Output template

```markdown
# <Kit name> — product spec
**SKU:** … · **Lane:** … · **BPM:** … · **Status:** agent-demo | draft | ship-candidate

## Promise
…

## Hit list
| Category | Count | Naming |
|----------|------:|--------|

## Folders
…

## Device exports
| Device | Status |
|--------|--------|
| … | UNVERIFIED |

## HITL
- [ ] Price · [ ] Cover · [ ] Ear sign-off (commercial only)
```

## Checklist

- [ ] INTAKE required rows answered or explicitly deferred
- [ ] Hit counts sum to planned kit size (e.g. 64)
- [ ] No device marked ready without verification record
- [ ] Provenance path named
- [ ] DSP handoff listed (FORGE / native_factory / recipe id)

## Related

`audio-dsp-engineering`, DRUMSMITH `kit-product-spec`, `sample-delivery-qa`, Disklordz `product-factory` profit agent (SaaS batch packs — separate lane)
