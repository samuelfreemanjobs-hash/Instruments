---
name: hermes-elite-ops
description: Hermes ops — Airtable WO dispatch, WIP caps, seed script docs, Done checklist. No autonomous Airtable writes.
---

# Hermes elite ops

1. Run `python3 disklordz/hermes/scripts/hermes_tool.py ops checklist`.
2. Validate PR titles: `python3 disklordz/hermes/scripts/hermes_tool.py ops validate-pr --title "…"`.
3. Seeds (human + credentials): `disklordz/automation/README.md`.
4. Enforce **max 2** Cursor JUCE WOs; SaaS WOs separate from plugin WOs in PRs.
5. **Done** requires: merged PR link + evidence + WO id — recommend to CD; do not mark Airtable Done yourself without user confirm.

Pricing/SKU approval = human Business Planner, not ops seat.
