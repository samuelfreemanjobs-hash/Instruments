#!/usr/bin/env python3
"""Overnight DiskLordz kit evolution loop.

Default: synthesize → score → mutate → log. Does NOT rebuild VST3 each iteration.
Optional: --build-plugin once (COPY_PLUGIN_AFTER_BUILD=OFF).
"""
from __future__ import annotations

import argparse
import json
import logging
import shutil
import subprocess
import sys
import time
from dataclasses import asdict
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parent
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from agent.mutate import propose
from agent.report import write_morning_report
from agent.score import score_kit
from agent.synth_kit import KitParams, generate_kit

OUT = ROOT / "output"
KITS = OUT / "kits"
LOG = OUT / "agent.log"
STATE = OUT / "state.json"
BEST = OUT / "best_kit.json"
REPORT = OUT / "morning_report.md"


def setup_log() -> logging.Logger:
    OUT.mkdir(parents=True, exist_ok=True)
    logger = logging.getLogger("overnight")
    logger.setLevel(logging.INFO)
    logger.handlers.clear()
    fmt = logging.Formatter("%(asctime)s %(levelname)s %(message)s")
    fh = logging.FileHandler(LOG, encoding="utf-8")
    fh.setFormatter(fmt)
    sh = logging.StreamHandler(sys.stdout)
    sh.setFormatter(fmt)
    logger.addHandler(fh)
    logger.addHandler(sh)
    return logger


def load_state() -> dict:
    if STATE.is_file():
        return json.loads(STATE.read_text(encoding="utf-8"))
    return {
        "history": [],
        "best": {"score": -1.0, "params": asdict(KitParams()), "kit_dir": ""},
        "iteration": 0,
        "last_mutation": "none",
    }


def save_state(state: dict) -> None:
    STATE.write_text(json.dumps(state, indent=2), encoding="utf-8")


def maybe_build_plugin(log: logging.Logger) -> bool | None:
    """Optional one-shot plugin build. Never copies into Program Files."""
    plugin = ROOT / "plugin"
    if not (plugin / "CMakeLists.txt").is_file():
        log.warning("plugin scaffold missing — skip build")
        return None
    build = plugin / "build"
    try:
        subprocess.run(
            [
                "cmake",
                "-S",
                str(plugin),
                "-B",
                str(build),
                "-G",
                "Visual Studio 17 2022",
                "-A",
                "x64",
                "-DCOPY_PLUGIN_AFTER_BUILD=OFF",
            ],
            check=True,
            capture_output=True,
            text=True,
        )
        subprocess.run(
            ["cmake", "--build", str(build), "--config", "Release"],
            check=True,
            capture_output=True,
            text=True,
        )
        log.info("plugin build OK (COPY_PLUGIN_AFTER_BUILD=OFF)")
        return True
    except Exception as exc:
        log.error("plugin build failed: %s", exc)
        return False


def run_loop(hours: float, max_iterations: int | None, build_plugin: bool, seed: int) -> None:
    log = setup_log()
    state = load_state()
    best_params = KitParams(**state["best"].get("params", asdict(KitParams())))
    best_score = float(state["best"].get("score", -1.0))

    deadline = time.time() + hours * 3600.0
    build_ok = maybe_build_plugin(log) if build_plugin else None
    log.info(
        "overnight start hours=%.3f build_plugin=%s mistral=%s",
        hours,
        build_plugin,
        bool(__import__("os").environ.get("MISTRAL_API_KEY")),
    )

    iteration = int(state.get("iteration", 0))
    while time.time() < deadline:
        if max_iterations is not None and iteration >= max_iterations:
            break
        iteration += 1
        kit_dir = KITS / f"iter_{iteration:04d}_{datetime.now(timezone.utc).strftime('%H%M%S')}"
        try:
            params, mode = propose(best_params, iteration, best_score, seed)
            state["last_mutation"] = mode
            manifest = generate_kit(kit_dir, params)
            scored = score_kit(kit_dir, build_ok=build_ok)
            row = {
                "iteration": iteration,
                "score": scored["score"],
                "ok": scored["ok"],
                "mutation": mode,
                "kit_dir": str(kit_dir),
                "utc": datetime.now(timezone.utc).isoformat(),
            }
            state["history"].append(row)
            state["iteration"] = iteration
            log.info(
                "iter=%d score=%.2f ok=%s mutation=%s dir=%s",
                iteration,
                scored["score"],
                scored["ok"],
                mode,
                kit_dir.name,
            )
            if scored["score"] > best_score:
                best_score = scored["score"]
                best_params = params
                state["best"] = {
                    "score": best_score,
                    "params": asdict(params),
                    "kit_dir": str(kit_dir),
                    "manifest": manifest,
                }
                BEST.write_text(json.dumps(state["best"], indent=2), encoding="utf-8")
                # keep a stable pointer for the plugin
                best_link = OUT / "best_kit"
                if best_link.exists():
                    if best_link.is_dir():
                        shutil.rmtree(best_link)
                    else:
                        best_link.unlink()
                shutil.copytree(kit_dir, best_link)
                log.info("new best score=%.2f → %s", best_score, kit_dir.name)
            save_state(state)
        except Exception as exc:
            log.exception("iter=%d failed: %s — continuing", iteration, exc)
            state["history"].append(
                {
                    "iteration": iteration,
                    "score": 0.0,
                    "ok": False,
                    "mutation": "error",
                    "error": str(exc),
                    "utc": datetime.now(timezone.utc).isoformat(),
                }
            )
            save_state(state)

    report = write_morning_report(STATE, REPORT)
    log.info("overnight done — report %s", report)


def main() -> int:
    ap = argparse.ArgumentParser(description="DiskLordz overnight drum-kit agent")
    ap.add_argument("--hours", type=float, default=8.0, help="wall-clock hours to run")
    ap.add_argument("--iterations", type=int, default=None, help="optional hard cap")
    ap.add_argument(
        "--build-plugin",
        action="store_true",
        help="build JUCE VST3 once (COPY_PLUGIN_AFTER_BUILD=OFF)",
    )
    ap.add_argument("--seed", type=int, default=0x0D15C1AB)
    args = ap.parse_args()
    run_loop(args.hours, args.iterations, args.build_plugin, args.seed)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
