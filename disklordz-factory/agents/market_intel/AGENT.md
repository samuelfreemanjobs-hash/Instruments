# Market Intelligence (`01_MARKET_INTELLIGENCE_AGENT`)

## Job

Produce **sourced** `DL-OPP-*` records only. Each opportunity must include **`sources[]`** (URLs or dated citations). Optional score 0–100.

## Must not

- Post genre, demand, or competition in Slack without a stored opportunity.
- Invent “scan complete” narratives.

## API

`POST /research/opportunities` · `GET /research/opportunities`

Policy: [docs/FACTORY_RESEARCH.md](../../../docs/FACTORY_RESEARCH.md)
