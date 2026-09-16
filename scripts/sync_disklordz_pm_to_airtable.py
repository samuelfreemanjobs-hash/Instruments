#!/usr/bin/env python3
"""Idempotent sync: Disklordz factory project + PM Agent rebrand in Airtable."""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request
from datetime import datetime, timezone

BASE_ID = "appUuhVQHAOv31wJ1"
TABLE = "Projects"
PM_AGENT_RECORD_ID = "rect3SJqsuOzSal5A"
FACTORY_NAME = "Disklordz — phonk factory & GRAVEYARD_SHIFT v1"
ACTIVEPIECES_NAME = "Disklordz — Activepieces automation (self-host)"

ACTIVEPIECES_NOTES = """Checklist (studio machine):
1. Pull/merge Instruments PR #12 (branch cursor/disklordz-blueprint-tools-2702)
2. cd infra/activepieces && cp .env.example .env — set DISKLORDZ_AP_ADMIN_EMAIL + DISKLORDZ_AP_ADMIN_PASSWORD (optional DISKLORDZ_SLACK_WEBHOOK)
3. chmod +x automate.sh && ./automate.sh (Docker required)
4. Confirm infra/activepieces/.disklordz-webhook-url; test tools/render_kit.py locally
5. GitHub secret AIRTABLE_API_KEY → run workflow Sync Disklordz PM to Airtable
6. GitHub (optional): DISKLORDZ_ACTIVEPIECES_WEBHOOK for workflow Disklordz factory render
7. Activepieces UI: Airtable connection for base appUuhVQHAOv31wJ1 (future store flows)
Docs: infra/activepieces/README.md"""


def api(method: str, path: str, body: dict | None = None) -> dict:
    key = os.environ.get("AIRTABLE_API_KEY") or os.environ.get("AIRTABLE_TOKEN")
    if not key:
        sys.exit("Set AIRTABLE_API_KEY or AIRTABLE_TOKEN")

    url = f"https://api.airtable.com/v0/{BASE_ID}/{path}"
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(
        url,
        data=data,
        method=method,
        headers={"Authorization": f"Bearer {key}", "Content-Type": "application/json"},
    )
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            return json.loads(resp.read().decode())
    except urllib.error.HTTPError as e:
        sys.exit(f"Airtable HTTP {e.code}: {e.read().decode()[:800]}")


def now_iso() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S.000Z")


def find_project_by_name(name: str) -> str | None:
    import urllib.parse

    formula = urllib.parse.quote(f"{{Project Name}}='{name}'")
    key = os.environ.get("AIRTABLE_API_KEY") or os.environ.get("AIRTABLE_TOKEN")
    url = f"https://api.airtable.com/v0/{BASE_ID}/{TABLE}?filterByFormula={formula}&maxRecords=1"
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {key}"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        data = json.loads(resp.read().decode())
    recs = data.get("records", [])
    return recs[0]["id"] if recs else None


def upsert_project(name: str, fields: dict) -> None:
    rid = find_project_by_name(name)
    if rid:
        api("PATCH", f"{TABLE}/{rid}", {"fields": fields})
        print(f"Updated project {name} ({rid})")
    else:
        out = api("POST", TABLE, {"fields": fields})
        print(f"Created project {name} ({out['id']})")


def main() -> int:
    ts = now_iso()
    factory_fields = {
        "Project Name": FACTORY_NAME,
        "Status": "ACTIVE",
        "Urgency": "HIGH",
        "Current Phase": "PHASE-1",
        "% Complete": 25,
        "Health": "GREEN",
        "Last Updated": ts,
        "Next Action": "Listen to GRAVEYARD_SHIFT batch in DAW; sort 26 WAVs into 01_Kicks/02_808s; tweak synthesis.py if needed",
        "Scope": "Active math factory (tools/render_kit.py), SQLite sample_dna, kit v1 GRAVEYARD_SHIFT. Master plan: docs/DISKLORDZ_FACTORY_PLAN.md (Instruments repo).",
        "Proposed Outcome": "Ship GRAVEYARD_SHIFT drum kit ZIP with LICENSE + demo track.",
        "For Who": "Phonk/trap producers — SP-1200 style 808 knock",
        "Opportunity Value": "First Disklordz commercial SKU + VSTi path",
        "Stack": "Python factory\ntools/disklordz\nSQLite\nJUCE 7",
        "Notes": "Factory kickoff — starter + 20 grid 808s in tools/output/samples.",
    }

    upsert_project(FACTORY_NAME, factory_fields)

    activepieces_fields = {
        "Project Name": ACTIVEPIECES_NAME,
        "Status": "ACTIVE",
        "Urgency": "HIGH",
        "Current Phase": "PHASE-1",
        "% Complete": 10,
        "Health": "GREEN",
        "Last Updated": ts,
        "Next Action": "Set DISKLORDZ_AP_ADMIN_* in infra/activepieces/.env and run ./automate.sh on studio machine",
        "Scope": "Self-hosted Activepieces for factory webhooks. Repo: infra/activepieces/, scripts/provision_disklordz_activepieces.py. PM stays Cursor Airtable MCP.",
        "Proposed Outcome": "automate.sh brings up Docker + provisions Disklordz factory webhook; render_kit notifies on batch complete.",
        "For Who": "Sam / Disklordz studio",
        "Opportunity Value": "Event glue without Zapier task burn",
        "Stack": "Docker Compose\nActivepieces\nGitHub Actions (optional)",
        "API Keys Needed": "DISKLORDZ_AP_ADMIN (local .env)\nGitHub: AIRTABLE_API_KEY, DISKLORDZ_ACTIVEPIECES_WEBHOOK (optional)",
        "Notes": ACTIVEPIECES_NOTES,
    }
    upsert_project(ACTIVEPIECES_NAME, activepieces_fields)

    pm_fields = {
        "Project Name": "Disklordz — Audio PM Agent",
        "For Who": "Sam / Disklordz — audio product operations",
        "Scope": "Airtable PM for Disklordz: factory, packs, JUCE plugins. Prompt: docs/pm-agent/DISKLORDZ_AUDIO_PM_AGENT.md",
        "% Complete": 85,
        "Health": "GREEN",
        "Last Updated": ts,
        "Next Action": "Run PM BRIEFING weekly; keep Disklordz factory current",
        "Notes": "Rebranded from Freeman Intelligence PM Agent. MCP: docs/pm-agent/CLAUDE_SETUP.md",
        "Current Phase": "PHASE-3",
        "Status": "ACTIVE",
    }
    api("PATCH", f"{TABLE}/{PM_AGENT_RECORD_ID}", {"fields": pm_fields})
    print(f"Updated PM Agent {PM_AGENT_RECORD_ID}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
