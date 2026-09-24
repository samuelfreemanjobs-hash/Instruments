#!/usr/bin/env python3
"""Night Circuit factory bank integrity — runs NightCircuitTests and writes a QA report."""

from __future__ import annotations

import argparse
import datetime as dt
import os
import platform
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_BUILD_DIR = REPO_ROOT / "build"
TEST_BINARY = DEFAULT_BUILD_DIR / "ProphetRev2Trap" / "NightCircuitTests"
REPORTS_DIR = Path(__file__).resolve().parent / "reports"

EXPECTED_COUNTS = {
    "Bass": 195,
    "Synth": 245,
    "Lead": 196,
    "Pad": 196,
    "Pluck/Keys": 196,
}
TOTAL_FACTORY = sum(EXPECTED_COUNTS.values())


def run(cmd: list[str], *, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        cmd,
        cwd=cwd or REPO_ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def compiler_version() -> str:
    for exe in ("g++-12", "g++"):
        if run(["which", exe]).returncode == 0:
            return run([exe, "--version"]).stdout.splitlines()[0]
    return "unknown"


def git_head() -> str:
    r = run(["git", "rev-parse", "--short", "HEAD"])
    return r.stdout.strip() if r.returncode == 0 else "unknown"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--test-binary",
        type=Path,
        default=TEST_BINARY,
        help="Path to NightCircuitTests (build first)",
    )
    parser.add_argument(
        "--skip-report",
        action="store_true",
        help="Only run tests; do not write markdown report",
    )
    args = parser.parse_args()

    if not args.test_binary.is_file():
        print(f"Missing test binary: {args.test_binary}", file=sys.stderr)
        print(
            "Build with:\n"
            "  cmake -B build -DCMAKE_BUILD_TYPE=Release "
            "-DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12\n"
            "  cmake --build build -j --target NightCircuitTests",
            file=sys.stderr,
        )
        return 2

    proc = run([str(args.test_binary)])
    passed = proc.returncode == 0
    today = dt.date.today().isoformat()

    if not args.skip_report:
        REPORTS_DIR.mkdir(parents=True, exist_ok=True)
        report_path = REPORTS_DIR / f"{today}-bank-integrity.md"
        lines = [
            f"# Night Circuit — bank integrity ({today})",
            "",
            "## Environment",
            f"- OS: {platform.platform()}",
            f"- Compiler: {compiler_version()}",
            f"- Git: `{git_head()}`",
            "",
            "## Acceptance criteria (automated)",
            f"- Total factory presets: **{TOTAL_FACTORY}** — "
            + ("**PASS**" if passed else "**FAIL** (see test output)"),
        ]
        for cat, n in EXPECTED_COUNTS.items():
            lines.append(f"- {cat}: {n} — **PASS** (validated via NightCircuitTests)" if passed else f"- {cat}: {n} — **NOT VERIFIED**")
        lines.extend(
            [
                "- Unique (category, name): "
                + ("**PASS**" if passed else "**NOT VERIFIED**"),
                "- Parameter finite + in-range: "
                + ("**PASS**" if passed else "**NOT VERIFIED**"),
                "",
                "## NightCircuitTests",
                "```",
                (proc.stdout + proc.stderr).strip() or "(no output)",
                f"exit code: {proc.returncode}",
                "```",
                "",
                "## Release recommendation",
                "**PASS** (bank integrity only)" if passed else "**FAIL** — fix preset factory before musical QA",
                "",
                "### NOT TESTED in this pass",
                "- Render-all-1028 automated audio",
                "- Listening review by category",
                "- Host project recall / DAW matrix",
                "- pluginval (run separately per NIGHT_CIRCUIT_QA_SUBAGENT.md)",
                "",
            ]
        )
        report_path.write_text("\n".join(lines), encoding="utf-8")
        print(f"Wrote {report_path}")

    if proc.stdout:
        print(proc.stdout, end="")
    if proc.stderr:
        print(proc.stderr, end="", file=sys.stderr)

    return proc.returncode


if __name__ == "__main__":
    raise SystemExit(main())
