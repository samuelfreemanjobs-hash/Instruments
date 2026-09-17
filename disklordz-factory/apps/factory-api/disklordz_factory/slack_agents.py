from __future__ import annotations

import json
import os
import urllib.error
import urllib.request
from typing import Any

from disklordz_factory.models import NightShiftRequest, NightShiftRun
from disklordz_factory.research import ResearchStatus, research_store

SLACK_FACTORY_WEBHOOK_ENV = "SLACK_FACTORY_WEBHOOK_URL"
SLACK_FALLBACK_WEBHOOK_ENV = "SLACK_WEBHOOK_URL"

AGENT_ROSTER: dict[str, dict[str, str]] = {
    "orchestrator": {
        "name": "Factory Orchestrator",
        "emoji": "🏭",
        "role": "CEO/producer — runs batches, delegates, morning queue",
    },
    "market_intel": {
        "name": "Market Intelligence",
        "emoji": "📡",
        "role": "Sourced trends only — posts `DL-OPP-*` with citations",
    },
    "aar": {
        "name": "A&R",
        "emoji": "🎯",
        "role": "Greenlight / kill — *only* after sourced research",
    },
    "producer": {
        "name": "Music Producer",
        "emoji": "🎛️",
        "role": "Briefs after A&R greenlight",
    },
    "sound_design": {"name": "Sound Design", "emoji": "🔊", "role": "Sound families"},
    "sample_miner": {"name": "Sample Miner", "emoji": "⛏️", "role": "Track → products"},
    "visual": {"name": "Visual Director", "emoji": "🎨", "role": "Identity & artwork"},
    "content": {"name": "Content Producer", "emoji": "📺", "role": "Programming & metadata"},
    "copy": {"name": "Copy Engine", "emoji": "✍️", "role": "Sales copy"},
    "rights": {"name": "Rights & QA", "emoji": "⚖️", "role": "Provenance gate"},
    "analytics": {"name": "Analytics", "emoji": "📈", "role": "Measure → learn"},
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


def mrkdwn_section(agent_key: str, text: str) -> dict[str, Any]:
    meta = AGENT_ROSTER[agent_key]
    return {
        "type": "section",
        "text": {"type": "mrkdwn", "text": f"{meta['emoji']} *{meta['name']}*\n{text}"},
    }


def build_market_intel_slack(status: ResearchStatus) -> str:
    opps = research_store.list_opportunities()
    if not opps:
        return (
            "*No market claims.* Waiting for sourced opportunities.\n"
            "Submit: `POST /research/opportunities` with `sources[]` (URLs or citations).\n"
            "Market Intel does not invent genres, demand, or competition."
        )
    lines = [f"*{len(opps)}* sourced opportunity record(s):"]
    for o in opps[:5]:
        src = ", ".join(f"<{s}|source>" if s.startswith("http") else f"`{s}`" for s in o.sources[:3])
        lines.append(
            f"• `{o.opportunity_id}` *{o.title}* ({o.genre}) — score `{o.score or 'unset'}` — {src}"
        )
    return "\n".join(lines)


def build_aar_slack(status: ResearchStatus) -> str:
    if status.block_reason and not status.greenlit_artist_ids:
        return (
            f"*No artist lineup announced.*\n{status.block_reason}\n"
            "A&R stays silent until `POST /research/aar-decisions` on a sourced `DL-OPP-*`."
        )
    lines = ["*Greenlit artists (research-backed only):*"]
    for d in research_store.decisions:
        if d.decision != "greenlight":
            continue
        lines.append(f"• `{d.artist_id}` on `{d.opportunity_id}` — _{d.rationale}_")
    rejects = [d for d in research_store.decisions if d.decision == "reject"]
    if rejects:
        lines.append("\n*Rejected:*")
        for d in rejects[:5]:
            lines.append(f"• `{d.opportunity_id}` — _{d.rationale}_")
    return "\n".join(lines)


def build_night_shift_slack_blocks(
    run: NightShiftRun,
    req: NightShiftRequest,
    status: ResearchStatus | None = None,
) -> dict[str, Any]:
    status = status or research_store.status()
    dashboard_hint = os.environ.get("FACTORY_DASHBOARD_URL", "http://127.0.0.1:5173")

    blocks: list[dict[str, Any]] = [
        {
            "type": "header",
            "text": {"type": "plain_text", "text": "DiskLordz Factory — night shift (ops)"},
        },
        {
            "type": "section",
            "fields": [
                {"type": "mrkdwn", "text": f"*Run:*\n`{run.run_id}`"},
                {"type": "mrkdwn", "text": f"*Batch:*\n`{run.batch_id}`"},
                {"type": "mrkdwn", "text": f"*Technical assets:*\n{run.assets_created}"},
                {"type": "mrkdwn", "text": f"*QA queue:*\n{run.approval_items} pending"},
            ],
        },
        {"type": "divider"},
        mrkdwn_section(
            "orchestrator",
            f"Mission: _{req.mission}_ · `{run.status}`\n"
            f"Research gate: {'*OPEN*' if status.ready_for_production else '*CLOSED*'}\n"
            f"Dashboard: <{dashboard_hint}|Factory>",
        ),
        mrkdwn_section("market_intel", build_market_intel_slack(status)),
        mrkdwn_section("aar", build_aar_slack(status)),
    ]

    if status.ready_for_production:
        blocks.extend(
            [
                mrkdwn_section(
                    "producer",
                    "Briefs may proceed for greenlit `artist_id` lanes only.",
                ),
                mrkdwn_section(
                    "rights",
                    f"{run.approval_items} item(s) in QA queue — operator approval still required.",
                ),
            ]
        )
    else:
        blocks.append(
            mrkdwn_section(
                "producer",
                "*Held* — no production briefs naming artists or market positioning until research + A&R greenlight.",
            )
        )

    blocks.append(
        {
            "type": "context",
            "elements": [
                {
                    "type": "mrkdwn",
                    "text": "Policy: research before anyone speaks · `docs/FACTORY_RESEARCH.md`",
                }
            ],
        }
    )
    return {
        "text": "DiskLordz Factory night shift — research-gated update",
        "blocks": blocks,
    }


def build_agent_checkin_blocks() -> dict[str, Any]:
    status = research_store.status()
    blocks: list[dict[str, Any]] = [
        {
            "type": "header",
            "text": {"type": "plain_text", "text": "DiskLordz Factory — systems online"},
        },
        {
            "type": "section",
            "text": {
                "type": "mrkdwn",
                "text": (
                    "Agents are *on standby*. Market Intel and A&R will *not* name artists or "
                    "market wins until sourced `DL-OPP-*` records exist.\n"
                    f"Research gate: {'OPEN' if status.ready_for_production else 'CLOSED'}"
                    + (f" — {status.block_reason}" if status.block_reason else "")
                ),
            },
        },
        {"type": "divider"},
        mrkdwn_section("market_intel", build_market_intel_slack(status)),
        mrkdwn_section("aar", build_aar_slack(status)),
        {
            "type": "context",
            "elements": [
                {
                    "type": "mrkdwn",
                    "text": "Roster: orchestrator, producer, sound, miner, visual, content, copy, rights, analytics — speak on ops/events only until research clears.",
                }
            ],
        },
    ]
    return {"text": "DiskLordz Factory — research-gated check-in", "blocks": blocks}


def notify_night_shift_slack(run: NightShiftRun, req: NightShiftRequest) -> dict[str, Any]:
    return post_slack_payload(build_night_shift_slack_blocks(run, req))


def notify_agent_checkin() -> dict[str, Any]:
    return post_slack_payload(build_agent_checkin_blocks())


def notify_approval(asset_id: str, title: str) -> dict[str, Any]:
    return post_slack_payload(
        {
            "text": f"Operator approved {asset_id}",
            "blocks": [
                mrkdwn_section(
                    "orchestrator",
                    f"Human approved `{asset_id}` — *{title}* (QA queue).",
                ),
                mrkdwn_section(
                    "rights",
                    "Publish externally only after rights gate + your explicit release plan.",
                ),
            ],
        }
    )


def notify_research_needed(mission: str) -> dict[str, Any]:
    return post_slack_payload(
        {
            "text": "DiskLordz Factory — research required",
            "blocks": [
                {
                    "type": "header",
                    "text": {"type": "plain_text", "text": "Research required before A&R speaks"},
                },
                mrkdwn_section(
                    "market_intel",
                    f"Mission queued: _{mission}_\n"
                    "Waiting for sourced opportunities (`POST /research/opportunities`).",
                ),
                mrkdwn_section(
                    "aar",
                    "No lineup will be posted until decisions reference a `DL-OPP-*` id.",
                ),
            ],
        }
    )
