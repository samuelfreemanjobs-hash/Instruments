# SOP-GTM-001 — Product launch and pricing handoff

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-gtm |
| **Consumer seats** | hermes-gtm, hermes-lead, hermes-support |
| **Cadence** | per_launch |

## Purpose

Launch copy, pricing ladder, and support docs align before public store — **Marketing/human approves pricing**.

## Procedure

1. Draft brief:
   ```bash
   python3 disklordz/hermes/scripts/hermes_tool.py gtm brief --product junova --write
   ```
2. Confirm **Tier C** product gates ([SOP-DEV-002](SOP-DEV-002-junova-finish-line.md) for Junova).
3. Landing stub / external repo per product `gtm/` folder.
4. hermes-support: ensure RAG corpus includes FAQ ([SOP-RAG-001](SOP-RAG-001-corpus-support.md)).
5. Human sign-off on price, SKU, Stripe products before go-live.

## Verification

- GTM brief in `disklordz/hermes/outbox/`.
- Launch checklist items ticked in PR or issue.

## Related

- [Junova-X/gtm/README.md](../../../Junova-X/gtm/README.md)
