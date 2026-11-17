"""Morning evolution report from state.json history."""
from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, List


def write_morning_report(state_path: Path, out_path: Path) -> Path:
    state: Dict[str, Any] = {}
    if state_path.is_file():
        state = json.loads(state_path.read_text(encoding="utf-8"))

    history: List[Dict[str, Any]] = state.get("history", [])
    best = state.get("best", {})
    lines = [
        "# Overnight Agent — Morning Report",
        "",
        f"Generated: {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M UTC')}",
        "",
        f"- Iterations completed: **{len(history)}**",
        f"- Best score: **{best.get('score', 'n/a')}**",
        f"- Best kit: `{best.get('kit_dir', 'n/a')}`",
        f"- Last mutation mode: `{state.get('last_mutation', 'n/a')}`",
        "",
        "## Score trajectory",
        "",
        "| Iter | Score | OK | Mutation | Kit |",
        "|------|-------|----|----------|-----|",
    ]
    for row in history[-50:]:
        lines.append(
            f"| {row.get('iteration')} | {row.get('score')} | {row.get('ok')} | "
            f"{row.get('mutation')} | `{Path(str(row.get('kit_dir', ''))).name}` |"
        )

    lines.extend(
        [
            "",
            "## Best params",
            "",
            "```json",
            json.dumps(best.get("params", {}), indent=2),
            "```",
            "",
            "## Notes",
            "",
            "- Plugin was **not** rebuilt each iteration (kits load from disk).",
            "- `COPY_PLUGIN_AFTER_BUILD` stays OFF — no writes to Program Files VST3.",
            "- Set `MISTRAL_API_KEY` to enable LLM mutation proposals.",
            "",
        ]
    )
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines), encoding="utf-8")
    return out_path
