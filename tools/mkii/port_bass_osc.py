#!/usr/bin/env python3
"""Port v1 bass .cpp OSC logic into mkII osc.h (class Osc)."""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

UNITS = [
    ("juno_106_rnb_bass", "src/oscillators/juno-rnb/juno_rnb.cpp"),
    ("dilla_type_bass", "src/oscillators/dilla-bass/dilla_bass.cpp"),
    ("moog_sub_phatty_bass", "src/oscillators/sub-phatty/sub_phatty.cpp"),
    ("prophet5_funk_rnb_bass", "src/oscillators/prophet-funk/prophet_bass.cpp"),
    ("dx7_lately_bass", "src/oscillators/dx7-lately/lately_bass.cpp"),
    ("sh101_babyface_bass", "src/oscillators/sh101-babyface/sh101_bass.cpp"),
    ("moog_voyager_se1_squelch", "src/oscillators/moog-voyager-se/voyager_se.cpp"),
    ("cardo_got_wings_gfunk", "src/oscillators/cardo-gfunk/cardo_bass.cpp"),
    ("larry_june_bay_cruise_bass", "src/oscillators/larry-june/larry_bass.cpp"),
    ("khalifa_haze_stoner_bass", "src/oscillators/khalifa-haze/khalifa_bass.cpp"),
    ("juicy_j_three6_memphis_bass", "src/oscillators/memphis-juicy/juicy_bass.cpp"),
    ("ensoniq_eps1_memphis_bass", "src/oscillators/ensoniq-eps1/eps_bass.cpp"),
]

STAGE_MAP = [
    (r"k_\w+_stage_idle\b", "0"),
    (r"k_\w+_stage_attack\b", "1"),
    (r"k_\w+_stage_decay\b", "2"),
    (r"k_\w+_stage_sustain\b", "3"),
    (r"k_\w+_stage_release\b", "4"),
]


def extract_enum(stub: str) -> str:
    m = re.search(r"enum \{[^}]+\};", stub, re.DOTALL)
    if not m:
        raise ValueError("enum not found")
    block = m.group(0)
    return re.sub(r",\s*NUM_PARAMS\s*", "", block)


def parse_osc_param(cpp: str) -> list[tuple[str, str]]:
    m = re.search(r"void OSC_PARAM\([^)]*\)\s*\{(.*)\n\}\s*\nvoid OSC_CYCLE", cpp, re.DOTALL)
    if not m:
        raise ValueError("OSC_PARAM block not found")
    body = m.group(1)
    cases = re.findall(
        r"case (k_\w+):\s*\n\s*(.*?)\s*break;",
        body,
        re.DOTALL,
    )
    out: list[tuple[str, str]] = []
    for _case, action in cases:
        out.append((_case, action.strip()))
    return cases


def build_set_parameter(stub: str, cpp: str) -> str:
    enum_block = extract_enum(stub)
    names = [n for n in re.findall(r"(\w+)\s*=\s*\d+U", enum_block) if n != "NUM_PARAMS"]
    cases = parse_osc_param(cpp)
    lines = [
        "  void setParameter(uint8_t index, int32_t value) override final {",
        "    const float norm = param_10bit_to_f32(value);",
        "    switch (index) {",
    ]
    for i, en in enumerate(names):
        if i >= len(cases):
            break
        _, action = cases[i]
        action = re.sub(r"\w+_norm100\(value\)", "norm", action)
        action = re.sub(
            r"\w+_time_sec\(value,\s*([^)]+)\)",
            r"timeSec(norm, \1)",
            action,
        )
        action = re.sub(r"\bs_(\w+)\b", r"\1_", action)
        action = re.sub(r"\w+_update_sustain\(\)", "updateSustain()", action)
        action = re.sub(r"\w+_update_sustain_level\(\)", "updateSustainLevel()", action)
        lines.append(f"    case {en}:")
        for ln in action.split("\n"):
            ln = ln.strip()
            if ln:
                lines.append(f"      {ln}")
        lines.append("      break;")
    lines += ["    default:", "      break;", "    }", "  }"]
    return "\n".join(lines)


def extract_defines(cpp: str) -> str:
    lines = []
    for m in re.finditer(r"^#define\s+(\w+)\s+([^\n]+)", cpp, re.MULTILINE):
        name, val = m.group(1), m.group(2).strip()
        if name.endswith("_FS"):
            continue
        if name.endswith("_TWO_PI"):
            continue
        if name.endswith("_AMP_FLOOR"):
            continue
        if "U" in val or val.isdigit():
            lines.append(f"  static constexpr auto {name} = {val};")
    return "\n".join(lines)


def macro_substitutions(text: str) -> str:
    text = re.sub(r"\b\w+_FS\b", "k_fs", text)
    text = re.sub(r"\b\w+_TWO_PI\b", "k_two_pi", text)
    text = re.sub(r"\b\w+_AMP_FLOOR\b", "k_amp_floor", text)
    return text


def remove_c_functions(text: str, pattern: str) -> str:
    while True:
        m = re.search(pattern, text)
        if not m:
            break
        start = m.start()
        brace = text.find("{", m.end())
        if brace < 0:
            break
        depth = 1
        i = brace + 1
        while i < len(text) and depth:
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
            i += 1
        text = text[:start] + text[i:]
    return text


def extract_members_and_methods(cpp: str) -> tuple[str, str]:
    """Return (member_declarations, method_definitions) for private section."""
    start = cpp.find("static ")
    end = cpp.find("void OSC_INIT")
    if start < 0 or end < 0:
        raise ValueError("helper region not found")
    region = cpp[start:end]
    region = re.sub(r"void \w+_reset\(void\) \{.*?\n\}\n", "", region, flags=re.DOTALL)
    region = re.sub(r"^enum \w+.*?\};?\n", "", region, flags=re.MULTILINE | re.DOTALL)

    members: list[str] = []
    for m in re.finditer(
        r"^static\s+([\w\s\*]+?)\s+s_(\w+)\s*(=\s*[^;]+)?;",
        region,
        re.MULTILINE,
    ):
        typ, name = m.group(1).strip(), m.group(2)
        init = m.group(3) or ""
        members.append(f"  {typ} {name}_{init};")
    region = re.sub(
        r"^static\s+([\w\s\*]+?)\s+s_(\w+)\s*(=\s*[^;]+)?;\n",
        "",
        region,
        flags=re.MULTILINE,
    )
    region = re.sub(
        r"^static\s+([\w\s\*]+?)\s+s_(\w+)\s*\[([^\]]+)\]\s*;\n",
        lambda m: "",
        region,
        flags=re.MULTILINE,
    )
    # arrays
    for m in re.finditer(
        r"^static\s+([\w\s\*]+?)\s+s_(\w+)\s*\[([^\]]+)\]\s*;",
        cpp[start:end],
        re.MULTILINE,
    ):
        typ, name, dim = m.group(1).strip(), m.group(2), m.group(3)
        dim = dim.replace("VS_NUM_OSC", "3")
        dim = re.sub(r"\w+_CHORUS_LEN", "1536", dim)
        members.append(f"  {typ} {name}_[{dim}];")

    for pat, repl in STAGE_MAP:
        region = re.sub(pat, repl, region)
    region = re.sub(r"\bs_(\w+)\b", r"\1_", region)
    region = remove_c_functions(region, r"^static float \w+_norm100\(")
    region = remove_c_functions(region, r"^static float \w+_time_sec\(")
    region = remove_c_functions(region, r"^static float \w+_w_to_float\(")
    region = re.sub(r"^static ", "", region, flags=re.MULTILINE)
    region = re.sub(r"void \w+_update_sustain\(void\)", "void updateSustain()", region)
    region = re.sub(r"void \w+_update_sustain_level\(void\)", "void updateSustainLevel()", region)
    region = macro_substitutions(region)
    region = region.replace("JR_CHORUS_LEN", "1536")
    region = region.replace("VS_NUM_OSC", "3")
    # indent methods
    method_lines = []
    for ln in region.strip().split("\n"):
        method_lines.append(f"  {ln}" if ln else ln)
    methods = "\n".join(method_lines)
    # drop duplicate pitch state (handled via w0_/w_tgt_ at top of private)
    members = [m for m in members if " w_tgt_" not in m and "w_tgt_;" not in m]
    if not any("w_cur_" in m for m in members):
        members.insert(0, "  float w_cur_;")
    return "\n".join(members), methods


def extract_note_on(cpp: str) -> str:
    m = re.search(r"void OSC_NOTEON\([^)]*\)\s*\{(.*?)\n\}", cpp, re.DOTALL)
    if not m:
        raise ValueError("OSC_NOTEON not found")
    body = m.group(1)
    for pat, repl in STAGE_MAP:
        body = re.sub(pat, repl, body)
    body = re.sub(r"\bs_(\w+)\b", r"\1_", body)
    body = re.sub(
        r"w_tgt_\s*=\s*\(float\)osc_w0\(params->pitch\);",
        "w_tgt_ = w0_ * k_w_scale;",
        body,
    )
    body = re.sub(r"\(void\)params;\s*", "", body)
    lines = [
        "  void noteOn(uint8_t note, uint8_t velo) override final {",
        "    (void)note;",
        "    (void)velo;",
    ]
    for ln in body.strip().split("\n"):
        ln = ln.strip()
        if ln:
            lines.append(f"    {ln}")
    lines.append("  }")
    return "\n".join(lines)


def extract_note_off(cpp: str) -> str:
    m = re.search(r"void OSC_NOTEOFF\([^)]*\)\s*\{(.*?)\n\}", cpp, re.DOTALL)
    if not m:
        raise ValueError("OSC_NOTEOFF not found")
    body = m.group(1)
    for pat, repl in STAGE_MAP:
        body = re.sub(pat, repl, body)
    body = re.sub(r"\bs_(\w+)\b", r"\1_", body)
    body = re.sub(r"\(void\)params;\s*", "", body)
    lines = ["  void noteOff(uint8_t note) override final {", "    (void)note;"]
    for ln in body.strip().split("\n"):
        ln = ln.strip()
        if ln:
            lines.append(f"    {ln}")
    lines.append("  }")
    return "\n".join(lines)


def extract_process_loop(cpp: str) -> str:
    m = re.search(r"void OSC_CYCLE\([^)]*\)\s*\{", cpp)
    if not m:
        raise ValueError("OSC_CYCLE not found")
    start = m.end()
    depth = 1
    i = start
    while i < len(cpp) and depth:
        if cpp[i] == "{":
            depth += 1
        elif cpp[i] == "}":
            depth -= 1
        i += 1
    body = cpp[start : i - 1]
    for pat, repl in STAGE_MAP:
        body = re.sub(pat, repl, body)
    body = re.sub(r"\bs_(\w+)\b", r"\1_", body)
    body = re.sub(
        r"w_tgt_\s*=\s*\(float\)osc_w0\(params->pitch\);",
        "w_tgt_ = w0_ * k_w_scale;",
        body,
    )
    body = re.sub(
        r"const float host_cut = \(float\)params->cutoff \* \(1\.0f / 8191\.0f\);",
        "const float host_cut = host_cut_;",
        body,
    )
    body = re.sub(
        r"const float host_res = \(float\)params->resonance \* \(1\.0f / 8191\.0f\);",
        "const float host_res = host_res_;",
        body,
    )
    body = re.sub(r"q31_t \*y = \(q31_t \*\)yn;\s*", "", body)
    body = re.sub(
        r"for \(uint32_t i = 0; i < frames; i\+\+\) \{",
        "for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {",
        body,
    )
    body = re.sub(r"\*y\+\+ = f32_to_q31\(sig\);", "out[0] = sig;", body)
    return body.strip()


def build_reset(stub: str, cpp: str) -> str:
    """Reset DSP state; knob defaults from mkII stub via setParameter()."""
    m = re.search(r"void \w+_reset\(void\) \{(.*?)\n\}", cpp, re.DOTALL)
    if not m:
        raise ValueError("v1 reset not found")
    body = m.group(1)
    for pat, repl in STAGE_MAP:
        body = re.sub(pat, repl, body)
    body = re.sub(r"\bs_(\w+)\b", r"\1_", body)
    skip_prefixes = (
        "vibe_", "saw_", "pwm_", "chorus_", "glide_", "attack_", "decay_", "release_",
        "hpf_", "sub_", "detune_", "fm_", "fold_", "wobble_", "warmth_", "grit_",
        "thick_", "osc_mix_", "drive_", "index_", "bite_", "ratio_", "tone_", "hollow_",
        "fat_", "square_", "squelch_", "trunk_", "tape_", "mid_", "haze_", "float_",
        "wave_", "drift_", "dark_", "reso_", "pulse_", "rubber_", "space_", "velvet_",
        "glow_", "sync_",
    )
    lines = ["  void reset() override final {"]
    for ln in body.strip().split("\n"):
        ln = ln.strip()
        if not ln:
            continue
        if any(ln.startswith(p) and " = " in ln for p in skip_prefixes):
            continue
        if "_sec = " in ln:
            continue
        ln = re.sub(r"\w+_update_sustain\(\);?", "updateSustain();", ln)
        ln = re.sub(r"\w+_update_sustain_level\(\);?", "updateSustainLevel();", ln)
        lines.append(f"    {ln}")

    inits = re.findall(r"p\d+\s*=\s*param_10bit_to_f32\((\d+)\)", stub)
    enum_block = extract_enum(stub)
    names = [n for n in re.findall(r"(\w+)\s*=\s*\d+U", enum_block) if n != "NUM_PARAMS"]
    for name, num in zip(names, inits):
        lines.append(f"    setParameter({name}, {num});")
    lines.append("    w0_ = 0.0f;")
    lines.append("    host_cut_ = 1.0f;")
    lines.append("    host_res_ = 0.5f;")
    lines.append("  }")
    return "\n".join(lines)


def port_one(slug: str, v1_rel: str) -> str:
    stub = (REPO / "src/mkii/oscillators" / slug / "osc.h").read_text(encoding="utf-8")
    cpp = (REPO / v1_rel).read_text(encoding="utf-8")
    enum_block = extract_enum(stub)
    note = re.search(r"PORT DSP from v1: ([^\n]+)", stub)
    v1_note = note.group(1) if note else v1_rel

    set_param = build_set_parameter(stub, cpp)
    extra_defines = extract_defines(cpp)
    member_decls, methods = extract_members_and_methods(cpp)
    note_on = extract_note_on(cpp)
    note_off = extract_note_off(cpp)
    loop = macro_substitutions(extract_process_loop(cpp))
    reset_fn = build_reset(stub, cpp)

    return f"""#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of {v1_note}
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {{
public:
  uint32_t getBufferSize() const override final {{ return 0; }}

  {enum_block}

  static constexpr float k_fs = 48000.0f;
  static constexpr float k_two_pi = 6.28318530718f;
  static constexpr float k_w_scale = 4294967296.0f;
  static constexpr float k_amp_floor = 0.00008f;
{extra_defines}

  static float timeSec(float norm, float min_s, float max_s) {{
    return min_s + norm * norm * max_s;
  }}

{set_param}

  void init(float *) override final {{ reset(); }}

{reset_fn}

  void setPitch(float w0) {{ w0_ = w0; }}
  void setShapeLfo(float lfo) {{ (void)lfo; }}

  void setHostCutoffRes(float cut, float res) {{
    host_cut_ = cut;
    host_res_ = res;
  }}

{note_on}

{note_off}

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {{
    (void)in;
    w_tgt_ = w0_ * k_w_scale;
{loop}
  }}

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

{member_decls}

{methods}
}};
"""


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("slug", nargs="?")
    ap.add_argument("--all", action="store_true")
    args = ap.parse_args()
    todo = UNITS
    if args.slug:
        todo = [u for u in UNITS if u[0] == args.slug]
    if args.all:
        todo = UNITS
    if not todo:
        print("nothing to do", file=sys.stderr)
        return 1
    for slug, rel in todo:
        try:
            text = port_one(slug, rel)
            out = REPO / "src/mkii/oscillators" / slug / "osc.h"
            out.write_text(text, encoding="utf-8")
            print(f"OK {slug}")
        except Exception as exc:
            print(f"FAIL {slug}: {exc}", file=sys.stderr)
            return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
