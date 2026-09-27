# Disklordz support runbook (WO-SAAS-024)

For humans and the **Customer Success agent**. Do not paste secrets or full Stripe payloads into tickets.

## Common issues

| Symptom | Check | Fix lane |
|---------|--------|----------|
| “Daily limit” | Guest IP cap | Sign in; WO-019 rate limit docs |
| “Not enough credits” | `/account`, ledger | WO-020 billing |
| Generate spinner forever (loop/SFX) | Async job — `/api/jobs/:id` | WO-026; Supabase service role |
| Preview 404 | Kit storage backend | WO-019 storage / Supabase bucket |
| Pro not active after pay | Stripe webhook logs | WO-020 webhook + metadata `user_id` |
| Sounds weak / wrong lane | Preset + prompt | WO-018/023 |

## Repro template (for issues)

1. Preset id, prompt, spec (mode/engine/bpm)
2. Signed in? plan?
3. Browser + timestamp UTC
4. `batchId` or `jobId` if available

## Escalation

- **Code fix** → PM router dispatch → appropriate WO agent
- **Refund** → human + Stripe dashboard (agent read-only)
