#!/usr/bin/env python3
"""Codegen ActiveAgentSeeds.cpp from data/vst-preset-lineage/active_seeds.json."""

from __future__ import annotations

import json
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
ACTIVE = REPO / "data/vst-preset-lineage/active_seeds.json"

WAVE_OUT = REPO / "Wave9090/Source/Presets/ActiveAgentSeeds.cpp"
PLUGG_OUT = REPO / "PluggVA/Source/Presets/ActiveAgentSeeds.cpp"


def _fmt_float(v: float) -> str:
    return f"{v:.4f}f"


def wave_params_block(params: dict) -> str:
    lines = ["              auto p = baseParams();"]
    for key, val in sorted(params.items()):
        if isinstance(val, bool):
            lines.append(f"              p.{key} = {'true' if val else 'false'};")
        elif isinstance(val, (int, float)):
            if isinstance(val, bool):
                continue
            suffix = "f" if isinstance(val, float) else ""
            lines.append(f"              p.{key} = {val}{suffix};")
    lines.append("              return p;")
    return "\n".join(lines)


def plugg_params_block(params: dict) -> str:
    lines = ["              auto p = base();"]
    wave_map = {"saw": "Waveform::saw", "square": "Waveform::square", "pulse": "Waveform::pulse", "triangle": "Waveform::triangle"}
    for key, val in sorted(params.items()):
        if key in ("osc1Wave", "osc2Wave") and isinstance(val, str):
            lines.append(f"              p.{key} = {wave_map.get(val, 'Waveform::saw')};")
        elif isinstance(val, bool):
            lines.append(f"              p.{key} = {'true' if val else 'false'};")
        elif isinstance(val, float):
            lines.append(f"              p.{key} = {_fmt_float(val)};")
        elif isinstance(val, int):
            lines.append(f"              p.{key} = {val};")
    lines.append("              return p;")
    return "\n".join(lines)


def gen_wave_cpp(seeds: list[dict]) -> str:
    entries = []
    for s in seeds:
        name, cat, params = s["name"], s["category"], s["params"]
        body = wave_params_block(params)
        entries.append(
            f'        {{ "{name}", "{cat}", [] {{\n{body}\n          }}() }},'
        )
    list_body = "\n".join(entries) if entries else ""
    return f'''// AUTO-GENERATED — do not edit. Run scripts/vst/daily_preset_batch.py

#include "Presets/ActiveAgentSeeds.h"
#include "Engine/SynthParams.h"

#include <vector>

namespace wave9090::presets
{{

namespace
{{
SynthParams baseParams() {{ return SynthParams {{}}; }}
}} // namespace

std::vector<FactoryPreset> getActiveAgentSeeds()
{{
    return {{
{list_body}
    }};
}}

}} // namespace wave9090::presets
'''


def gen_plugg_cpp(seeds: list[dict]) -> str:
    entries = []
    for s in seeds:
        name, cat, params = s["name"], s["category"], s["params"]
        body = plugg_params_block(params)
        entries.append(
            f'        {{ "{name}", "{cat}", [] {{\n{body}\n          }}() }},'
        )
    list_body = "\n".join(entries) if entries else ""
    return f'''// AUTO-GENERATED — do not edit. Run scripts/vst/daily_preset_batch.py

#include "Presets/ActiveAgentSeeds.h"
#include "Engine/SynthParams.h"

#include <vector>

namespace pluggva::presets
{{

namespace
{{
SynthParams base() {{ return SynthParams {{}}; }}
}} // namespace

std::vector<FactoryPreset> getActiveAgentSeeds()
{{
    return {{
{list_body}
    }};
}}

}} // namespace pluggva::presets
'''


def main() -> None:
    data = json.loads(ACTIVE.read_text(encoding="utf-8")) if ACTIVE.exists() else {}
    WAVE_OUT.write_text(gen_wave_cpp(data.get("wave9090", [])), encoding="utf-8")
    PLUGG_OUT.write_text(gen_plugg_cpp(data.get("pluggva", [])), encoding="utf-8")
    print(f"Wrote {WAVE_OUT}")
    print(f"Wrote {PLUGG_OUT}")


if __name__ == "__main__":
    main()
