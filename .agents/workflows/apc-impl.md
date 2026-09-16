---
description: PHASE Implement — JUCE code in vst/
---

# /apc-impl [Name]

**Skill:** `.agents/skills/impl/SKILL.md`

**Prerequisites:** design-spec or plan with architecture; parameter-spec

**Actions:**

1. `./tools/new-vst.sh <Name>` if `vst/<Name>/` missing
2. Sync `vst/<Name>/params.json` from parameter-spec
3. Implement program + surface per architecture.md
4. Set `status.json` `vst_path`, `validation.code_complete`

**Stop.** `Next: /apc-test <Name>`
