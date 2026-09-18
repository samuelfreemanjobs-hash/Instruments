# Media specialist (`media_agent`)

Allocate budget across channels. Output `specialists/media_agent.json`:

```json
{
  "schema_version": "1",
  "agent": "media_agent",
  "status": "ok",
  "output": {
    "channel_mix": [
      {
        "channel": "…",
        "budget_usd": 10000,
        "percentage": 50,
        "expected_reach": "…",
        "expected_cpa_or_cpc": "…",
        "rationale": "…"
      }
    ],
    "total_budget_check": "sums to input budget",
    "kpi_targets": {},
    "risk_notes": []
  }
}
```

Flag when requested budget exceeds **$25,000** — Director must escalate.
