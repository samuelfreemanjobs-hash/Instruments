#!/usr/bin/env python3
"""Print available MIDI input/output port names."""

from __future__ import annotations

from renaissance_host.ports import list_io_names


def main() -> None:
    ins, outs = list_io_names()
    print("MIDI inputs:")
    for i, name in enumerate(ins):
        print(f"  [{i}] {name}")
    print("MIDI outputs:")
    for i, name in enumerate(outs):
        print(f"  [{i}] {name}")
    if not ins and not outs:
        print("(no ports — connect Renaissance or a virtual MIDI driver)")


if __name__ == "__main__":
    main()
