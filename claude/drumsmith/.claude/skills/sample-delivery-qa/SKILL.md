---
name: sample-delivery-qa
description: W3 render verify + W5 audit via verify_kit.py and CALIPER
---

# Skill — sample delivery QA

## W3

```bash
python3 claude/drumsmith/scripts/verify_kit.py <kit_dir> --bpm <master> --json
```

Interpret:

- FAIL peak/clipping/rate mismatch → block ship
- WARN DC / bar grid / fades → document; ear-check with Sam
- Approximate true-peak — never claim loudness certification

Invoke **caliper** subagent for second pass.

## W5

Checklist: INTAKE vs spec vs disk filenames; device dirs empty if UNVERIFIED; provenance file exists.