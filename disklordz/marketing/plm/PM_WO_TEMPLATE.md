# PM Agent — work order template (paste into Airtable)

**Prefix:** `[PM][Launch]` or `[Factory]` / `[Eng]` / `[Marketing]` as appropriate

---

## WO-LAUNCH-001 — Tripwire sample pack (Day 2 SKU)

| Field | Value |
|-------|--------|
| **Title** | `[Factory] Build Day 2 tripwire phonk sample pack (15–20 one-shots)` |
| **Product** | `DL-LAUNCH-TRIPWIRE-SAMPLE` (create row in Products) |
| **Acceptance** | ZIP in `disklordz/launch-assets/tripwire-sample-v1/`; folders 01–04; LICENSE.txt; peaks legal; **founder sign-off** |
| **Owner** | Cursor Cloud (factory) + **Founder (approve)** |
| **Blocks** | Day 2 cart, email Day 2, Stripe description |

## WO-LAUNCH-002 — Stripe tripwire checkout

| Field | Value |
|-------|--------|
| **Title** | `[Eng] Stripe Checkout for tripwire sample + success URL` |
| **Acceptance** | Price ID in env; test purchase → download link; webhook logs order |
| **Depends** | WO-LAUNCH-001 |

## WO-LAUNCH-003 — Lifecycle email enablement

| Field | Value |
|-------|--------|
| **Title** | `[Eng][Marketing] Wire Resend + schedule 4-day sequence (manual trigger v1)` |
| **Acceptance** | Copy from `campaigns/launch-v1-four-day-sequence.md`; Day 1 not sent until founder trigger |
| **Depends** | `LAUNCH_VOCAL_STEMS_URL`, Resend env |

## WO-PM-ADD-001 — Elite copy subagents (founder tonight)

| Field | Value |
|-------|--------|
| **Title** | `[PM] Register Marketing copy subagents: email-only + DR-only` |
| **Acceptance** | Agent prompts live in repo; VARA roster updated; optional Claude Project per agent |
| **Owner** | **Founder** (create agents) + Cursor (merge prompts to repo) |
| **Note** | See `disklordz/marketing/TODO.md` |
