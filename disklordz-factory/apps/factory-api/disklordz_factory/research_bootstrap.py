from __future__ import annotations

import json
import logging
from pathlib import Path
from typing import Any

from disklordz_factory.research import AARDecisionIn, MarketOpportunityIn, research_store

logger = logging.getLogger(__name__)

_SEED_PATH = (
    Path(__file__).resolve().parents[3] / "database" / "youtube_research_seed.json"
)


def _load_seed() -> dict[str, Any]:
    with _SEED_PATH.open(encoding="utf-8") as f:
        return json.load(f)


def official_greenlit_artist_ids() -> list[str]:
    seed = _load_seed()
    ids = seed.get("greenlit_artist_ids")
    if isinstance(ids, list) and ids:
        return list(ids)
    return ["DL001", "DL002", "DL006", "DL004"]


def bootstrap_research_if_empty() -> bool:
    """Load HQ official DL-OPP + A&R greenlights when store has no opportunities."""
    if research_store.opportunities:
        return False
    if not _SEED_PATH.is_file():
        logger.warning("research bootstrap skipped: missing %s", _SEED_PATH)
        return False

    seed = _load_seed()
    if not seed.get("hq_official_greenlight"):
        logger.info("research bootstrap skipped: hq_official_greenlight not set in seed")
        return False

    skip_keys = {"youtube_brand_id", "artist_id", "aar"}
    for opp in seed.get("opportunities", []):
        payload = {k: v for k, v in opp.items() if k not in skip_keys and v is not None}
        created = research_store.add_opportunity(MarketOpportunityIn(**payload))
        aar = opp.get("aar") or {}
        artist_id = opp.get("artist_id")
        if aar.get("decision") == "greenlight" and artist_id:
            research_store.record_aar_decision(
                AARDecisionIn(
                    opportunity_id=created.opportunity_id,
                    artist_id=artist_id,
                    decision="greenlight",
                    rationale=aar["rationale"],
                )
            )

    logger.info(
        "research bootstrap loaded %s opportunities, greenlit %s",
        len(research_store.opportunities),
        research_store.greenlit_artist_ids(),
    )
    return True


def greenlights_summary() -> dict[str, Any]:
    seed = _load_seed()
    rows = []
    for opp in seed.get("opportunities", []):
        artist = opp.get("artist_id")
        stored = None
        for o in research_store.list_opportunities():
            if o.title == opp.get("title"):
                stored = o.opportunity_id
                break
        rows.append(
            {
                "artist_id": artist,
                "youtube_brand_id": opp.get("youtube_brand_id"),
                "title": opp.get("title"),
                "opportunity_id_in_store": stored,
            }
        )
    return {
        "hq_official_greenlight": bool(seed.get("hq_official_greenlight")),
        "greenlit_artist_ids": official_greenlit_artist_ids(),
        "artists": rows,
        "research_status": research_store.status().model_dump(),
    }
