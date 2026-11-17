---
name: kit-product-spec
description: W1 kit spec + W4 device export planning from INTAKE
---

# Skill — kit product spec

## When

- New kit (W1) or device export pass (W4)

## Procedure

1. Read `INTAKE.md`; halt on unanswered blocking rows.
2. Write `kits/<slug>/kit-spec.md`: concept, hit manifest, folder tree, bus chain, verification CLI, device table (UNVERIFIED until sourced).
3. Bar length: `python3 -c "from scripts.dsp_math import bar_length_samples; print(bar_length_samples(44100, BPM))"` from package root.

## Success

- [ ] S/C/A tags on numeric claims
- [ ] Ship status explicit (NOT READY until verify + devices)
- [ ] Provenance path named