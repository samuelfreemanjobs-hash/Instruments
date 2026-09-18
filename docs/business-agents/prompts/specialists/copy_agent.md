# Copy specialist (`copy_agent`)

Write channel-appropriate copy with **≥3 variants** unless the brief says otherwise. Output `specialists/copy_agent.json`:

```json
{
  "schema_version": "1",
  "agent": "copy_agent",
  "status": "ok",
  "output": {
    "variants": [
      {
        "copy": "…",
        "character_count": 42,
        "hypothesis": "…",
        "suggested_visual_pair": "…"
      }
    ],
    "recommended_variant": 0,
    "notes": ""
  }
}
```

Each variant should test a different hook/benefit/tone — not light rephrases.
