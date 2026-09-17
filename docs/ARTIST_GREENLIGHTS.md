# Artist greenlights (HQ locked)

DiskLordz Factory treats **exactly four artist lanes** as greenlit for production, night shift, and Slack A&R copy. Each greenlight is tied to a sourced `DL-OPP-*` and a YouTube network brand.

**Status:** HQ locked — do not add fifth lanes or cross-assign batches without new research + explicit A&R decision.

| Artist | YouTube brand | Channel | Role |
|--------|---------------|---------|------|
| **DL001** | YT-DL-001 | Boulevard 86 | French touch / vintage disco-funk → robot funk |
| **DL002** | YT-DL-002 | Midnight Circuit | Drift phonk · wave phonk (fast) |
| **DL006** | YT-DL-003 | Disklordz | DJ Screw · 90s phonk · lo-fi (slow) |
| **DL004** | YT-DL-004 | Terminal Mirage | Cyber funk + vapor aesthetic (coding) |

**Not in network roster:** DL003, DL005 (legacy IDs in catalog SQL only).

## Evidence & ingest

Canonical payloads: [`disklordz-factory/database/youtube_research_seed.json`](../disklordz-factory/database/youtube_research_seed.json) (`hq_official_greenlight: true`).

- **On Factory API startup**, research is bootstrapped from that seed when the in-memory store is empty.
- **Re-ingest manually:** `./disklordz-factory/scripts/ingest-youtube-network-research.sh`
- **Check:** `GET /research/status` and `GET /research/greenlights`

Replace `internal: REPLACE` source placeholders with your Slack/Airtable backup when you harden citations for external claims.

## Agent rules

- Market Intel may cite only recorded `DL-OPP-*` rows.
- A&R may name **only** the four artists above when `ready_for_production` is true.
- Producer / Sample Miner assign assets to `artist_id` in this set unless HQ passes an override on a single batch.

## Related

- [FACTORY_RESEARCH.md](FACTORY_RESEARCH.md)
- [FACTORY_VINTAGE_COLLECTIVE.md](FACTORY_VINTAGE_COLLECTIVE.md)
- [DISKLORDZ_YOUTUBE_NETWORK.md](DISKLORDZ_YOUTUBE_NETWORK.md)
