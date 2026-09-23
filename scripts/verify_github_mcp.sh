#!/usr/bin/env bash
# Verify GitHub auth for agents: gh CLI (OAuth) + optional GitHub MCP PAT.
set -euo pipefail

echo "== GitHub CLI (git operations) =="
if gh auth status -h github.com 2>&1 | grep -q 'Logged in'; then
  gh auth status -h github.com 2>&1 | sed -n '1,3p'
  gh api user --jq '"login: " + .login' 2>/dev/null || true
else
  echo "FAIL: gh not logged in — run: gh auth login"
  exit 1
fi

echo ""
echo "== GitHub MCP (api.githubcopilot.com) =="
if [[ -z "${GITHUB_TOKEN:-}" ]]; then
  echo "FAIL: GITHUB_TOKEN unset."
  echo "Approve Cloud secret GITHUB_TOKEN, Save environment, start a NEW agent run."
  echo "(Hosted MCP requires PAT; see docs/GITHUB_MCP.md)"
  exit 1
fi

code=$(curl -sS -o /dev/null -w "%{http_code}" --max-time 25 \
  -H "Authorization: Bearer ${GITHUB_TOKEN}" \
  -H "Accept: application/json" \
  "https://api.githubcopilot.com/mcp/" || echo "000")

if [[ "$code" == "000" ]]; then
  echo "FAIL: cannot reach api.githubcopilot.com (egress? add to .cursor/environment.json egressAllowlist and Save environment)"
  exit 1
fi

echo "HTTP $code from MCP root (401/403 may mean bad token; 2xx/404/405 often means host reachable)"
if [[ "$code" =~ ^2 ]]; then
  echo "OK: MCP host reachable with token"
fi
