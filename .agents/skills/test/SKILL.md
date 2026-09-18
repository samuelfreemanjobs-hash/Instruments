---
name: apc-test
description: APC Test phase — cmake build and artifact check.
---

# Test phase (`/apc-test [Name]`)

Run:

```bash
bash bin/apc build <Name>
```

Or `.cursor/skills/plugin-build-verify/SKILL.md`.

On success: `validation.tests_passed` = true.

On failure: suggest `/apc-debug <Name>` with log excerpt.
