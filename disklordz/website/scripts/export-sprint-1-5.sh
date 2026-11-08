#!/usr/bin/env bash
# Regenerate 808 Lab sprint load packs (sessions 1–5). Run from disklordz/website.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DOCS="$ROOT/docs/disklordz/spark-le"
cd "$(dirname "$0")/.."

npx --yes tsx scripts/export-spark-sprint-kit.ts \
  --manifest "$DOCS/examples/manifest-gangsta-trap.json" \
  --out "$DOCS/sprint-kits/session-1-gangsta-trap" \
  --prompt "trap or die street hard knock" \
  --seed 14031103

npx --yes tsx scripts/export-spark-sprint-kit.ts \
  --manifest "$DOCS/examples/manifest-dj-screw.json" \
  --out "$DOCS/sprint-kits/session-2-dj-screw" \
  --prompt "chop screw slow houston" \
  --seed 72031103

npx --yes tsx scripts/export-spark-sprint-kit.ts \
  --manifest "$DOCS/examples/manifest-smooth-jazz-rap.json" \
  --out "$DOCS/sprint-kits/session-3-smooth-jazz-rap" \
  --prompt "smooth jazz rap smoky cruise" \
  --seed 86031103

npx --yes tsx scripts/export-spark-sprint-kit.ts \
  --manifest "$DOCS/examples/manifest-trunk-sermon.json" \
  --out "$DOCS/sprint-kits/session-4-trunk-sermon" \
  --prompt "fat pat syrupy trunk houston" \
  --seed 84031103

npx --yes tsx scripts/export-spark-sprint-kit.ts \
  --manifest "$DOCS/examples/manifest-stretch-uk-drill.json" \
  --out "$DOCS/sprint-kits/session-5-stretch-uk-drill" \
  --prompt "uk drill british sliding 808" \
  --seed 14431103

npm run verify:spark
echo "export-sprint-1-5: OK"
