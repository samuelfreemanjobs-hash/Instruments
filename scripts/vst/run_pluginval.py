#!/usr/bin/env python3
"""Run Tracktion pluginval against VST3 bundles and summarize failures for CI or agents."""

from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
import time
import urllib.request
import zipfile
from dataclasses import dataclass
from pathlib import Path

DEFAULT_VERSION = "1.0.4"
DEFAULT_LINUX_URL = (
    f"https://github.com/Tracktion/pluginval/releases/download/v{DEFAULT_VERSION}/"
    "pluginval_Linux.zip"
)
FAIL_MARKERS = re.compile(
    r"\b(FAILED|FAILURE|ERROR|CRASH|Segmentation fault|assertion failed)\b",
    re.IGNORECASE,
)


@dataclass
class ValidationResult:
    plugin: Path
    exit_code: int
    output: str

    @property
    def passed(self) -> bool:
        if self.exit_code != 0:
            return False
        tail = self.output.strip().splitlines()[-1] if self.output.strip() else ""
        if tail.upper() == "SUCCESS":
            return True
        return not FAIL_MARKERS.search(self.output)


def repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def cache_dir() -> Path:
    env = os.environ.get("PLUGINVAL_CACHE_DIR")
    if env:
        return Path(env)
    return repo_root() / "build" / "tools" / "pluginval"


def ensure_pluginval(force_download: bool = False) -> Path:
    explicit = os.environ.get("PLUGINVAL_BIN")
    if explicit:
        path = Path(explicit)
        if not path.is_file() or not os.access(path, os.X_OK):
            raise SystemExit(f"PLUGINVAL_BIN is not executable: {path}")
        return path

    cache = cache_dir()
    binary = cache / "pluginval"
    if binary.is_file() and os.access(binary, os.X_OK) and not force_download:
        return binary

    url = os.environ.get("PLUGINVAL_DOWNLOAD_URL", DEFAULT_LINUX_URL)
    cache.mkdir(parents=True, exist_ok=True)
    zip_path = cache / "pluginval.zip"

    print(f"Downloading pluginval from {url}", file=sys.stderr)
    urllib.request.urlretrieve(url, zip_path)

    with zipfile.ZipFile(zip_path) as zf:
        members = [m for m in zf.namelist() if Path(m).name == "pluginval"]
        if not members:
            raise SystemExit(f"No pluginval binary inside {zip_path}")
        with zf.open(members[0]) as src, open(binary, "wb") as dst:
            shutil.copyfileobj(src, dst)
    binary.chmod(0o755)
    zip_path.unlink(missing_ok=True)
    return binary


def discover_vst3_plugins(search_roots: list[Path]) -> list[Path]:
    found: list[Path] = []
    for root in search_roots:
        if not root.exists():
            continue
        if root.suffix == ".vst3" and root.is_dir():
            found.append(root.resolve())
            continue
        for path in sorted(root.rglob("*.vst3")):
            if path.is_dir():
                found.append(path.resolve())
    # De-dupe while preserving order
    seen: set[Path] = set()
    unique: list[Path] = []
    for p in found:
        if p not in seen:
            seen.add(p)
            unique.append(p)
    return unique


def pluginval_extra_cli_args() -> list[str]:
    """Headless CI runners cannot open plugin editor windows reliably."""
    if os.environ.get("PLUGINVAL_SKIP_GUI", "").lower() in ("1", "true", "yes"):
        return ["--skip-gui-tests"]
    if os.environ.get("CI", "").lower() in ("1", "true", "yes"):
        return ["--skip-gui-tests"]
    return []


def run_pluginval(
    binary: Path,
    plugin: Path,
    *,
    strictness: int,
    timeout_ms: int,
    validate_in_process: bool,
    extra_args: list[str],
) -> ValidationResult:
    cmd = [str(binary)]
    cmd.extend(pluginval_extra_cli_args())
    cmd.extend(
        [
            "--strictness-level",
            str(strictness),
            "--timeout-ms",
            str(timeout_ms),
            "--file",
            str(plugin),
        ]
    )
    if validate_in_process:
        cmd.insert(1, "--validate-in-process")
    cmd.extend(extra_args)

    proc = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=False,
        timeout=max(timeout_ms // 1000 + 60, 120),
    )
    output = (proc.stdout or "") + (proc.stderr or "")
    return ValidationResult(plugin=plugin, exit_code=proc.returncode, output=output)


def summarize_failure(result: ValidationResult, max_lines: int = 40) -> str:
    lines = result.output.splitlines()
    interesting = [ln for ln in lines if FAIL_MARKERS.search(ln)]
    if not interesting:
        interesting = lines[-max_lines:]
    body = "\n".join(interesting[-max_lines:])
    return (
        f"Plugin: {result.plugin}\n"
        f"Exit code: {result.exit_code}\n"
        f"--- pluginval output (tail / failures) ---\n{body}\n"
    )


def print_agent_summary(results: list[ValidationResult]) -> int:
    failed = [r for r in results if not r.passed]
    if not failed:
        print("pluginval: all plugins passed.")
        for r in results:
            print(f"  OK  {r.plugin}")
        return 0

    print("pluginval: FAILURES detected — summary for debugging:", file=sys.stderr)
    for r in failed:
        print(summarize_failure(r), file=sys.stderr)
    return 1


def watch_and_validate(
    binary: Path,
    watch_dir: Path,
    *,
    strictness: int,
    timeout_ms: int,
    validate_in_process: bool,
    poll_seconds: float,
    extra_args: list[str],
) -> None:
    print(f"Watching {watch_dir} for .vst3 changes (poll {poll_seconds}s). Ctrl+C to stop.")
    last_mtime: dict[Path, float] = {}
    while True:
        plugins = discover_vst3_plugins([watch_dir])
        for plugin in plugins:
            mtime = plugin.stat().st_mtime
            if last_mtime.get(plugin) == mtime:
                continue
            last_mtime[plugin] = mtime
            print(f"\n=== Validating {plugin} ===")
            result = run_pluginval(
                binary,
                plugin,
                strictness=strictness,
                timeout_ms=timeout_ms,
                validate_in_process=validate_in_process,
                extra_args=extra_args,
            )
            sys.stdout.write(result.output)
            if result.passed:
                print(f"PASS {plugin}")
            else:
                print(summarize_failure(result), file=sys.stderr)
        time.sleep(poll_seconds)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--plugin",
        action="append",
        dest="plugins",
        type=Path,
        help="Path to a .vst3 bundle (repeatable)",
    )
    parser.add_argument(
        "--discover",
        action="append",
        dest="discover_roots",
        type=Path,
        help="Directory to scan for *.vst3 bundles (repeatable)",
    )
    parser.add_argument(
        "--default-artefacts",
        action="store_true",
        help="Validate JD Upgraded + Wave909 VST3 under build/*_artefacts/Release/VST3/",
    )
    parser.add_argument(
        "--factory-artefacts",
        action="store_true",
        help="Discover and validate all Plugin Factory OS VST3 bundles",
    )
    parser.add_argument("--strictness-level", type=int, default=5)
    parser.add_argument("--timeout-ms", type=int, default=120_000)
    parser.add_argument(
        "--validate-in-process",
        action=argparse.BooleanOptionalAction,
        default=True,
        help="Use in-process validation (default: true, matches CI)",
    )
    parser.add_argument(
        "--watch",
        type=Path,
        metavar="DIR",
        help="Poll DIR for new/changed .vst3 and re-run pluginval",
    )
    parser.add_argument("--poll-seconds", type=float, default=2.0)
    parser.add_argument(
        "--install-only",
        action="store_true",
        help="Download/cache pluginval binary and exit",
    )
    parser.add_argument(
        "pluginval_extra",
        nargs="*",
        help="Extra args forwarded to pluginval after --file",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    binary = ensure_pluginval()
    if args.install_only:
        print(binary)
        return 0

    roots: list[Path] = list(args.discover_roots or [])
    if args.default_artefacts:
        root = repo_root()
        roots.extend(
            [
                root / "build" / "JDUpgraded_artefacts" / "Release" / "VST3",
                root / "build" / "Wave909" / "Wave909_artefacts" / "Release" / "VST3",
                root / "MyFirstPlugin" / "build" / "MyFirstPlugin_artefacts" / "Release" / "VST3",
            ]
        )
    if args.factory_artefacts:
        factory_build = repo_root() / "plugin-factory" / "build"
        roots.append(factory_build / "plugins")
    if not roots and not args.plugins:
        roots = [repo_root() / "build" / "JDUpgraded_artefacts" / "Release" / "VST3"]

    plugins: list[Path] = []
    if args.plugins:
        plugins.extend(args.plugins)
    plugins.extend(discover_vst3_plugins(roots))

    if args.watch:
        watch_and_validate(
            binary,
            args.watch,
            strictness=args.strictness_level,
            timeout_ms=args.timeout_ms,
            validate_in_process=args.validate_in_process,
            poll_seconds=args.poll_seconds,
            extra_args=args.pluginval_extra,
        )
        return 0

    if not plugins:
        print(
            "No .vst3 plugins found. Build first or pass --plugin / --discover.",
            file=sys.stderr,
        )
        return 2

    results: list[ValidationResult] = []
    for plugin in plugins:
        if not plugin.is_dir():
            print(f"Skipping missing plugin bundle: {plugin}", file=sys.stderr)
            continue
        print(f"Running pluginval on {plugin} …", file=sys.stderr)
        results.append(
            run_pluginval(
                binary,
                plugin,
                strictness=args.strictness_level,
                timeout_ms=args.timeout_ms,
                validate_in_process=args.validate_in_process,
                extra_args=args.pluginval_extra,
            )
        )
        sys.stdout.write(results[-1].output)

    if not results:
        return 2
    return print_agent_summary(results)


if __name__ == "__main__":
    raise SystemExit(main())
