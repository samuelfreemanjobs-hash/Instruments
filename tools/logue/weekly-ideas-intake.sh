#!/usr/bin/env bash
# Generate weekly Korg Logue Agent intake issue body or agent prompt.
#
# Usage:
#   ./tools/logue/weekly-ideas-intake.sh --print-issue-body
#   ./tools/logue/weekly-ideas-intake.sh --print-agent-prompt
#   ./tools/logue/weekly-ideas-intake.sh --print-slack-text

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DATE="$(date -u +%Y-%m-%d)"
WEEK="$(date -u +%G-W%V)"

MODE="${1:---print-issue-body}"

ISSUE_BODY="$(cat <<EOF
## Korg Logue Agent — weekly oscillator ideas (${DATE})

**Work order (suggested):** \`WO-LOGUE-IDEAS-${WEEK}\`

### Agent task

1. Read [docs/logue-oscillator-ideas-backlog.md](docs/logue-oscillator-ideas-backlog.md) and [docs/logue-oscillator-collection-roadmap.md](docs/logue-oscillator-collection-roadmap.md).
2. Propose **3–5 new** NTS-1 mkII oscillator or custom FX **ideas** not already listed (unique \`snake_case\` slugs).
3. For each idea: genre/reference, one-line DSP sketch, ≤10 param names, ship priority (P1–P3).
4. **Do not implement** unless this issue is labeled \`logue-implement\` and a slug is named in the title.

### Team task (human)

- [ ] Review proposals in comments
- [ ] Approve slugs with 👍 or \`approved: <slug>\`
- [ ] PM moves approved rows to backlog \`planned\` or roadmap phase

### Backlog anchor

Append approved weekly rows under **Weekly intake queue** in \`docs/logue-oscillator-ideas-backlog.md\` (after \`WEEKLY_IDEAS_ANCHOR\`).

### Planned production (already approved — do not re-propose)

- \`plug_nether_pluck\` — Plug/Nether pluck
- \`drill_slide_808_tail\` — Drill 808 slide + tail

---
_Automation: \`.github/workflows/logue-weekly-ideas-intake.yml\`_
EOF
)"

AGENT_PROMPT="$(cat <<EOF
You are the **Korg Logue Agent** (logue SDK mkII custom oscillator agent).

Weekly ideas intake (${DATE}, ${WEEK}).

1. Read \`${ROOT}/docs/logue-oscillator-ideas-backlog.md\` and \`${ROOT}/docs/logue-oscillator-collection-roadmap.md\`.
2. Propose 3–5 NEW mkII oscillator or FX ideas (unique snake_case slugs) with genre reference and ≤10 params each.
3. Open a PR that appends a markdown table row per idea to the **Weekly intake queue** section (after WEEKLY_IDEAS_ANCHOR). Set status \`idea\` and source \`${DATE}\`.
4. Do NOT implement DSP in this run unless the user explicitly labeled the task \`logue-implement\`.
5. Comment on the weekly GitHub issue with a summary and link to the PR.

Respect security baseline: no secrets in git.
EOF
)"

SLACK_TEXT="Korg Logue Agent: weekly oscillator ideas intake for ${DATE} (${WEEK}). Check GitHub issue [logue-agent] for proposals."

case "${MODE}" in
  --print-issue-body)
    printf '%s\n' "${ISSUE_BODY}"
    ;;
  --print-agent-prompt)
    printf '%s\n' "${AGENT_PROMPT}"
    ;;
  --print-slack-text)
    printf '%s\n' "${SLACK_TEXT}"
    ;;
  *)
    echo "Usage: $0 --print-issue-body | --print-agent-prompt | --print-slack-text" >&2
    exit 1
    ;;
esac
