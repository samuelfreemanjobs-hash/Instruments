#!/usr/bin/env python3
"""Ingest four YouTube brand DL-OPP records + optional A&R greenlights into Factory API."""

from __future__ import annotations

import argparse
import json
import sys
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SEED = ROOT / "database" / "youtube_research_seed.json"


def _http_json(method: str, url: str, body: dict | None = None) -> Any:
    data = None if body is None else json.dumps(body).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=data,
        method=method,
        headers={"Content-Type": "application/json", "Accept": "application/json"},
    )
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            return json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as e:
        detail = e.read().decode("utf-8", errors="replace")
        raise SystemExit(f"HTTP {e.code} {url}\n{detail}") from e


def load_seed(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def apply_sources_overlay(
    opportunities: list[dict[str, Any]], overlay: dict[str, list[str]]
) -> None:
    for opp in opportunities:
        brand = opp.get("youtube_brand_id")
        if brand and brand in overlay:
            opp["sources"] = overlay[brand]


def opportunity_payload(opp: dict[str, Any]) -> dict[str, Any]:
    skip = {"youtube_brand_id", "artist_id", "aar"}
    return {k: v for k, v in opp.items() if k not in skip and v is not None}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--base-url",
        default="http://127.0.0.1:8787",
        help="Factory API base URL",
    )
    parser.add_argument(
        "--seed",
        type=Path,
        default=DEFAULT_SEED,
        help="youtube_research_seed.json path",
    )
    parser.add_argument(
        "--sources-overlay",
        type=Path,
        default=None,
        help="JSON map YT-DL-00X -> sources[] (replaces seed sources for that brand)",
    )
    parser.add_argument(
        "--no-greenlight",
        action="store_true",
        help="Only POST opportunities, skip A&R decisions",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print actions without calling API",
    )
    args = parser.parse_args()

    seed = load_seed(args.seed)
    opportunities = list(seed.get("opportunities", []))
    if len(opportunities) != 4:
        print(f"warning: expected 4 opportunities, got {len(opportunities)}", file=sys.stderr)

    if args.sources_overlay:
        overlay = load_seed(args.sources_overlay)
        if "opportunities" in overlay:
            raise SystemExit("sources overlay must be a map YT-DL-00X -> sources[], not full seed")
        apply_sources_overlay(opportunities, overlay)

    base = args.base_url.rstrip("/")
    results: list[dict[str, Any]] = []

    for opp in opportunities:
        brand = opp.get("youtube_brand_id", "?")
        artist = opp.get("artist_id", "?")
        payload = opportunity_payload(opp)
        if args.dry_run:
            print(f"[dry-run] POST /research/opportunities  brand={brand} artist={artist}")
            print(json.dumps(payload, indent=2)[:500] + "...")
            results.append({"brand_id": brand, "artist_id": artist, "opportunity_id": "DRY-RUN"})
            continue

        created = _http_json("POST", f"{base}/research/opportunities", payload)
        oid = created["opportunity_id"]
        print(f"recorded {oid}  {brand}  ->  {artist}  ({payload.get('title', '')[:50]})")
        entry = {
            "youtube_brand_id": brand,
            "artist_id": artist,
            "opportunity_id": oid,
        }

        aar = opp.get("aar") or {}
        if not args.no_greenlight and aar.get("decision") == "greenlight":
            decision_body = {
                "opportunity_id": oid,
                "artist_id": artist,
                "decision": "greenlight",
                "rationale": aar["rationale"],
            }
            _http_json("POST", f"{base}/research/aar-decisions", decision_body)
            print(f"  greenlight {artist} on {oid}")

        results.append(entry)

    if args.dry_run:
        print("\n[dry-run] skip GET /research/status")
        return 0

    status = _http_json("GET", f"{base}/research/status")
    print("\n--- research status ---")
    print(json.dumps(status, indent=2))
    if status.get("ready_for_production"):
        print("\nReady: run night shift or OpenClaw vintage collective batch.")
    else:
        print(f"\nBlocked: {status.get('block_reason')}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
