#!/usr/bin/env python3
"""
Parse Yamaha DX7 32-voice bank SysEx into fm4op-dx7-xd voice blobs (stub P1).

Usage (planned):
  python3 tools/dx7/import_dx7_bank.py --in patches.syx --out src/oscillators/fm4op-dx7-xd/voices.bin

DX7 bank: header F0 43 00 09 20 00, then 32 × 128 bytes (4096 total voice data).
This script validates headers and writes a placeholder until full Voice4 mapping lands.

See docs/minilogue-xd-fm4-dx7-oscillator-plan.md
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

DX7_BANK_HEADERS = (
    bytes.fromhex("f04300092000"),
    bytes.fromhex("f04300042000"),
)


def main() -> int:
    parser = argparse.ArgumentParser(description="DX7 bank → fm4op voice blob (stub)")
    parser.add_argument("--in", dest="inp", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    data = args.inp.read_bytes()
    if len(data) < 6 + 4096:
        print("error: file too short for 32-voice DX7 bank", file=sys.stderr)
        return 1

    head = data[:6]
    if head not in DX7_BANK_HEADERS:
        print(f"warning: unexpected sysex header {head.hex()}", file=sys.stderr)

    payload = data[6 : 6 + 4096]
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_bytes(payload)
    print(f"Wrote raw 4096-byte voice block → {args.out} (Voice4 mapping not applied yet)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
