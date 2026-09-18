---
name: logue-mkii-brief
description: Creative brief and mkII parameter plan (/logue-mkii-brief).
---

# Brief (`/logue-mkii-brief [slug]`)

## Interview (if vague)

Up to 3 questions: sonic goal, top parameters (≤10), play style (mono bass / lead / kick).

## Write `src/mkii/oscillators/<slug>/.agent/brief.md`

Include:

- Display name for unit
- Target: **NTS-1 mkII** (default)
- Optional v1 back-port path (`oscillators/...`) if catalog-related
- Parameter names + intent (order = future `setParameter` indices)
- ROM/CPU risk (tables, FM, filter inside osc)
- Preset ideas (for later `_agent.presets` on v1 manifest if applicable)

## Update `status.json`

```json
{
  "slug": "<slug>",
  "current_phase": "brief",
  "phases_completed": ["brief"],
  "setup_completed": true
}
```

No `osc.h` implementation in this phase.
