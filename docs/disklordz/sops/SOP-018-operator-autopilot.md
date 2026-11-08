# SOP-018 — Operator autopilot (unknown tasks)

## Purpose

Turn “I don’t know how” into repeatable **Cursor Cloud** runs without adding another orchestration repo.

## Procedure

0. One-time setup (labels `agent-run` / `needs-secrets`, Slack, secrets): [OPERATOR_OPS_MASTER_CHECKLIST.md](../../OPERATOR_OPS_MASTER_CHECKLIST.md).
1. Open GitHub issue: **New issue → Agent run (operator autopilot)** (label `agent-run`).
2. Fill **I don’t know how to**, **Goal**, and **Success criteria**.
3. Copy the generated **Cloud Agent prompt** from the issue body into Cursor Cloud on **Instruments**.
4. Agent follows [SOP-001](SOP-001-cloud-agent-task.md) and [OPERATOR_AUTOPILOT.md](../../OPERATOR_AUTOPILOT.md) router.
5. Review draft PR; merge when CI and artifacts match checkboxes.
6. Weekly: triage `agent-run` issues; run `stub-agent-env-check.sh`.

## Escalation

Production, Stripe live, schema, merge — see R10 gates in [OPERATOR_AUTOPILOT.md](../../OPERATOR_AUTOPILOT.md). Agent documents human steps; does not self-confirm.
