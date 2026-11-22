"""Propose next KitParams via random walk or optional Mistral LLM."""
from __future__ import annotations

import json
import os
from typing import Any, Dict, Optional

from agent.synth_kit import KitParams


FIELDS = [
    "kick_decay",
    "kick_pitch_start",
    "kick_pitch_end",
    "kick_drive",
    "snare_decay",
    "snare_tone",
    "snare_noise",
    "hat_decay",
    "open_hat_decay",
    "clap_bursts",
    "tom_low_hz",
    "tom_high_hz",
    "perc_hz",
    "drive_master",
    "noise_mix",
]


def random_walk(best: KitParams, iteration: int, rng_seed: int) -> KitParams:
    import numpy as np

    rng = np.random.default_rng((rng_seed + iteration * 9973) & 0xFFFFFFFF)
    d = best.__dict__.copy()
    # mutate 3–5 fields
    keys = list(FIELDS)
    rng.shuffle(keys)
    for key in keys[: int(rng.integers(3, 6))]:
        if key == "clap_bursts":
            d[key] = int(np.clip(int(d[key]) + int(rng.integers(-1, 2)), 2, 5))
            continue
        scale = 0.08 + 0.04 * float(rng.random())
        d[key] = float(d[key]) * (1.0 + float(rng.normal(0.0, scale)))
    d["seed"] = int(best.seed) + iteration + 1
    return KitParams(**{k: d[k] for k in KitParams.__dataclass_fields__}).clamp()


def _mistral_suggest(best: KitParams, last_score: float) -> Optional[KitParams]:
    api_key = os.environ.get("MISTRAL_API_KEY", "").strip()
    if not api_key:
        return None
    try:
        import requests
    except ImportError:
        return None

    prompt = (
        "You optimize procedural drum-kit synthesis params. "
        "Return ONLY a JSON object with numeric fields matching this schema, "
        "nudging toward punchier Memphis/trap character without extreme values.\n"
        f"current_score={last_score}\ncurrent={json.dumps(best.__dict__)}\n"
        f"fields={FIELDS}"
    )
    try:
        resp = requests.post(
            "https://api.mistral.ai/v1/chat/completions",
            headers={"Authorization": f"Bearer {api_key}", "Content-Type": "application/json"},
            json={
                "model": "mistral-small-latest",
                "messages": [{"role": "user", "content": prompt}],
                "temperature": 0.4,
                "response_format": {"type": "json_object"},
            },
            timeout=45,
        )
        resp.raise_for_status()
        content = resp.json()["choices"][0]["message"]["content"]
        data: Dict[str, Any] = json.loads(content)
        merged = best.__dict__.copy()
        for k in FIELDS:
            if k in data:
                merged[k] = data[k]
        merged["seed"] = int(best.seed) + 17
        return KitParams(**{k: merged[k] for k in KitParams.__dataclass_fields__}).clamp()
    except Exception:
        return None


def propose(best: KitParams, iteration: int, last_score: float, seed: int) -> tuple[KitParams, str]:
    llm = _mistral_suggest(best, last_score)
    if llm is not None:
        return llm, "mistral"
    return random_walk(best, iteration, seed), "random_walk"
