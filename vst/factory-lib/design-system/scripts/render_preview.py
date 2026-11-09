#!/usr/bin/env python3
"""Render previews/<theme>.html (a visual reference sheet) from each theme's tokens.json.

Output is deterministic so check_tokens.py --check-preview can detect drift.

Usage:
    python3 vst/factory-lib/design-system/scripts/render_preview.py               # all themes
    python3 vst/factory-lib/design-system/scripts/render_preview.py --theme disklordz
"""
from __future__ import annotations

import argparse
import html
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from check_tokens import (  # noqa: E402
    PREVIEWS,
    contrast,
    effective_pairs,
    leaves,
    list_themes,
    load_contract,
    load_theme,
    preview_path,
    resolve,
    resolve_path,
)


def css_vars(t: dict) -> str:
    out = []
    for path, raw in leaves(t["semantic"], "semantic"):
        name = "--" + path.removeprefix("semantic.").replace(".", "-")
        out.append(f"  {name}: {resolve(t, raw, (path,))};")
    return "\n".join(out)


def c(t: dict, path: str) -> str:
    return resolve_path(t, "semantic." + path)


# ---------- SVG helpers ----------

KNOB_START, KNOB_END = 135.0, 405.0  # degrees, clockwise in SVG space (y down)


def _pt(cx: float, cy: float, r: float, deg: float) -> tuple[float, float]:
    a = math.radians(deg)
    return cx + r * math.cos(a), cy + r * math.sin(a)


def arc(cx: float, cy: float, r: float, a0: float, a1: float) -> str:
    if a1 < a0:
        a0, a1 = a1, a0
    x0, y0 = _pt(cx, cy, r, a0)
    x1, y1 = _pt(cx, cy, r, a1)
    large = 1 if (a1 - a0) > 180 else 0
    return f"M{x0:.2f},{y0:.2f} A{r:.2f},{r:.2f} 0 {large} 1 {x1:.2f},{y1:.2f}"


def knob_svg(t: dict, d: int, value: float, origin: float = 0.0, state: str = "default") -> str:
    inset = t["size"]["knobArcInset"]
    arc_w = t["stroke"]["arcMd"] if d >= 44 else t["stroke"]["arcSm"]
    size = d + 2 * (inset + arc_w)
    cx = cy = size / 2
    body_r = d / 2
    ring_r = body_r + inset + arc_w / 2
    a_val = KNOB_START + (KNOB_END - KNOB_START) * value
    a_org = KNOB_START + (KNOB_END - KNOB_START) * origin
    disabled = state == "disabled"
    stroke = c(t, "text.disabled") if disabled else c(t, "brand.stroke")
    pointer = c(t, "text.disabled") if disabled else c(t, "control.pointer")
    px0, py0 = _pt(cx, cy, body_r * 0.35, a_val)
    px1, py1 = _pt(cx, cy, body_r * 0.85, a_val)
    gid = f"kg{d}{state}{int(value * 100)}"
    ds = t["elevation"]["raised"]["dropShadow"]
    focus = ""
    if state == "focused":
        fr = ring_r + arc_w / 2 + t["stroke"]["focusOffset"] + t["stroke"]["focus"] / 2
        focus = (f'<circle cx="{cx}" cy="{cy}" r="{fr:.2f}" fill="none" stroke="{c(t, "focus.ring")}" '
                 f'stroke-width="{t["stroke"]["focus"]}"/>')
        size_pad = 2 * (t["stroke"]["focusOffset"] + t["stroke"]["focus"])
    else:
        size_pad = 2 * (t["stroke"]["focusOffset"] + t["stroke"]["focus"])
    total = size + size_pad
    off = size_pad / 2
    top = c(t, "control.bodyTop") if state != "hover" else c(t, "control.rim")
    return (
        f'<svg width="{total:.0f}" height="{total:.0f}" viewBox="{-off} {-off} {total} {total}" '
        f'role="img" aria-label="knob {int(value * 100)} percent">'
        f'<defs><linearGradient id="{gid}" x1="0" y1="0" x2="0" y2="1">'
        f'<stop offset="0" stop-color="{top}"/><stop offset="1" stop-color="{c(t, "control.body")}"/>'
        f"</linearGradient></defs>"
        f'<path d="{arc(cx, cy, ring_r, KNOB_START, KNOB_END)}" fill="none" stroke="{c(t, "control.track")}" '
        f'stroke-width="{arc_w}" stroke-linecap="round"/>'
        f'<path d="{arc(cx, cy, ring_r, a_org, a_val)}" fill="none" stroke="{stroke}" '
        f'stroke-width="{arc_w}" stroke-linecap="round"/>'
        f'<circle cx="{cx}" cy="{cy + ds["offsetY"]}" r="{body_r}" fill="{ds["color"]}" opacity="{ds["alpha"]}"/>'
        f'<circle cx="{cx}" cy="{cy}" r="{body_r}" fill="url(#{gid})" stroke="{c(t, "control.rim")}" '
        f'stroke-width="{t["stroke"]["rim"]}"/>'
        f'<line x1="{px0:.2f}" y1="{py0:.2f}" x2="{px1:.2f}" y2="{py1:.2f}" stroke="{pointer}" '
        f'stroke-width="2" stroke-linecap="round"/>'
        f"{focus}</svg>"
    )


def meter_html(t: dict, width: int, height: int, rms_db: float, peak_db: float, hold_db: float, over: bool) -> str:
    m = t["meter"]
    floor = m["defaultFloorDb"]

    def y(db: float) -> float:
        db = max(floor, min(0.0, db))
        return height * (db / floor)  # 0 dB -> 0 (top), floor -> height

    rms_top = y(rms_db)
    peak_top = y(peak_db)
    hot_y = y(m["hotThresholdDb"])
    pw = width * m["peakWidthFraction"]
    px = (width - pw) / 2
    parts = [
        f'<div class="meter" style="width:{width}px">',
        f'<div class="over-cap" style="height:{m["overCapHeight"]}px;background:'
        f'{c(t, "meter.over") if over else c(t, "state.ledOff")}"></div>',
        f'<div class="meter-well" style="height:{height}px">',
        f'<div style="position:absolute;left:0;right:0;top:{rms_top:.1f}px;bottom:0;background:{c(t, "meter.rms")}"></div>',
    ]
    if peak_top < hot_y:
        parts.append(f'<div style="position:absolute;left:{px:.1f}px;width:{pw:.1f}px;top:{hot_y:.1f}px;bottom:0;'
                     f'background:{c(t, "meter.peak")}"></div>')
        parts.append(f'<div style="position:absolute;left:{px:.1f}px;width:{pw:.1f}px;top:{peak_top:.1f}px;'
                     f'height:{hot_y - peak_top:.1f}px;background:{c(t, "meter.hot")}"></div>')
    else:
        parts.append(f'<div style="position:absolute;left:{px:.1f}px;width:{pw:.1f}px;top:{peak_top:.1f}px;bottom:0;'
                     f'background:{c(t, "meter.peak")}"></div>')
    parts.append(f'<div style="position:absolute;left:0;right:0;top:{y(hold_db):.1f}px;'
                 f'height:{m["holdLineThickness"]}px;background:{c(t, "meter.hold")}"></div>')
    parts.append("</div></div>")
    return "".join(parts)


def scale_html(t: dict, height: int) -> str:
    m = t["meter"]
    floor = m["defaultFloorDb"]
    marks = []
    for db in m["ticksDb"]:
        top = height * (db / floor)
        label = str(db).replace("-", "−") if db in m["labelledTicksDb"] else ""
        marks.append(f'<div class="tick" style="top:{top:.1f}px"><span>{label}</span></div>')
    return f'<div class="scale" style="height:{height}px;margin-top:{m["overCapHeight"]}px">{"".join(marks)}</div>'


# ---------- page ----------

def section(title: str, body: str) -> str:
    return f'<section><h2>{html.escape(title)}</h2>{body}</section>'


def swatches(t: dict) -> str:
    rows = []
    for family, shades in t["color"].items():
        cells = "".join(
            f'<div class="sw"><div class="chip" style="background:{hexv}"></div>'
            f'<div class="mono">{family}.{shade}</div><div class="mono dim">{hexv}</div></div>'
            for shade, hexv in shades.items()
        )
        rows.append(f'<div class="row">{cells}</div>')
    sem = []
    for path, raw in leaves(t["semantic"], "semantic"):
        hexv = resolve(t, raw, (path,))
        sem.append(f'<div class="sw"><div class="chip" style="background:{hexv}"></div>'
                   f'<div class="mono">{path.removeprefix("semantic.")}</div><div class="mono dim">{hexv}</div></div>')
    restr = "".join(f"<li><b>{r['primitive']}</b> only under {', '.join(r['allowedUnder'])}: "
                    f"{html.escape(r['reason'])}</li>" for r in t.get("usageRestrictions", []))
    restr_block = f"<h3>Usage restrictions (enforced)</h3><ul>{restr}</ul>" if restr else ""
    return ("<h3>Primitives</h3>" + "".join(rows) + "<h3>Semantic</h3>" + f'<div class="row">{"".join(sem)}</div>'
            + restr_block)


def contrast_table(t: dict, contract: dict) -> str:
    rows = []
    for p in effective_pairs(t, contract):
        fg, bg = resolve_path(t, p["fg"]), resolve_path(t, p["bg"])
        r = contrast(fg, bg)
        rows.append(f'<tr><td><span class="pair" style="background:{bg};color:{fg}">Aa ▮</span></td>'
                    f'<td class="mono">{r:.2f}</td><td class="mono">{p["min"]}</td>'
                    f'<td class="mono">{p["fg"].removeprefix("semantic.")} / {p["bg"].removeprefix("semantic.")}</td>'
                    f'<td>{html.escape(p["use"])}</td></tr>')
    return ('<table><tr><th>Sample</th><th>Ratio</th><th>Min</th><th>Pair</th><th>Use</th></tr>'
            + "".join(rows) + "</table>")


def type_specimens(t: dict) -> str:
    fam = t["typography"]["family"]
    out = []
    for name, s in t["typography"]["style"].items():
        f = fam[s["family"]]
        text = {"productName": "MeterTap", "sectionTitle": "Input stage", "legend": "Trim",
                "body": "Adjusts input level before metering.", "value": "−12.0 dB",
                "valueLarge": "−15.01", "scale": "−6  −12  −24", "micro": "v0.1.0 · Pro"}[name]
        transform = "uppercase" if s["case"] == "upper" else "none"
        out.append(
            f'<div class="type-row"><div class="mono dim">{name}<br>{f["name"]} {s["size"]}/{s["lineHeight"]} '
            f'w{s["weight"]}</div><div style="font-family:\'{f["name"]}\',{f["fallback"]};font-size:{s["size"]}px;'
            f'line-height:{s["lineHeight"]}px;font-weight:{s["weight"]};letter-spacing:{s["tracking"]}em;'
            f'text-transform:{transform}">{html.escape(text)}</div></div>')
    return "".join(out)


def spacing(t: dict) -> str:
    sp = "".join(f'<div class="sp"><div style="width:{v}px;height:12px;background:var(--brand-stroke)"></div>'
                 f'<span class="mono dim">space.{k} = {v}</span></div>' for k, v in t["space"].items())
    rad = "".join(f'<div class="rad" style="border-radius:{min(v, 24)}px"><span class="mono dim">{k} {v}</span></div>'
                  for k, v in t["radius"].items())
    elev = ('<div class="row"><div class="elev well-sample">well (recessed)</div>'
            '<div class="elev flat-sample">flat</div><div class="elev raised-sample">raised</div>'
            '<div class="elev overlay-sample">overlay</div></div>')
    return f"<h3>Spacing (4 px grid)</h3>{sp}<h3>Radius</h3><div class='row'>{rad}</div><h3>Elevation</h3>{elev}"


def controls(t: dict) -> str:
    ks = t["size"]["knob"]
    sizes = "".join(f'<div class="ctl">{knob_svg(t, d, 0.62)}<div class="legend">{k}</div>'
                    f'<div class="mono dim">{d}px</div></div>' for k, d in ks.items())
    states = "".join(
        f'<div class="ctl">{knob_svg(t, ks["md"], v, o, s)}<div class="legend">{label}</div>'
        f'<div class="value">{val}</div></div>'
        for s, v, o, label, val in [
            ("default", 0.62, 0.0, "Drive", "62 %"),
            ("hover", 0.62, 0.0, "Hover", "62 %"),
            ("focused", 0.62, 0.0, "Focus", "62 %"),
            ("default", 0.80, 0.6667, "Trim (bipolar)", "+6.0 dB"),
            ("disabled", 0.40, 0.0, "Disabled", "40 %"),
        ])
    toggles = (
        '<button class="toggle"><i class="led"></i>Hold</button>'
        '<button class="toggle on"><i class="led"></i>Hold</button>'
        '<button class="toggle caution"><i class="led"></i>Bypass</button>'
        '<button class="toggle focus"><i class="led"></i>Focus</button>'
        '<button class="toggle" disabled><i class="led"></i>Disabled</button>'
    )
    buttons = ('<button class="btn">Secondary</button><button class="btn primary">Render</button>'
               '<button class="btn" disabled>Disabled</button><span class="badge">Pro</span>')
    tabs = ('<div class="tabs"><button class="tab sel">Main</button><button class="tab">Meter</button>'
            '<button class="tab">Settings</button></div>'
            '<div class="seg"><button class="sel">Peak</button><button>RMS</button><button>Both</button></div>')
    slider = ('<div class="slider"><div class="track"><div class="fill" style="width:58%"></div></div>'
              '<div class="thumb" style="left:58%"></div></div>')
    readout = ('<div class="readout"><span class="v">−15.01</span><span class="u">dBFS</span></div>'
               '<div class="readout"><span class="v">−∞</span><span class="u">dBFS</span></div>'
               '<div class="readout over"><span class="v">+1.2</span><span class="u">OVER</span></div>')
    tooltip = ('<div class="tooltip"><b>Input trim</b><br>Gain applied before metering and output.<br>'
               '<span class="dim">Range −24…+12 dB · default 0 dB · double-click to reset</span></div>')
    return (f"<h3>Knob sizes</h3><div class='row'>{sizes}</div>"
            f"<h3>Knob states</h3><div class='row'>{states}</div>"
            f"<h3>LED toggles</h3><div class='row'>{toggles}</div>"
            f"<h3>Buttons and badge</h3><div class='row'>{buttons}</div>"
            f"<h3>Tabs and segmented selector</h3><div class='row'>{tabs}</div>"
            f"<h3>Linear slider</h3><div class='row'>{slider}</div>"
            f"<h3>Value readouts</h3><div class='row'>{readout}</div>"
            f"<h3>Tooltip</h3><div class='row'>{tooltip}</div>")


def meters(t: dict) -> str:
    h, w = 240, t["size"]["meterBar"]["xl"]
    stereo = (
        '<div class="meter-group">'
        f'<div class="mcol">{meter_html(t, w, h, -15.0, -9.5, -8.0, False)}<div class="legend c">L</div>'
        '<div class="value c">−8.0</div></div>'
        f'{scale_html(t, h)}'
        f'<div class="mcol">{meter_html(t, w, h, -10.0, -2.0, 0.0, True)}<div class="legend c">R</div>'
        '<div class="value c over-text">+1.2</div></div>'
        '</div>'
    )
    small = "".join(
        f'<div class="mcol">{meter_html(t, wv, 120, -18, -10, -7, False)}<div class="mono dim c">{k}</div></div>'
        for k, wv in t["size"]["meterBar"].items())
    return (f"<div class='row'>{stereo}<div class='note'>"
            "<p><b>RMS</b>: full width, <code>meter.rms</code>. <b>Peak</b>: centre half-width, <code>meter.peak</code>; "
            f"<code>meter.hot</code> above {str(t['meter']['hotThresholdDb']).replace('-', '−')} dBFS. "
            f"<b>Hold</b>: {t['meter']['holdLineThickness']} px <code>meter.hold</code> line. "
            "<b>Over</b>: <code>meter.over</code> cap above the bar plus a <code>state.danger</code> readout, "
            "latched until the meter is clicked.</p>"
            "<p>Left: normal programme. Right: clipped signal with over latched.</p></div></div>"
            f"<h3>Bar widths</h3><div class='row'>{small}</div>")


def header_bar(t: dict) -> str:
    glyph = ('<svg width="22" height="22" viewBox="0 0 22 22" aria-label="DiskLordz glyph placeholder">'
             '<rect x="2" y="5" width="18" height="15" rx="2" fill="none" stroke="var(--brand-stroke)" stroke-width="1.5"/>'
             '<rect x="6" y="5" width="9" height="5" fill="var(--brand-stroke)"/>'
             '<path d="M5 4 L8 1 L11 4 L14 1 L17 4" fill="none" stroke="var(--premium-badge)" stroke-width="1.5"/>'
             "</svg>")
    return ('<div class="header">'
            f'<div class="hl">{glyph}<span class="product">{html.escape(t.get("previewProduct", "MeterTap"))}</span>'
            f'<span class="micro dim">{html.escape(t["brandName"])}</span></div>'
            '<div class="preset"><button class="arrow" aria-label="previous preset">‹</button>'
            '<span>Init</span><button class="arrow" aria-label="next preset">›</button></div>'
            '<div class="hr"><span class="micro dim">v0.1.0</span>'
            '<button class="toggle caution"><i class="led"></i>Bypass</button></div></div>')


def shadow(spec: dict | None, inset: bool = False) -> str:
    if not spec:
        return "0 0 0 transparent"
    h = spec["color"].lstrip("#")
    r, g, b = (int(h[i : i + 2], 16) for i in (0, 2, 4))
    return (f"{'inset ' if inset else ''}0 {spec['offsetY']}px {spec.get('blur', 0)}px "
            f"rgba({r},{g},{b},{spec['alpha']})")


def render(t: dict, contract: dict | None = None) -> str:
    contract = contract or load_contract()
    fam = t["typography"]["family"]
    sans = f"'{fam['sans']['name']}', {fam['sans']['fallback']}, 'Segoe UI', sans-serif"
    mono = f"'{fam['mono']['name']}', {fam['mono']['fallback']}, Consolas, monospace"
    st = t["typography"]["style"]
    r = t["radius"]
    size = t["size"]
    e = t["elevation"]
    css = f"""
:root {{
{css_vars(t)}
  --sans: {sans};
  --raised-shadow: {shadow(e['raised'].get('topHighlight'), True)}, {shadow(e['raised']['dropShadow'])};
  --well-shadow: {shadow(e['well']['innerShadow'], True)}, {shadow(e['well']['bottomHighlight'], True)};
  --overlay-shadow: {shadow(e['overlay']['dropShadow'])};
  --mono: {mono};
}}
* {{ box-sizing: border-box; }}
body {{ margin: 0; background: var(--bg-chassis); color: var(--text-primary); font-family: var(--sans);
  font-size: {st['body']['size']}px; line-height: {st['body']['lineHeight']}px; }}
main {{ max-width: 1120px; margin: 0 auto; padding: 32px 24px 64px; }}
h1 {{ font-size: 22px; letter-spacing: .04em; text-transform: uppercase; margin: 0 0 4px; }}
h2 {{ font-size: {st['sectionTitle']['size']}px; letter-spacing: {st['sectionTitle']['tracking']}em; text-transform: uppercase;
  color: var(--text-secondary); border-bottom: 1px solid var(--border-divider); padding-bottom: 8px; margin: 40px 0 16px; }}
h3 {{ font-size: {st['legend']['size']}px; letter-spacing: .08em; text-transform: uppercase; color: var(--text-tertiary);
  margin: 24px 0 8px; font-weight: 600; }}
section {{ background: var(--bg-panel); border-radius: {r['md']}px; padding: 4px 24px 24px; margin-top: 24px; }}
.row {{ display: flex; flex-wrap: wrap; gap: 16px; align-items: flex-end; }}
.mono {{ font-family: var(--mono); font-size: {st['scale']['size']}px; line-height: {st['scale']['lineHeight']}px; }}
.dim {{ color: var(--text-tertiary); }}
.c {{ text-align: center; }}
.micro {{ font-size: {st['micro']['size']}px; letter-spacing: {st['micro']['tracking']}em; text-transform: uppercase; font-weight: 600; }}
.sw {{ width: 112px; }}
.chip {{ height: 40px; border-radius: {r['sm']}px; border: 1px solid var(--border-subtle); margin-bottom: 4px; }}
table {{ border-collapse: collapse; width: 100%; }}
th, td {{ text-align: left; padding: 6px 8px; border-bottom: 1px solid var(--border-subtle); }}
th {{ color: var(--text-tertiary); font-weight: 600; font-size: 11px; text-transform: uppercase; letter-spacing: .08em; }}
.pair {{ display: inline-block; padding: 2px 10px; border-radius: {r['xs']}px; font-weight: 600; }}
.type-row {{ display: grid; grid-template-columns: 220px 1fr; gap: 16px; align-items: center; padding: 8px 0;
  border-bottom: 1px solid var(--border-subtle); }}
.sp {{ display: flex; align-items: center; gap: 12px; margin: 4px 0; }}
.rad {{ width: 72px; height: 48px; background: var(--control-body); border: 1px solid var(--control-rim);
  display: flex; align-items: flex-end; padding: 4px; }}
.elev {{ width: 150px; height: 64px; display: flex; align-items: center; justify-content: center; border-radius: {r['sm']}px;
  color: var(--text-secondary); }}
.well-sample {{ background: var(--bg-well); box-shadow: var(--well-shadow); }}
.flat-sample {{ background: var(--bg-section); }}
.raised-sample {{ background: linear-gradient(var(--control-bodyTop), var(--control-body)); box-shadow: var(--raised-shadow); }}
.overlay-sample {{ background: var(--bg-overlay); border: 1px solid var(--border-overlay); box-shadow: var(--overlay-shadow); }}
.ctl {{ display: flex; flex-direction: column; align-items: center; gap: {size['legendGap']}px; min-width: 72px; }}
.legend {{ font-size: {st['legend']['size']}px; line-height: {st['legend']['lineHeight']}px; letter-spacing: {st['legend']['tracking']}em;
  text-transform: uppercase; font-weight: 600; color: var(--text-primary); }}
.value {{ font-family: var(--mono); font-size: {st['value']['size']}px; color: var(--text-secondary); }}
button {{ font-family: var(--sans); }}
.toggle, .btn {{ height: {size['toggle']['md']}px; padding: 0 12px; border-radius: {r['xs']}px; border: 1px solid var(--control-rim);
  background: linear-gradient(var(--control-bodyTop), var(--control-body)); color: var(--text-primary);
  font-size: {st['legend']['size']}px; letter-spacing: {st['legend']['tracking']}em; text-transform: uppercase; font-weight: 600;
  display: inline-flex; align-items: center; gap: 8px;
  box-shadow: var(--raised-shadow); }}
.btn {{ height: {size['button']['md']}px; }}
.led {{ width: {size['led']}px; height: {size['led']}px; border-radius: 50%; background: var(--state-ledOff); display: inline-block; }}
.toggle.on .led {{ background: var(--active-default); box-shadow: 0 0 6px var(--active-default); }}
.toggle.caution .led {{ background: var(--state-caution); box-shadow: 0 0 6px var(--state-caution); }}
.toggle.focus {{ outline: {t['stroke']['focus']}px solid var(--focus-ring); outline-offset: {t['stroke']['focusOffset']}px; }}
button:disabled {{ color: var(--text-disabled); box-shadow: none; background: var(--control-body); }}
.btn.primary {{ background: var(--active-default); color: var(--text-onActive); border-color: var(--active-pressed); }}
.badge {{ background: var(--premium-badge); color: var(--text-onPremium); font-size: 10px; font-weight: 700;
  letter-spacing: .06em; text-transform: uppercase; padding: 2px 6px; border-radius: {r['xs']}px; }}
.tabs {{ display: flex; border-bottom: 1px solid var(--border-divider); }}
.tab {{ background: none; border: 0; color: var(--text-secondary); padding: 8px 14px; font-size: 11px; font-weight: 600;
  letter-spacing: .08em; text-transform: uppercase; border-bottom: 2px solid transparent; }}
.tab.sel {{ color: var(--text-primary); border-bottom-color: var(--brand-stroke); }}
.seg {{ display: inline-flex; border: 1px solid var(--control-rim); border-radius: {r['xs']}px; overflow: hidden; }}
.seg button {{ height: {size['toggle']['md']}px; padding: 0 12px; background: var(--control-body); color: var(--text-secondary);
  border: 0; border-right: 1px solid var(--control-rim); font-size: 11px; font-weight: 600; letter-spacing: .08em; text-transform: uppercase; }}
.seg button:last-child {{ border-right: 0; }}
.seg button.sel {{ background: var(--brand-fill); color: var(--text-onBrand); }}
.slider {{ position: relative; width: 240px; height: {size['hitTargetMin']}px; }}
.slider .track {{ position: absolute; top: 10px; left: 0; right: 0; height: {size['slider']['trackThickness']}px;
  background: var(--bg-well); border-radius: 2px; }}
.slider .fill {{ height: 100%; background: var(--brand-stroke); border-radius: 2px; }}
.slider .thumb {{ position: absolute; top: 6px; width: {size['slider']['thumbLength']}px; height: {size['slider']['thumbThickness']}px;
  margin-left: -10px; border-radius: {r['xs']}px; background: linear-gradient(var(--control-bodyTop), var(--control-body));
  border: 1px solid var(--control-rim); box-shadow: var(--raised-shadow); }}
.readout {{ background: var(--bg-well); border-radius: {r['xs']}px; padding: 6px 10px; min-width: 112px; display: flex;
  justify-content: space-between; align-items: baseline; gap: 8px;
  box-shadow: var(--well-shadow); }}
.readout .v {{ font-family: var(--mono); font-size: {st['valueLarge']['size']}px; color: var(--text-primary); }}
.readout .u {{ font-size: 10px; color: var(--text-tertiary); text-transform: uppercase; letter-spacing: .06em; font-weight: 600; }}
.readout.over .v, .readout.over .u, .over-text {{ color: var(--state-danger); }}
.tooltip {{ max-width: {size['tooltipMaxWidth']}px; background: var(--bg-overlay); border: 1px solid var(--border-overlay);
  border-radius: {r['sm']}px; padding: 8px 10px; box-shadow: var(--overlay-shadow); }}
.meter-group {{ display: flex; gap: 6px; align-items: flex-start; background: var(--bg-section); padding: 16px; border-radius: {r['md']}px; }}
.mcol {{ display: flex; flex-direction: column; gap: 4px; }}
.meter {{ display: flex; flex-direction: column; gap: 2px; }}
.over-cap {{ border-radius: 1px; }}
.meter-well {{ position: relative; background: var(--meter-track); overflow: hidden; border-radius: 1px;
  box-shadow: var(--well-shadow); }}
.scale {{ position: relative; width: 36px; }}
.tick {{ position: absolute; left: 0; right: 0; height: 0; }}
.tick::before, .tick::after {{ content: ""; position: absolute; top: 0; width: 5px; border-top: 1px solid var(--meter-tick); }}
.tick::before {{ left: 0; }}
.tick::after {{ right: 0; }}
.tick span {{ position: absolute; top: -6px; left: 0; right: 0; text-align: center; font-family: var(--mono); font-size: 10px;
  color: var(--meter-tickLabel); line-height: 12px; }}
.note {{ max-width: 420px; color: var(--text-secondary); }}
.header {{ height: {size['headerHeight']}px; background: var(--bg-chassis); border-bottom: 1px solid var(--border-divider);
  display: flex; align-items: center; justify-content: space-between; padding: 0 {size['windowPadding']}px; border-radius: {r['md']}px {r['md']}px 0 0; }}
.hl, .hr {{ display: flex; align-items: center; gap: 10px; }}
.product {{ font-size: {st['productName']['size']}px; font-weight: 700; letter-spacing: {st['productName']['tracking']}em; text-transform: uppercase; }}
.preset {{ display: flex; align-items: center; gap: 8px; background: var(--bg-well); border-radius: {r['xs']}px; padding: 0 4px;
  height: {size['button']['sm']}px; min-width: 180px; justify-content: space-between; box-shadow: var(--well-shadow); }}
.arrow {{ background: none; border: 0; color: var(--text-secondary); font-size: 16px; width: {size['hitTargetMin']}px; height: {size['hitTargetMin']}px; }}
"""
    fonts = " and ".join(f"{f['name']} ({f['license']})" for f in fam.values())
    body = (
        f"<main><h1>{html.escape(t['name'])} theme</h1>"
        f"<div class='mono dim'>theme {t['id']} v{t['version']} · contract {contract['contractVersion']} · "
        f"generated from themes/{t['id']}/tokens.json by scripts/render_preview.py — do not edit</div>"
        + section("Header (product chrome)", header_bar(t))
        + section("Colour", swatches(t))
        + section("Contrast (WCAG 2.x)", contrast_table(t, contract))
        + section("Typography", type_specimens(t)
                  + f"<p class='dim'>{html.escape(fonts)} are embedded in plugins; this page falls back "
                    "to system fonts if they are not installed.</p>")
        + section("Space, radius, elevation", spacing(t))
        + section("Controls", controls(t))
        + section("Meters", meters(t))
        + "</main>"
    )
    return ("<!doctype html>\n<html lang=\"en\"><head><meta charset=\"utf-8\">"
            f"<title>{html.escape(t['name'])} theme v{t['version']}</title>"
            f"<style>{css}</style></head><body>{body}</body></html>\n")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--theme", action="append", help="theme id (repeatable); default: all themes")
    args = ap.parse_args()
    contract = load_contract()
    PREVIEWS.mkdir(exist_ok=True)
    for theme_id in args.theme or list_themes():
        out = preview_path(theme_id)
        out.write_text(render(load_theme(theme_id), contract), encoding="utf-8", newline="\n")
        print(f"wrote {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
