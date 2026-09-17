from __future__ import annotations

from functools import lru_cache
from pathlib import Path
from typing import Any

from disklordz_factory.collective import list_collective_artists

_FACTORY_ROOT = Path(__file__).resolve().parents[3]
_ARTISTS_DIR = _FACTORY_ROOT / "agents" / "artists"
_PROMPTS_DIR = _FACTORY_ROOT / "prompts" / "artists"

GREENLIT_ARTIST_IDS = ("DL001", "DL002", "DL006", "DL004")

def openclaw_agent_id(artist_id: str) -> str:
    return f"DISKLORDZ_ARTIST_{artist_id}"


def _read_text(path: Path) -> str | None:
    if not path.is_file():
        return None
    return path.read_text(encoding="utf-8")


@lru_cache(maxsize=16)
def artist_agent_contract(artist_id: str) -> str | None:
    return _read_text(_ARTISTS_DIR / artist_id / "AGENT.md")


@lru_cache(maxsize=16)
def artist_system_prompt(artist_id: str) -> str | None:
    return _read_text(_PROMPTS_DIR / f"{artist_id}.md")


def build_artist_agent_profile(artist_id: str) -> dict[str, Any] | None:
    if artist_id not in GREENLIT_ARTIST_IDS:
        return None
    roster = {a["artist_id"]: a for a in list_collective_artists()}
    base = roster.get(artist_id)
    if not base:
        return None
    return {
        **base,
        "openclaw_agent_id": openclaw_agent_id(artist_id),
        "hq_greenlit": True,
        "agent_contract_path": f"agents/artists/{artist_id}/AGENT.md",
        "system_prompt_path": f"prompts/artists/{artist_id}.md",
        "agent_contract": artist_agent_contract(artist_id),
        "system_prompt": artist_system_prompt(artist_id),
    }


def list_artist_agent_profiles() -> list[dict[str, Any]]:
    out: list[dict[str, Any]] = []
    for aid in GREENLIT_ARTIST_IDS:
        profile = build_artist_agent_profile(aid)
        if profile:
            out.append(profile)
    return out
