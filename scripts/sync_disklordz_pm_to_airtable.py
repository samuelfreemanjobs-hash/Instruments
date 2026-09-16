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


def find_factory_record() -> str | None:
    # filterByFormula on primary field
    import urllib.parse

    formula = urllib.parse.quote(f"{{Project Name}}='{FACTORY_NAME}'")
    key = os.environ.get("AIRTABLE_API_KEY") or os.environ.get("AIRTABLE_TOKEN")
    url = f"https://api.airtable.com/v0/{BASE_ID}/{TABLE}?filterByFormula={formula}&maxRecords=1"
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {key}"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        data = json.loads(resp.read().decode())
    recs = data.get("records", [])
    return recs[0]["id"] if recs else None


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
        "Scope": "Active math factory (tools/render_kit.py), SQLite sample_dna, kit v1 GRAVEYARD_SHIFT. Master plan: docs/DISKLOrdZ_FACTORY_PLAN.md (Instruments repo).",
        "Proposed Outcome": "Ship GRAVEYARD_SHIFT drum kit ZIP with LICENSE + demo track.",
        "For Who": "Phonk/trap producers — SP-1200 style 808 knock",
        "Opportunity Value": "First Disklordz commercial SKU + VSTi path",
        "Stack": "Python factory\ntools/disklordz\nSQLite\nJUCE 7",
        "Notes": "Factory kickoff — starter + 20 grid 808s in tools/output/samples.",
    }

    rid = find_factory_record()
    if rid:
        api("PATCH", f"{TABLE}/{rid}", {"fields": factory_fields})
        print(f"Updated factory project {rid}")
    else:
        out = api("POST", TABLE, {"fields": factory_fields})
        print(f"Created factory project {out['id']}")

    pm_fields = {
        "Project Name": "Disklordz — Audio PM Agent",
        "For Who": "Sam / Disklordz — audio product operations",
        "Scope": "Airtable PM for Disklordz: factory, packs, JUCE plugins. Prompt: docs/pm-agent/DISKLOrdZ_AUDIO_PM_AGENT.md",
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
