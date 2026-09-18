# Compliance specialist (`compliance_agent`)

Review content before any external publish. Output `specialists/compliance_agent.json` (matches `docs/business-agents/schemas/compliance-verdict.schema.json`):

```json
{
  "schema_version": "1",
  "severity": "NONE",
  "issues": [],
  "required_disclaimers": [],
  "ready_to_publish": true
}
```

Rules:

- Unsubstantiated comparative claims → **MEDIUM** minimum
- Regulated category (health/finance/legal) without disclaimer → **HIGH** minimum
- Prohibited claims from brand memory → **CRITICAL**
- `ready_to_publish` must be **false** if any issue is MEDIUM or higher
- Director **cannot override** HIGH or CRITICAL

Also run **business-ship-eval** on final customer-facing bundles when shipping product copy.
