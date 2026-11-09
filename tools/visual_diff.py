#!/usr/bin/env python3
"""Compare GUI render PNGs (+ text sidecars) against committed visual goldens.

Exit 0 when every image matches within tolerance and every text sidecar matches.
Exit 1 on any failure (missing golden, size mismatch, pixel delta, text/ellipsis).

Usage:
  python3 tools/visual_diff.py --actual build/.../gui-renders \\
      --golden tests/visual/golden/factory-gui --out build/visual-diff

  python3 tools/visual_diff.py ... --update   # re-baseline (explicit only)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

try:
    from PIL import Image, ImageChops, ImageDraw
except ImportError as exc:  # pragma: no cover
    sys.stderr.write(
        "visual_diff requires Pillow. Install with: pip install Pillow\n"
    )
    raise SystemExit(2) from exc

DEFAULT_CHANNEL_TOLERANCE = 8
DEFAULT_MAX_DIFF_RATIO = 0.002  # 0.2%


@dataclass
class ImageResult:
    rel: str
    ok: bool
    reason: str = ""
    diff_pixels: int = 0
    total_pixels: int = 0
    diff_ratio: float = 0.0
    diff_path: str | None = None


@dataclass
class TextResult:
    rel: str
    ok: bool
    reason: str = ""


@dataclass
class Report:
    image_results: list[ImageResult] = field(default_factory=list)
    text_results: list[TextResult] = field(default_factory=list)
    manifest_ok: bool = True
    manifest_reason: str = ""
    missing_goldens: list[str] = field(default_factory=list)
    extra_actuals: list[str] = field(default_factory=list)

    @property
    def ok(self) -> bool:
        return (
            self.manifest_ok
            and not self.missing_goldens
            and all(r.ok for r in self.image_results)
            and all(r.ok for r in self.text_results)
        )


def _load_png(path: Path) -> Image.Image:
    with Image.open(path) as img:
        return img.convert("RGBA")


def compare_images(
    golden: Path,
    actual: Path,
    *,
    channel_tol: int,
    max_diff_ratio: float,
    diff_out: Path | None,
) -> ImageResult:
    rel = actual.name
    g = _load_png(golden)
    a = _load_png(actual)
    if g.size != a.size:
        return ImageResult(
            rel=rel,
            ok=False,
            reason=f"size mismatch golden={g.size} actual={a.size}",
            total_pixels=g.size[0] * g.size[1],
        )

    gw, gh = g.size
    total = gw * gh
    gp = g.load()
    ap = a.load()
    diff_count = 0
    # Highlight mask (red) for pixels over tolerance
    highlight = Image.new("RGBA", g.size, (0, 0, 0, 0))
    hp = highlight.load()
    for y in range(gh):
        for x in range(gw):
            gr, gg, gb, ga = gp[x, y]
            ar, ag, ab, aa = ap[x, y]
            if (
                abs(gr - ar) > channel_tol
                or abs(gg - ag) > channel_tol
                or abs(gb - ab) > channel_tol
                or abs(ga - aa) > channel_tol
            ):
                diff_count += 1
                hp[x, y] = (255, 0, 0, 200)

    ratio = diff_count / total if total else 0.0
    ok = ratio <= max_diff_ratio
    reason = ""
    if not ok:
        reason = (
            f"{diff_count}/{total} pixels differ "
            f"({ratio * 100:.4f}% > {max_diff_ratio * 100:.4f}%, tol={channel_tol})"
        )

    diff_path = None
    if diff_out is not None and (not ok or diff_count > 0):
        diff_out.parent.mkdir(parents=True, exist_ok=True)
        # Side-by-side: golden | actual | highlighted diff over actual
        pad = 8
        canvas = Image.new(
            "RGBA",
            (gw * 3 + pad * 4, gh + pad * 2),
            (32, 32, 32, 255),
        )
        canvas.paste(g, (pad, pad))
        canvas.paste(a, (pad * 2 + gw, pad))
        blended = Image.alpha_composite(a, highlight)
        canvas.paste(blended, (pad * 3 + gw * 2, pad))
        draw = ImageDraw.Draw(canvas)
        draw.text((pad, 0), "golden", fill=(200, 200, 200, 255))
        draw.text((pad * 2 + gw, 0), "actual", fill=(200, 200, 200, 255))
        draw.text((pad * 3 + gw * 2, 0), "diff", fill=(255, 120, 120, 255))
        canvas.save(diff_out)
        diff_path = str(diff_out)

    return ImageResult(
        rel=rel,
        ok=ok,
        reason=reason,
        diff_pixels=diff_count,
        total_pixels=total,
        diff_ratio=ratio,
        diff_path=diff_path,
    )


def _bounds_inside(bounds: dict[str, Any], width: float, height: float) -> bool:
    x = float(bounds.get("x", 0))
    y = float(bounds.get("y", 0))
    w = float(bounds.get("w", 0))
    h = float(bounds.get("h", 0))
    eps = 0.51
    return (
        x >= -eps
        and y >= -eps
        and (x + w) <= width + eps
        and (y + h) <= height + eps
        and w >= 0
        and h >= 0
    )


def compare_text_sidecars(golden: Path, actual: Path) -> TextResult:
    rel = actual.name
    try:
        g = json.loads(golden.read_text(encoding="utf-8"))
        a = json.loads(actual.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return TextResult(rel=rel, ok=False, reason=f"json error: {exc}")

    g_texts = g.get("texts", [])
    a_texts = a.get("texts", [])
    if len(g_texts) != len(a_texts):
        return TextResult(
            rel=rel,
            ok=False,
            reason=f"text count mismatch golden={len(g_texts)} actual={len(a_texts)}",
        )

    width = float(a.get("width", g.get("width", 0)))
    height = float(a.get("height", g.get("height", 0)))

    for i, (gt, at) in enumerate(zip(g_texts, a_texts)):
        gs = gt.get("string", "")
        ats = at.get("string", "")
        if "..." in ats or "…" in ats:
            return TextResult(
                rel=rel,
                ok=False,
                reason=f"ellipsis detected in actual texts[{i}]: {ats!r}",
            )
        if "..." in gs or "…" in gs:
            return TextResult(
                rel=rel,
                ok=False,
                reason=f"ellipsis in golden texts[{i}] (re-baseline required): {gs!r}",
            )
        if gs != ats:
            return TextResult(
                rel=rel,
                ok=False,
                reason=f"string mismatch texts[{i}]: golden={gs!r} actual={ats!r}",
            )
        bounds = at.get("bounds", {})
        if not _bounds_inside(bounds, width, height):
            return TextResult(
                rel=rel,
                ok=False,
                reason=(
                    f"bounds outside component texts[{i}]: "
                    f"{bounds} not in 0..{width}x{height}"
                ),
            )

    return TextResult(rel=rel, ok=True)


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 16), b""):
            h.update(chunk)
    return h.hexdigest()


def load_manifest(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def check_manifest(
    manifest: dict[str, Any],
    actual_root: Path,
    *,
    expected_juce: str | None,
) -> tuple[bool, str]:
    """Fail closed if golden MANIFEST JUCE/font versions disagree with the build."""
    build_infos = list(actual_root.glob("**/BUILD_INFO.json"))
    # Prefer root BUILD_INFO.json written once per theme binary run (last wins).
    root_info = actual_root / "BUILD_INFO.json"
    info: dict[str, Any] | None = None
    if root_info.is_file():
        info = json.loads(root_info.read_text(encoding="utf-8"))
    elif build_infos:
        info = json.loads(build_infos[0].read_text(encoding="utf-8"))

    man_juce = str(manifest.get("juce_tag") or manifest.get("juce_version") or "")
    if expected_juce:
        if man_juce != expected_juce and expected_juce not in man_juce:
            return False, (
                f"MANIFEST juce_tag={man_juce!r} != expected {expected_juce!r} "
                "(conscious re-baseline required)"
            )

    if info is not None:
        build_juce = str(info.get("juce_version", ""))
        # SystemStats::getJUCEVersion() returns e.g. "JUCE v8.0.15"
        if man_juce and build_juce and man_juce not in build_juce and build_juce not in man_juce:
            # Also accept bare tag "8.0.15" vs "JUCE v8.0.15"
            bare_build = build_juce.replace("JUCE v", "").strip()
            bare_man = man_juce.replace("JUCE v", "").strip()
            if bare_build != bare_man:
                return False, (
                    f"MANIFEST juce={man_juce!r} != build {build_juce!r} "
                    "(conscious re-baseline required)"
                )

        man_os = str(manifest.get("os", ""))
        build_os = str(info.get("os", ""))
        if man_os and build_os and man_os.lower() != build_os.lower():
            return False, f"MANIFEST os={man_os!r} != build os={build_os!r}"

    # Font hashes: every listed font must still match repo bytes.
    fonts = manifest.get("fonts") or {}
    repo_root = Path(__file__).resolve().parents[1]
    for rel, expected_hash in fonts.items():
        fpath = repo_root / rel
        if not fpath.is_file():
            return False, f"MANIFEST font missing: {rel}"
        actual_hash = sha256_file(fpath)
        if actual_hash != expected_hash:
            return False, (
                f"MANIFEST font hash mismatch for {rel}: "
                f"manifest={expected_hash} actual={actual_hash}"
            )

    return True, ""


def iter_theme_dirs(root: Path) -> list[Path]:
    if not root.is_dir():
        return []
    themes = [p for p in sorted(root.iterdir()) if p.is_dir() and not p.name.startswith(".")]
    return themes


def collect_stems(theme_dir: Path) -> set[str]:
    stems: set[str] = set()
    for png in theme_dir.glob("*.png"):
        stems.add(png.stem)
    return stems


def update_goldens(actual_root: Path, golden_root: Path, manifest: dict[str, Any] | None) -> None:
    golden_root.mkdir(parents=True, exist_ok=True)
    for theme_dir in iter_theme_dirs(actual_root):
        dest = golden_root / theme_dir.name
        if dest.exists():
            shutil.rmtree(dest)
        shutil.copytree(theme_dir, dest)
    # Copy BUILD_INFO if present
    bi = actual_root / "BUILD_INFO.json"
    if bi.is_file():
        shutil.copy2(bi, golden_root / "BUILD_INFO.json")
    if manifest is not None:
        (golden_root / "MANIFEST.json").write_text(
            json.dumps(manifest, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )


def build_default_manifest(actual_root: Path, juce_tag: str) -> dict[str, Any]:
    repo_root = Path(__file__).resolve().parents[1]
    font_files = [
        "vst/factory-lib/gui/fonts/Inter/Inter-Regular.ttf",
        "vst/factory-lib/gui/fonts/Inter/Inter-Medium.ttf",
        "vst/factory-lib/gui/fonts/Inter/Inter-SemiBold.ttf",
        "vst/factory-lib/gui/fonts/Inter/Inter-Bold.ttf",
        "vst/factory-lib/gui/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf",
        "vst/factory-lib/gui/fonts/JetBrainsMono/JetBrainsMono-Medium.ttf",
        "vst/factory-lib/gui/fonts/JetBrainsMono/JetBrainsMono-SemiBold.ttf",
        "vst/factory-lib/gui/fonts/JetBrainsMono/JetBrainsMono-Bold.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexSans/IBMPlexSans-Regular.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexSans/IBMPlexSans-Medium.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexSans/IBMPlexSans-SemiBold.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexSans/IBMPlexSans-Bold.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexMono/IBMPlexMono-Regular.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexMono/IBMPlexMono-Medium.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexMono/IBMPlexMono-SemiBold.ttf",
        "vst/factory-lib/gui/fonts/IBMPlexMono/IBMPlexMono-Bold.ttf",
    ]
    fonts = {}
    for rel in font_files:
        p = repo_root / rel
        if p.is_file():
            fonts[rel] = sha256_file(p)

    build_juce = juce_tag
    bi = actual_root / "BUILD_INFO.json"
    os_name = "Linux"
    if bi.is_file():
        info = json.loads(bi.read_text(encoding="utf-8"))
        build_juce = str(info.get("juce_version", juce_tag))
        os_name = str(info.get("os", "Linux"))

    themes = [p.name for p in iter_theme_dirs(actual_root)]
    return {
        "suite": "factory-gui",
        "juce_tag": juce_tag,
        "juce_version": build_juce,
        "os": os_name,
        "channel_tolerance": DEFAULT_CHANNEL_TOLERANCE,
        "max_diff_ratio": DEFAULT_MAX_DIFF_RATIO,
        "scales": ["1x", "2x"],
        "themes": themes,
        "fonts": fonts,
        "notes": (
            "Golden cell = ubuntu-latest + JUCE 8.0.15. "
            "Re-baseline only with visual_diff.py --update in the same PR as the UI change."
        ),
    }


def run_compare(
    actual_root: Path,
    golden_root: Path,
    out_dir: Path,
    *,
    channel_tol: int,
    max_diff_ratio: float,
    expected_juce: str | None,
    update: bool,
) -> Report:
    report = Report()
    out_dir.mkdir(parents=True, exist_ok=True)
    diffs_dir = out_dir / "diffs"
    diffs_dir.mkdir(parents=True, exist_ok=True)

    manifest_path = golden_root / "MANIFEST.json"

    if update:
        manifest = build_default_manifest(actual_root, expected_juce or "8.0.15")
        update_goldens(actual_root, golden_root, manifest)
        report.manifest_ok = True
        report.manifest_reason = "updated"
        # After update, compare should be identical — fall through.

    if not manifest_path.is_file():
        report.manifest_ok = False
        report.manifest_reason = f"missing MANIFEST.json at {manifest_path}"
        if not update:
            return report
    else:
        manifest = load_manifest(manifest_path)
        ok, reason = check_manifest(manifest, actual_root, expected_juce=expected_juce)
        report.manifest_ok = ok
        report.manifest_reason = reason

    if not actual_root.is_dir():
        report.missing_goldens.append(f"(actual root missing: {actual_root})")
        return report

    golden_themes = {p.name: p for p in iter_theme_dirs(golden_root)}
    actual_themes = {p.name: p for p in iter_theme_dirs(actual_root)}

    for theme, actual_dir in actual_themes.items():
        golden_dir = golden_themes.get(theme)
        actual_stems = collect_stems(actual_dir)
        golden_stems = collect_stems(golden_dir) if golden_dir else set()

        for stem in sorted(actual_stems):
            apng = actual_dir / f"{stem}.png"
            gpng = (golden_dir / f"{stem}.png") if golden_dir else None
            if gpng is None or not gpng.is_file():
                report.missing_goldens.append(f"{theme}/{stem}.png")
                report.image_results.append(
                    ImageResult(rel=f"{theme}/{stem}.png", ok=False, reason="missing golden")
                )
                continue
            diff_path = diffs_dir / theme / f"{stem}.diff.png"
            img_res = compare_images(
                gpng,
                apng,
                channel_tol=channel_tol,
                max_diff_ratio=max_diff_ratio,
                diff_out=diff_path,
            )
            img_res.rel = f"{theme}/{stem}.png"
            report.image_results.append(img_res)

            ajson = actual_dir / f"{stem}.json"
            gjson = golden_dir / f"{stem}.json"
            if ajson.is_file() and gjson.is_file():
                tres = compare_text_sidecars(gjson, ajson)
                tres.rel = f"{theme}/{stem}.json"
                report.text_results.append(tres)
            elif ajson.is_file() and not gjson.is_file():
                report.text_results.append(
                    TextResult(
                        rel=f"{theme}/{stem}.json",
                        ok=False,
                        reason="missing golden sidecar",
                    )
                )
            elif gjson.is_file() and not ajson.is_file():
                report.text_results.append(
                    TextResult(
                        rel=f"{theme}/{stem}.json",
                        ok=False,
                        reason="missing actual sidecar",
                    )
                )

        for stem in sorted(golden_stems - actual_stems):
            report.extra_actuals.append(f"{theme}/{stem}.png (golden present, actual missing)")
            report.image_results.append(
                ImageResult(
                    rel=f"{theme}/{stem}.png",
                    ok=False,
                    reason="missing actual (golden present)",
                )
            )

    for theme in sorted(set(golden_themes) - set(actual_themes)):
        report.missing_goldens.append(f"(theme {theme} missing from actual)")

    return report


def write_html_report(report: Report, out_dir: Path) -> Path:
    rows: list[str] = []
    for r in report.image_results:
        status = "PASS" if r.ok else "FAIL"
        color = "#1a7f37" if r.ok else "#cf222e"
        diff_link = ""
        if r.diff_path:
            rel = os.path.relpath(r.diff_path, out_dir)
            diff_link = f'<a href="{rel}"><img src="{rel}" alt="diff" style="max-height:120px"/></a>'
        rows.append(
            f"<tr><td style='color:{color};font-weight:bold'>{status}</td>"
            f"<td>{r.rel}</td><td>{r.reason}</td><td>{diff_link}</td></tr>"
        )
    for r in report.text_results:
        status = "PASS" if r.ok else "FAIL"
        color = "#1a7f37" if r.ok else "#cf222e"
        rows.append(
            f"<tr><td style='color:{color};font-weight:bold'>{status}</td>"
            f"<td>{r.rel}</td><td>{r.reason}</td><td></td></tr>"
        )

    overall = "PASS" if report.ok else "FAIL"
    html = f"""<!DOCTYPE html>
<html><head><meta charset="utf-8"/><title>visual_diff {overall}</title>
<style>
body {{ font-family: ui-sans-serif, system-ui, sans-serif; margin: 24px; background: #0d1117; color: #e6edf3; }}
table {{ border-collapse: collapse; width: 100%; }}
th, td {{ border: 1px solid #30363d; padding: 8px; vertical-align: top; }}
th {{ background: #161b22; text-align: left; }}
h1 {{ color: {'#3fb950' if report.ok else '#f85149'}; }}
</style></head><body>
<h1>visual_diff: {overall}</h1>
<p>manifest: {'ok' if report.manifest_ok else 'FAIL — ' + report.manifest_reason}</p>
<p>missing goldens: {len(report.missing_goldens)}</p>
<table>
<thead><tr><th>Status</th><th>File</th><th>Reason</th><th>Diff</th></tr></thead>
<tbody>
{''.join(rows)}
</tbody></table>
</body></html>
"""
    path = out_dir / "report.html"
    path.write_text(html, encoding="utf-8")
    return path


def write_summary_md(report: Report) -> str:
    fails = [r for r in report.image_results if not r.ok] + [
        r for r in report.text_results if not r.ok
    ]
    lines = [
        f"### visual_diff: {'PASS' if report.ok else 'FAIL'}",
        f"- images checked: {len(report.image_results)}",
        f"- text sidecars checked: {len(report.text_results)}",
        f"- missing goldens: {len(report.missing_goldens)}",
        f"- manifest: {'ok' if report.manifest_ok else report.manifest_reason}",
    ]
    if fails:
        lines.append("- failures:")
        for r in fails[:20]:
            reason = getattr(r, "reason", "")
            lines.append(f"  - `{r.rel}`: {reason}")
        if len(fails) > 20:
            lines.append(f"  - …and {len(fails) - 20} more")
    return "\n".join(lines) + "\n"


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--actual", type=Path, required=True, help="Render output root (gui-renders/)")
    p.add_argument("--golden", type=Path, required=True, help="Golden suite root")
    p.add_argument("--out", type=Path, default=Path("build/visual-diff"), help="Diff report directory")
    p.add_argument("--channel-tol", type=int, default=DEFAULT_CHANNEL_TOLERANCE)
    p.add_argument("--max-diff-ratio", type=float, default=DEFAULT_MAX_DIFF_RATIO)
    p.add_argument(
        "--expected-juce",
        default="8.0.15",
        help="JUCE tag that must match MANIFEST (golden cell)",
    )
    p.add_argument(
        "--update",
        action="store_true",
        help="Re-baseline goldens from --actual (explicit flag required)",
    )
    p.add_argument("--summary", type=Path, default=None, help="Write markdown summary here")
    return p.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    report = run_compare(
        args.actual,
        args.golden,
        args.out,
        channel_tol=args.channel_tol,
        max_diff_ratio=args.max_diff_ratio,
        expected_juce=args.expected_juce,
        update=args.update,
    )
    html = write_html_report(report, args.out)
    summary = write_summary_md(report)
    (args.out / "summary.md").write_text(summary, encoding="utf-8")
    if args.summary:
        args.summary.write_text(summary, encoding="utf-8")

    gh_summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if gh_summary:
        with open(gh_summary, "a", encoding="utf-8") as fh:
            fh.write(summary)

    sys.stdout.write(summary)
    sys.stdout.write(f"HTML report: {html}\n")
    return 0 if report.ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
