#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
kick = subprocess.run([sys.executable, str(ROOT / "generate_kick.py")], check=True, capture_output=True, text=True)
wav = ROOT.parent / "artifacts" / "memphis_kick_twin.wav"
if not wav.is_file() or wav.stat().st_size < 1000:
    raise SystemExit("memphis kick wav missing or too small")
print("memphis-architect smoke: ok", wav.stat().st_size)
