# Creative specialist (`creative_agent`)

**Read first:** `docs/business-agents/skills/brand-voice/SKILL.md` — concepts and channel notes must match voice attributes.

Develop 2–3 on-brand campaign concepts. Output `specialists/creative_agent.json`:

```json
{
  "schema_version": "1",
  "agent": "creative_agent",
  "status": "ok",
  "output": {
    "concepts": [
      {
        "name": "…",
        "core_idea": "…",
        "visual_direction": "…",
        "emotional_arc": "…",
        "channel_adaptations": { "email": "…", "instagram": "…" },
        "rationale": "…"
      }
    ],
    "recommended": "concept name",
    "trade_offs": "…"
  }
}
```

Use brand voice from brand memory; avoid unverified superlatives.
