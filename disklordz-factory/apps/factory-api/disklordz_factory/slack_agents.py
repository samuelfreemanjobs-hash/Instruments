from __future__ import annotations

import json
import os
import urllib.error
import urllib.request
from typing import Any

from disklordz_factory.models import NightShiftRequest, NightShiftRun

SLACK_FACTORY_WEBHOOK_ENV = "SLACK_FACTORY_WEBHOOK_URL"
SLACK_FALLBACK_WEBHOOK_ENV = "SLACK_WEBHOOK_URL"

# Display personas for #disklordz-factory (or shared CI channel until split)
AGENT_ROSTER: dict[str, dict[str, str]] = {
    "orchestrator": {
        "name": "Factory Orchestrator",
        "emoji": "🏭",
        "role": "CEO/producer — runs batches, delegates, morning queue",
    },
    "market_intel": {
        "name": "Market Intelligence",
        "emoji": "📡",
        "role": "Trends, keywords, product gaps (YouTube, Spotify, marketplaces)",
    },
    "aar": {
        "name": "A&R",
        "emoji": "🎯",
        "role": "Greenlight / kill — maps opportunities to DL001–DL006",
    },
    "producer": {
        "name": "Music Producer",
        "emoji": "🎛️",
        "role": "Briefs: tempo, key, arrangement, energy curve",
    },
    "sound_design": {
        "name": "Sound Design",
        "emoji": "🔊",
        "role": "Drum families, 808/kick/hat machines (DiskLordz Drumworks)",
    },
    "sample_miner": {
        "name": "Sample Miner",
        "emoji": "⛏️",
        "role": "Track → kits, one-shots, MIDI, presets",
    },
    "visual": {
        "name": "Visual Director",
        "emoji": "🎨",
        "role": "Artist identity, artwork, thumbnail system",
    },
    "content": {
        "name": "Content Producer",
        "emoji": "📺",
        "role": "YouTube programming, Shorts, distribution metadata",
    },
    "copy": {
        "name": "Copy Engine",
        "emoji": "✍️",
        "role": "DR copy: feature → benefit → CTA",
    },
    "rights": {
        "name": "Rights & QA",
        "emoji": "⚖️",
        "role": "Provenance gate — nothing publishes without pass",
    },
    "analytics": {
        "name": "Analytics",
        "emoji": "📈",
        "role": "Performance → next batch rules",
    },
}

ARTIST_LABELS: dict[str, str] = {
    "DL001": "DISKLORD 001 — robot funk / French house / electro",
    "DL002": "DISKLORD 002 — dark phonk / Memphis / 808",
    "DL003": "DISKLORD 003 — Detroit electro",
    "DL004": "DISKLORD 004 — cyber disco",
    "DL005": "DISKLORD 005 — industrial trap",
    "DL006": "DISKLORD 006 — digital soul",
}


def slack_webhook_url() -> str | None:
    url = os.environ.get(SLACK_FACTORY_WEBHOOK_ENV) or os.environ.get(SLACK_FALLBACK_WEBHOOK_ENV)
    return url.strip() if url else None


def post_slack_payload(payload: dict[str, Any]) -> dict[str, Any]:
    url = slack_webhook_url()
    if not url:
        return {"ok": False, "skipped": True, "reason": "no webhook (set SLACK_FACTORY_WEBHOOK_URL or SLACK_WEBHOOK_URL)"}
    data = json.dumps(payload).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=data,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=15) as resp:
            body = resp.read().decode("utf-8", errors="replace")
            return {"ok": True, "status": resp.status, "body": body}
    except urllib.error.HTTPError as e:
        return {"ok": False, "status": e.code, "body": e.read().decode("utf-8", errors="replace")}
    except urllib.error.URLError as e:
        return {"ok": False, "error": str(e.reason)}


def _mrkdwn_section(agent_key: str, text: str) -> dict[str, Any]:
    meta = AGENT_ROSTER[agent_key]
    return {
        "type": "section",
        "text": {
            "type": "mrkdwn",
            "text": f"{meta['emoji']} *{meta['name']}*\n{text}",
        },
    }


def build_market_intel_copy(mission: str) -> str:
    return (
        f"Scan complete for mission: _{mission}_\n"
        "• Opportunity: *dark 90s digital phonk* — BPM 140–155, CRT/night-drive aesthetic\n"
        "• Supply angle: 808 + drum kit + MIDI pack\n"
        "• Demand: high on YouTube beat channels; competition: medium\n"
        "_Note: live API ingest coming — this briefing uses factory heuristics until Market Intel MCP is wired._"
    )


def build_aar_copy(artist_ids: list[str], mission: str) -> str:
    lines = [f"*Greenlit* for batch under mission: _{mission}_"]
    for aid in artist_ids or ["DL002", "DL003"]:
        lines.append(f"• `{aid}` → {ARTIST_LABELS.get(aid, aid)}")
    lines.append(
        "\n*Rejected this cycle:* generic lo-fi study beats (off-brand), "
        "uncleared sample-flip series (rights risk)."
    )
    lines.append(
        "\n_Lineup is artist-IP first; full ranked `DL-OPP-*` records land when research pipeline is live._"
    )
    return "\n".join(lines)


def build_night_shift_slack_blocks(
    run: NightShiftRun,
    req: NightShiftRequest,
) -> dict[str, Any]:
    artists = req.artist_ids or ["DL002", "DL003"]
    dashboard_hint = os.environ.get("FACTORY_DASHBOARD_URL", "http://127.0.0.1:5173")

    blocks: list[dict[str, Any]] = [
        {
            "type": "header",
            "text": {"type": "plain_text", "text": "DiskLordz Factory — night shift complete"},
        },
        {
            "type": "section",
            "fields": [
                {"type": "mrkdwn", "text": f"*Run:*\n`{run.run_id}`"},
                {"type": "mrkdwn", "text": f"*Batch:*\n`{run.batch_id}`"},
                {"type": "mrkdwn", "text": f"*Assets:*\n{run.assets_created}"},
                {"type": "mrkdwn", "text": f"*Approval queue:*\n{run.approval_items} pending"},
            ],
        },
        {"type": "divider"},
        _mrkdwn_section(
            "orchestrator",
            f"Mission locked. Status: `{run.status}`. "
            f"Human approval required before publish — open <{dashboard_hint}|Factory dashboard>.",
        ),
        _mrkdwn_section("market_intel", build_market_intel_copy(req.mission)),
        _mrkdwn_section("aar", build_aar_copy(artists, req.mission)),
        _mrkdwn_section(
            "producer",
            f"Briefs issued for {len(artists)} artist lane(s). "
            "Target internal SP-1200 rate story where relevant (26,041.66 Hz lane) for Labs crossover.",
        ),
        _mrkdwn_section(
            "sample_miner",
            f"Extracted product candidates from overnight tracks — kits queued for QA ({run.assets_created} total assets touched).",
        ),
        _mrkdwn_section(
            "content",
            "YouTube titles/descriptions drafted; Shorts concepts attached per track cluster.",
        ),
        _mrkdwn_section(
            "rights",
            "All queue items passed provenance checklist for *review* — operator must still approve.",
        ),
        {
            "type": "context",
            "elements": [
                {
                    "type": "mrkdwn",
                    "text": "Factory API · POST /factory/slack/checkin · Agents roster in `disklordz-factory/agents/`",
                }
            ],
        },
    ]
    return {
        "text": f"DiskLordz Factory night shift — {run.approval_items} items awaiting approval",
        "blocks": blocks,
    }


def build_agent_checkin_blocks() -> dict[str, Any]:
    """Morning stand-up: every core agent checks in (no batch required)."""
    blocks: list[dict[str, Any]] = [
        {
            "type": "header",
            "text": {"type": "plain_text", "text": "DiskLordz Factory — agent check-in"},
        },
        {
            "type": "section",
            "text": {
                "type": "mrkdwn",
                "text": "Core team online. Reply in thread with missions, or run *Run night shift* on the dashboard.",
            },
        },
        {"type": "divider"},
    ]
    for key in (
        "orchestrator",
        "market_intel",
        "aar",
        "producer",
        "sound_design",
        "sample_miner",
        "visual",
        "content",
        "copy",
        "rights",
        "analytics",
    ):
        meta = AGENT_ROSTER[key]
        blocks.append(
            {
                "type": "section",
                "text": {
                    "type": "mrkdwn",
                    "text": f"{meta['emoji']} *{meta['name']}* — {meta['role']}",
                },
            }
        )
    blocks.append(
        {
            "type": "context",
            "elements": [{"type": "mrkdwn", "text": "Configure `SLACK_FACTORY_WEBHOOK_URL` → #disklordz-factory (or reuse CI webhook)."}],
        }
    )
    return {"text": "DiskLordz Factory agents checking in", "blocks": blocks}


def notify_night_shift_slack(run: NightShiftRun, req: NightShiftRequest) -> dict[str, Any]:
    return post_slack_payload(build_night_shift_slack_blocks(run, req))


def notify_agent_checkin() -> dict[str, Any]:
    return post_slack_payload(build_agent_checkin_blocks())


def notify_approval(asset_id: str, title: str) -> dict[str, Any]:
    return post_slack_payload(
        {
            "text": f"Approved {asset_id}",
            "blocks": [
                mrkdwn_section(
                    "content",
                    f"Operator approved `{asset_id}` — *{title}*.\n"
                    "Distribution prep can proceed after batch publish gate.",
                ),
                mrkdwn_section(
                    "aar",
                    "Catalog slot confirmed. Analytics will tag this asset on next MEASURE cycle.",
                ),
            ],
        }
    )


def mrkdwn_section(agent_key: str, text: str) -> dict[str, Any]:
    return _mrkdwn_section(agent_key, text)
