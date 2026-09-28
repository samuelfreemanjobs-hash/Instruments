#!/usr/bin/env python3
"""
Phonk sample ingest — download (robots-aware URL list) + optional loop slicing.

Legal: only pass URLs you are allowed to fetch and use commercially.
Looperman/site-specific crawlers are not enabled by default; use curated URL lists.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
BLUEPRINT = REPO / "tools" / "drum-synth-blueprint"


def main() -> int:
    parser = argparse.ArgumentParser(description="Memphis phonk sample ingest agent")
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--download-urls", type=Path, help="Text file, one WAV URL per line")
    parser.add_argument("--download-dir", type=Path, default=BLUEPRINT / "drums" / "ingest")
    parser.add_argument("--slice-input", type=Path, help="Loop folder to slice")
    parser.add_argument("--slice-output", type=Path, default=BLUEPRINT / "drums" / "sliced")
    args = parser.parse_args()

    if args.self_test:
        subprocess.run(
            [sys.executable, "-m", "pytest", "tests/test_phonk_slicer.py", "-q"],
            cwd=BLUEPRINT,
            check=True,
        )
        print("phonk-sample-ingest: ok")
        return 0

    if args.download_urls:
        urls = [
            ln.strip()
            for ln in args.download_urls.read_text(encoding="utf-8").splitlines()
            if ln.strip() and not ln.strip().startswith("#")
        ]
        sys.path.insert(0, str(BLUEPRINT))
        from drum_synth_blueprint.phonk_kit.sample_download import download_url_list

        recs = download_url_list(urls, args.download_dir)
        print(json.dumps({"downloaded": len(recs)}, indent=2))

    if args.slice_input:
        script = BLUEPRINT / "scripts" / "slice_phonk_loops.py"
        subprocess.run(
            [
                sys.executable,
                str(script),
                "--input",
                str(args.slice_input),
                "--output",
                str(args.slice_output),
            ],
            check=True,
        )

    if not args.download_urls and not args.slice_input:
        print(
            "Usage: --download-urls urls.txt | --slice-input ./loops\n"
            f"Blueprint: {BLUEPRINT}",
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
