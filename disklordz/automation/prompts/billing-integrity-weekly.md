# {{WO}} — Billing integrity

Read `.cursor/agents/disklordz-billing-integrity.md`.

1. Run `bash disklordz/automation/scripts/check-billing-integrity.sh`.
2. Audit Stripe webhook coverage, credits path, idempotency, and `.env.example`.
3. Implement **one** small hardening fix or test if needed.
4. Draft PR **`{{WO}}: Billing — <topic>`** on `cursor/billing-<topic>-4170`.

Stripe MCP: read-only unless user confirmed writes. No live charges.
