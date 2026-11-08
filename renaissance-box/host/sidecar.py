#!/usr/bin/env python3
"""MPC Renaissance box sidecar — MIDI tap, extensions, optional forward."""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

import mido

from renaissance_host.config import load_box_config, load_midi_map
from renaissance_host.extensions import build_extensions
from renaissance_host.ports import list_io_names, pick_port


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Renaissance box MIDI sidecar")
    p.add_argument("--config", type=Path, required=True, help="box.windows.json or box.local.json")
    p.add_argument("--log-midi", action="store_true", help="Log all MIDI messages")
    p.add_argument("--dry-run", action="store_true", help="Validate config and exit")
    return p.parse_args()


def main() -> int:
    args = parse_args()
    cfg = load_box_config(args.config)
    _ = load_midi_map(cfg.midi_map_path)
    extensions = build_extensions(cfg)

    if args.dry_run:
        print(f"OK mode={cfg.mode} map={cfg.midi_map_path} extensions={len(extensions)}")
        return 0

    ins, outs = list_io_names()
    in_name = pick_port(
        ins,
        cfg.midi.input_port_substring,
        cfg.midi.fallback_input_index,
    )
    out_name = pick_port(
        outs,
        cfg.midi.output_port_substring,
        cfg.midi.fallback_output_index,
    )

    if in_name is None:
        print("No MIDI input matched; connect hardware or fix inputPortSubstring.", file=sys.stderr)
        return 1

    print(f"Input:  {in_name}")
    if out_name:
        print(f"Output: {out_name}")
    else:
        print("Output: (none — listen-only mode)")

    out_port = mido.open_output(out_name) if out_name else None

    try:
        with mido.open_input(in_name) as in_port:
            print("Sidecar running. Ctrl+C to stop.")
            while True:
                for ext in extensions:
                    if hasattr(ext, "tick"):
                        ext.tick()
                msg = in_port.poll()
                if msg is None:
                    time.sleep(0.002)
                    continue
                if args.log_midi:
                    print(msg)
                for ext in extensions:
                    ext.on_midi(msg)
                if out_port is not None:
                    out_port.send(msg)
    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        if out_port is not None:
            out_port.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
