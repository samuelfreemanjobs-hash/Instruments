#!/usr/bin/env python3
"""Extract pitch hints via spotify/basic-pitch (optional)."""

from __future__ import annotations

import json
import sys


def main() -> None:
    path = sys.argv[1] if len(sys.argv) > 1 else ""
    if not path:
        print(json.dumps({"error": "usage: analyze.py path.wav"}))
        sys.exit(1)
    try:
        from basic_pitch.inference import predict
        from basic_pitch import ICASSP_2022_MODEL_PATH
    except ImportError:
        print(json.dumps({"error": "pip install basic-pitch"}))
        sys.exit(1)
    _, midi_data, _ = predict(path, ICASSP_2022_MODEL_PATH)
    print(json.dumps({"midiNotes": len(midi_data.instruments[0].notes) if midi_data.instruments else 0}))


if __name__ == "__main__":
    main()
