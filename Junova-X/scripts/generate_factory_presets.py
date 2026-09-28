#!/usr/bin/env python3
"""Regenerate Source/Presets/FactoryPresets.cpp (48 MVP programs)."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "Source/Presets/FactoryPresets.cpp"


def fmt_float(v: float) -> str:
    s = f"{float(v):.6f}".rstrip("0").rstrip(".")
    if "e" in s or "E" in s:
        return f"{float(v):.6g}f"
    if "." not in s:
        s += ".0"
    return s + "f"


def main() -> None:
    # Keep in sync with WO-2026-003 taxonomy; edit categories/names here.
    categories = [
        ("BASS", 8, dict(filterCutoff=0.28, filterRes=0.12, dcoSubLvl=95, dcoPwm=40, ampDecay=0.35, ampSustain=0.85, chorusMode=0)),
        ("PAD", 10, dict(filterCutoff=0.52, filterRes=0.08, dcoSubLvl=55, dcoPwm=72, ampAttack=0.08, ampRelease=1.2, chorusMode=1, width=130)),
        ("LEAD", 10, dict(filterCutoff=0.62, filterRes=0.22, dcoLfoMod=45, vcfEnv=70, ampDecay=0.18, chorusMode=2, detune=12, voiceMode=3)),
        ("POLY", 10, dict(filterCutoff=0.58, filterRes=0.15, dcoPwm=58, chorusMode=1, voiceMode=0)),
        ("KEYS", 6, dict(filterCutoff=0.68, filterRes=0.1, ampAttack=0.002, ampDecay=0.25, dcoNoise=5, chorusMode=0)),
        ("FX", 4, dict(filterCutoff=0.85, filterRes=0.55, dcoNoise=35, lfoRate=8.5, vcfLfo=80, chorusMode=3)),
    ]
    names = {
        "BASS": ["Sub Juno", "Disk Bass", "Analog Floor", "Low Rider", "Club Sub", "Warm Bottom", "Pulse Bass", "Night Drive"],
        "PAD": ["Celestial Wash", "Nebula Choir", "Glass Pad", "Solar Haze", "Midnight Air", "Analog Cloud", "Dream Stack", "Wide Aurora", "Soft Poly", "Hold Fade"],
        "LEAD": ["Neon Solo", "Cut Lead", "Sync Bite", "Portamento", "Bright Hook", "Filter Talk", "Reso Lead", "Stage Solo", "Hook Line", "Laser Mono"],
        "POLY": ["Classic Poly", "Brass Stack", "Chorus Keys", "Unison Stack", "Five Voice", "Disco Poly", "Junova Stack", "Spread Poly", "Room Poly", "Init Plus"],
        "KEYS": ["Electric Keys", "Soft Clav", "Pluck Key", "Bell Key", "House Key", "Mellow Key"],
        "FX": ["Noise Sweep", "Reso FX", "LFO Wobble", "Chorus Wash"],
    }
    fields_order = [
        "masterGain", "ampAttack", "ampDecay", "ampSustain", "ampRelease",
        "filtAttack", "filtDecay", "filtSustain", "filtRelease",
        "filterCutoff", "filterRes", "hpfEnabled", "hpfCutoff",
        "chorusMode", "voiceMode", "lfoRate", "lfoDelay", "glide",
        "dcoLfoMod", "dcoPwm", "dcoSubLvl", "dcoNoise",
        "vcfEnv", "vcfLfo", "vcfKey", "drift", "detune", "width",
        "arpRange", "arpRate",
    ]
    entries: list[str] = []
    idx = 0
    for cat, count, overrides in categories:
        for i in range(count):
            name = names[cat][i]
            p = {
                "masterGain": 0.0,
                "ampAttack": 0.01, "ampDecay": 0.22, "ampSustain": 0.72, "ampRelease": 0.45,
                "filtAttack": 0.005, "filtDecay": 0.32, "filtSustain": 0.38, "filtRelease": 0.48,
                "filterCutoff": 0.55 + (idx % 7) * 0.04,
                "filterRes": 0.12 + (idx % 5) * 0.03,
                "hpfEnabled": idx % 9 == 0,
                "hpfCutoff": 0.15 + (idx % 4) * 0.05,
                "chorusMode": idx % 4,
                "voiceMode": idx % 4,
                "lfoRate": 2.0 + (idx % 6),
                "lfoDelay": 0.1,
                "glide": 8.0 if cat == "LEAD" else 12.0,
                "dcoLfoMod": 25 + (idx % 8) * 3,
                "dcoPwm": 50 + (idx % 10) * 4,
                "dcoSubLvl": 70,
                "dcoNoise": 8 + (idx % 3) * 4,
                "vcfEnv": 45 + (idx % 6) * 5,
                "vcfLfo": 28 + (idx % 5) * 4,
                "vcfKey": 75,
                "drift": 10 + (idx % 7),
                "detune": 4 + (idx % 5),
                "width": 100 + (idx % 3) * 15,
                "arpRange": 2, "arpRate": 0.25,
            }
            p.update(overrides)
            if cat == "LEAD" and name == "Portamento":
                p["glide"] = 120.0
            inits = ",\n                ".join(
                f".{k} = {(str(v).lower() if isinstance(v, bool) else (str(v) if k in ('chorusMode', 'voiceMode') else fmt_float(v)))}"
                for k in fields_order
                for v in [p[k]]
            )
            entries.append(f'        {{ "{name}", "{cat}", PresetParams {{\n                {inits}\n            }} }},')
            idx += 1
    if idx != 48:
        raise SystemExit(f"expected 48 presets, got {idx}")
    cpp = (
        '#include "Presets/PresetParams.h"\n\n#include <vector>\n\nnamespace junovax::presets\n{\n'
        "const std::vector<FactoryPreset>& getFactoryPresets() noexcept\n{\n"
        "    static const std::vector<FactoryPreset> kPresets = {\n"
        + "\n".join(entries)
        + "\n    };\n    return kPresets;\n}\n} // namespace junovax::presets\n"
    )
    OUT.write_text(cpp, encoding="utf-8")
    print(f"wrote {OUT.relative_to(ROOT.parents[1])} ({idx} presets)")


if __name__ == "__main__":
    main()
