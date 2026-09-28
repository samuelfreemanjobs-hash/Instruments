#!/usr/bin/env bash
# Social Clip Factory — cut ~15s preview (requires ffmpeg)
set -euo pipefail
INPUT="${1:-}"
OUT="${2:-clip-15s.mp4}"
if [[ -z "$INPUT" || ! -f "$INPUT" ]]; then
  echo "Usage: $0 input.wav|mp3 [output.mp4]" >&2
  exit 1
fi
if ! command -v ffmpeg >/dev/null; then
  echo "ffmpeg not installed; stub only documents intent." >&2
  exit 2
fi
ffmpeg -y -i "$INPUT" -t 15 -af "loudnorm" -c:a aac -b:a 128k "$OUT"
echo "Wrote $OUT"
