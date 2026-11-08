#!/usr/bin/env python3
"""Plugin Factory OS — discover VST3 artefacts, verify registry, ship to disk."""

from __future__ import annotations

import argparse
import os
import shutil
import sys
from pathlib import Path


def factory_root() -> Path:
    return Path(__file__).resolve().parents[1]


def repo_root() -> Path:
    return factory_root().parent


def load_registry(root: Path | None = None) -> list[str]:
    reg = (root or factory_root()) / "plugins" / "registry.txt"
    entries: list[str] = []
    for line in reg.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        entries.append(line)
    return entries


def discover_vst3_bundles(root: Path | None = None, *, build_dir: Path | None = None) -> list[Path]:
    """Return Release VST3 bundles for every registered plugin that was built."""
    fr = root or factory_root()
    build = build_dir or (fr / "build")
    if not build.is_dir():
        return []

    found: list[Path] = []
    for plugin_dir in load_registry(fr):
        search_roots = [
            build / "plugins" / plugin_dir,
            build / plugin_dir,
        ]
        for search in search_roots:
            if not search.is_dir():
                continue
            for path in sorted(search.rglob("*.vst3")):
                if path.is_dir() and "Release" in path.as_posix():
                    found.append(path.resolve())

    seen: set[Path] = set()
    unique: list[Path] = []
    for p in found:
        if p not in seen:
            seen.add(p)
            unique.append(p)
    return unique


def default_install_dir() -> Path:
    explicit = os.environ.get("FACTORY_VST3_INSTALL_DIR")
    if explicit:
        return Path(explicit).expanduser()
    if sys.platform == "win32":
        common = os.environ.get("COMMONPROGRAMFILES", r"C:\Program Files\Common Files")
        return Path(common) / "VST3"
    if sys.platform == "darwin":
        return Path.home() / "Library" / "Audio" / "Plug-Ins" / "VST3"
    return Path.home() / ".vst3"


def ship_bundles(bundles: list[Path], install_dir: Path | None = None) -> list[tuple[Path, Path]]:
    dest_root = (install_dir or default_install_dir()).expanduser()
    dest_root.mkdir(parents=True, exist_ok=True)
    shipped: list[tuple[Path, Path]] = []
    for bundle in bundles:
        target = dest_root / bundle.name
        if target.exists():
            shutil.rmtree(target)
        shutil.copytree(bundle, target)
        shipped.append((bundle, target))
    return shipped


def cmd_discover(_: argparse.Namespace) -> int:
    bundles = discover_vst3_bundles()
    if not bundles:
        print("No factory VST3 bundles found. Run factory build first.", file=sys.stderr)
        return 2
    for b in bundles:
        print(b)
    return 0


def cmd_verify(args: argparse.Namespace) -> int:
    fr = factory_root()
    registered = load_registry(fr)
    bundles = discover_vst3_bundles(fr)
    by_name = {b.name: b for b in bundles}
    ok = True
    lines: list[str] = []
    for name in registered:
        matches = [b for b in bundles if f"/plugins/{name}/" in b.as_posix()]
        if not matches:
            ok = False
            lines.append(f"[MISSING] {name} — no Release VST3 under build/plugins/{name}/")
        else:
            for b in matches:
                lines.append(f"[OK] {name} → {b.relative_to(fr)}")
    if not registered:
        lines.append("[WARN] registry is empty")
        ok = False
    print("\n".join(lines))
    if args.json:
        return 0 if ok else 1
    return 0 if ok else 1


def cmd_ship(args: argparse.Namespace) -> int:
    bundles = discover_vst3_bundles()
    if not bundles:
        print("Nothing to ship — build factory plugins first.", file=sys.stderr)
        return 2
    install = Path(args.install_dir).expanduser() if args.install_dir else default_install_dir()
    shipped = ship_bundles(bundles, install)
    for src, dst in shipped:
        print(f"SHIPPED {src.name}\n  from {src}\n  to   {dst}")
    print(f"\nInstall directory: {install} ({len(shipped)} bundle(s))")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)

    p_disc = sub.add_parser("discover", help="Print built VST3 bundle paths")
    p_disc.set_defaults(func=cmd_discover)

    p_ver = sub.add_parser("verify-artefacts", help="Ensure each registry plugin has a VST3 artefact")
    p_ver.add_argument("--json", action="store_true")
    p_ver.set_defaults(func=cmd_verify)

    p_ship = sub.add_parser("ship", help="Copy all factory VST3 bundles to the install directory")
    p_ship.add_argument(
        "--install-dir",
        help="Override FACTORY_VST3_INSTALL_DIR / platform default",
    )
    p_ship.set_defaults(func=cmd_ship)

    args = parser.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
