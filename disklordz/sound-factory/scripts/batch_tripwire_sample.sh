#!/usr/bin/env bash
# Batch factory candidates for Day 2 tripwire (DL002). A&R QC still required.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="$ROOT/launch-assets/tripwire-sample-v1/candidates"
PRESET="midnight-circuit"
PROMPTS=(
  "1997 Memphis phonk kick short boxy saturated"
  "drift phonk 808 long sub glide dark minor"
  "Memphis snap snare bit crushed"
  "phonk cowbell metallic lead F sharp"
  "dirty hat sizzle sample rate reduced"
)

mkdir -p "$OUT"
for i in "${!PROMPTS[@]}"; do
  python3 "$(dirname "$0")/generate_kit.py" \
    --preset "$PRESET" \
    --prompt "${PROMPTS[$i]}" \
    --out "$OUT/batch_$i"
done
echo "Wrote candidate kits under $OUT — run A&R QC before shipping."
