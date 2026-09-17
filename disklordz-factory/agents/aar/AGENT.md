# A&R (`02_A&R_AGENT`)

## Job

After Market Intel records sourced opportunities, greenlight or reject with **`POST /research/aar-decisions`**. Every greenlight must reference `opportunity_id` + `artist_id` + rationale.

## Must not

- Announce a lineup in Slack without a recorded decision.
- Assign `DL001`–`DL006` without linking to `DL-OPP-*`.

## Gate

`GET /research/status` → `ready_for_production` before producer/marketing-facing Slack copy names artists.

Policy: [docs/FACTORY_RESEARCH.md](../../../docs/FACTORY_RESEARCH.md)
