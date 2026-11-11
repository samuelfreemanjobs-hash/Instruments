#!/usr/bin/env bash
# Repo-health: fast Python/docs contract checks (no JUCE build).
# Linux / macOS / Git Bash on Windows.
#
# Usage:
#   bash scripts/repo_health.sh              # run all checks; exit 1 if any fail
#   bash scripts/repo_health.sh --only NAME  # run one check by id (see list below)
#   bash scripts/repo_health.sh --list
#
# Check ids: tokens | gen-tokens | ds-unit | apc-templates | doc-links |
#            factory | memory | fleet | apc-gates

set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

# Prefer python3; fall back to python (Windows Git Bash often has both).
if command -v python3 >/dev/null 2>&1; then
  PY=python3
elif command -v python >/dev/null 2>&1; then
  PY=python
else
  echo "ERROR: python3/python not found" >&2
  exit 127
fi

ONLY=""
LIST=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    --only) ONLY="${2:-}"; shift 2 ;;
    --list) LIST=1; shift ;;
    -h|--help)
      sed -n '2,16p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown arg: $1" >&2
      exit 2
      ;;
  esac
done

# ----- check implementations (echo human label on stdout first line optional) -----

check_tokens() {
  echo "design-system token contrast + preview freshness"
  "$PY" vst/factory-lib/design-system/scripts/check_tokens.py --check-preview
}

check_gen_tokens() {
  echo "design-system generated DesignTokens.h freshness"
  "$PY" vst/factory-lib/design-system/scripts/gen_juce_tokens.py --all --check
}

check_ds_unit() {
  echo "design-system python unit tests"
  "$PY" -m unittest discover -s vst/factory-lib/design-system/scripts -p 'test_*.py'
}

check_apc_templates() {
  echo "APC template contract tests"
  "$PY" -m unittest tools/test_apc_templates.py
}

check_doc_links() {
  echo "markdown relative-link check (default set + all SOPs)"
  "$PY" tools/check_doc_links.py || return $?
  "$PY" tools/check_doc_links.py 'docs/disklordz/sops/*.md'
}

check_factory() {
  echo "disklordz/vst-factory import + pytest"
  # Isolate install so local system site-packages stay clean when venv works.
  # Local: isolate in a venv outside the worktree. CI (setup-python): install into the job env.
  local py_bin="$PY"
  local pip_bin=""
  if [[ "${CI:-}" == "true" && -z "${REPO_HEALTH_VENV:-}" ]]; then
    py_bin="$PY"
  else
    local tmp_base="${TMPDIR:-/tmp}"
    local venv_dir="${REPO_HEALTH_VENV:-$tmp_base/repo-health-venv}"
    if "$PY" -m venv --help >/dev/null 2>&1; then
      if [[ ! -x "$venv_dir/bin/python" && ! -x "$venv_dir/Scripts/python.exe" ]]; then
        "$PY" -m venv "$venv_dir" || true
      fi
      if [[ -x "$venv_dir/bin/python" ]]; then
        py_bin="$venv_dir/bin/python"
        pip_bin="$venv_dir/bin/pip"
      elif [[ -x "$venv_dir/Scripts/python.exe" ]]; then
        py_bin="$venv_dir/Scripts/python.exe"
        pip_bin="$venv_dir/Scripts/pip.exe"
      fi
    fi
  fi
  (
    cd disklordz/vst-factory
    if [[ -n "$pip_bin" ]]; then
      "$pip_bin" install -e '.[dev]'
    else
      "$py_bin" -m pip install -e '.[dev]'
    fi
    "$py_bin" -c 'import vst_factory.main'
    "$py_bin" -m pytest -q
  )
  local rc=$?
  # pytest mutates data/liaison-status.json timestamps — restore tracked file.
  if command -v git >/dev/null 2>&1 && git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    git checkout -- disklordz/vst-factory/data/liaison-status.json 2>/dev/null || true
  fi
  return "$rc"
}

check_memory() {
  echo "agent memory integrity + unit tests"
  if [[ ! -f scripts/agent_memory.py ]]; then
    echo "SKIP: scripts/agent_memory.py not present (PR #129 not merged yet)"
    return 0
  fi
  "$PY" scripts/agent_memory.py check || return $?
  "$PY" -m unittest discover -s scripts -p 'test_agent_memory.py'
}

check_fleet() {
  echo "disklordz agent fleet tree check"
  "$PY" disklordz/agents/profit/scaffold_agents.py --check
}

# Map plugins/<Name>/status.json → highest apc-gate phase we can enforce without a JUCE build.
# Prints "TARGET|HINT" on stdout. Empty TARGET = report-only (not yet at plan).
# dream → report-only; plan/design/impl as-is; test/ship/debug → impl (ship needs .vst3).
_apc_gate_target_line() {
  local status="$1"
  "$PY" - "$status" <<'PY'
import json, sys
from pathlib import Path
s = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
hint = (s.get("apc_phase") or s.get("current_phase") or "").strip().lower()
phase = hint
v = s.get("validation") or {}
if phase in ("", "dream"):
    if v.get("design_complete"):
        phase = "impl"
    elif v.get("architecture_defined"):
        phase = "design"
    elif v.get("creative_brief_exists") and v.get("parameter_spec_exists"):
        phase = "plan"
mapping = {
    "dream": "",
    "plan": "plan",
    "design": "design",
    "impl": "impl",
    "test": "impl",
    "debug": "impl",
    "ship": "impl",
}
target = mapping.get(phase, "plan" if phase else "")
print(f"{target}|{hint or phase or 'unknown'}")
PY
}

check_apc_gates() {
  echo "APC gates for every plugins/*/status.json (up to derived phase; ship capped at impl)"
  local failed=0
  local found=0
  local status name line target hint
  shopt -s nullglob
  for status in plugins/*/status.json; do
    found=1
    name="$(basename "$(dirname "$status")")"
    line="$(_apc_gate_target_line "$status")" || line="|error"
    target="${line%%|*}"
    hint="${line#*|}"
    if [[ -z "$target" ]]; then
      echo "REPORT-ONLY: $name not yet at plan (phase=${hint}) — skip gate"
      continue
    fi
    echo "GATE $name → $target (status phase hint: $hint)"
    if ! bash tools/apc-gate.sh "$name" "$target"; then
      failed=1
    fi
  done
  shopt -u nullglob
  if [[ "$found" -eq 0 ]]; then
    echo "No plugins/*/status.json found — nothing to gate"
  fi
  return "$failed"
}

# Ordered registry: id|function|title
CHECKS=(
  "tokens|check_tokens|1. design-system tokens (--check-preview)"
  "gen-tokens|check_gen_tokens|2. gen_juce_tokens --all --check"
  "ds-unit|check_ds_unit|3. design-system unittest"
  "apc-templates|check_apc_templates|4. APC templates unittest"
  "doc-links|check_doc_links|5. doc links (default + SOPs)"
  "factory|check_factory|6. vst-factory import + pytest"
  "memory|check_memory|7. agent memory (optional)"
  "fleet|check_fleet|8. agent fleet scaffold --check"
  "apc-gates|check_apc_gates|9. APC gates (derived phase)"
)

if [[ "$LIST" -eq 1 ]]; then
  for entry in "${CHECKS[@]}"; do
    IFS='|' read -r id _fn title <<<"$entry"
    printf '%-14s %s\n' "$id" "$title"
  done
  exit 0
fi

run_one() {
  local id="$1" fn="$2" title="$3"
  echo ""
  echo "======== $title ========"
  "$fn"
  return $?
}

# Disable errexit for the driver so one failure does not abort the suite.
set +e

RESULTS=()
OVERALL=0

for entry in "${CHECKS[@]}"; do
  IFS='|' read -r id fn title <<<"$entry"
  if [[ -n "$ONLY" && "$ONLY" != "$id" ]]; then
    continue
  fi
  # Memory is optional when the tool is absent; still a named check when --only memory.
  run_one "$id" "$fn" "$title"
  rc=$?
  if [[ $rc -eq 0 ]]; then
    RESULTS+=("$id|PASS|$title")
    echo "→ PASS ($id)"
  else
    RESULTS+=("$id|FAIL|$title")
    echo "→ FAIL ($id) exit=$rc"
    OVERALL=1
  fi
done

if [[ -n "$ONLY" && ${#RESULTS[@]} -eq 0 ]]; then
  echo "Unknown check id: $ONLY (try --list)" >&2
  exit 2
fi

echo ""
echo "======== repo-health summary ========"
printf '%-14s %-6s %s\n' "CHECK" "RESULT" "TITLE"
printf '%-14s %-6s %s\n' "-----" "------" "-----"
for row in "${RESULTS[@]}"; do
  IFS='|' read -r id result title <<<"$row"
  printf '%-14s %-6s %s\n' "$id" "$result" "$title"
done

if [[ -n "${GITHUB_STEP_SUMMARY:-}" && -z "$ONLY" ]]; then
  {
    echo "## Repo health"
    echo ""
    echo "| Check | Result | Title |"
    echo "|-------|--------|-------|"
    for row in "${RESULTS[@]}"; do
      IFS='|' read -r id result title <<<"$row"
      echo "| \`$id\` | **$result** | $title |"
    done
    if [[ "$OVERALL" -eq 0 ]]; then
      echo ""
      echo "All checks passed."
    else
      echo ""
      echo "One or more checks failed."
    fi
  } >> "$GITHUB_STEP_SUMMARY"
fi

if [[ "$OVERALL" -eq 0 ]]; then
  echo "ALL PASSED"
else
  echo "FAILED"
fi
exit "$OVERALL"
