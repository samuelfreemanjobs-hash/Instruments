#!/usr/bin/env bash
# Plugin state helpers for status.json
# Requires: scripts/lib/apc-paths.sh sourced first (for apc_repo_root).

apc_state_now() {
  date -u +"%Y-%m-%dT%H:%M:%SZ"
}

apc_init_status() {
  local plugin_path="$1" name="$2"
  local tpl
  tpl="$(apc_repo_root)/templates/apc/status-template.json"
  mkdir -p "$plugin_path/.ideas" "$plugin_path/references"
  if [[ -f "$tpl" ]]; then
    sed "s/\"plugin_name\": \"\"/\"plugin_name\": \"$name\"/" "$tpl" \
      | sed "s/\"created_at\": \"\"/\"created_at\": \"$(apc_state_now)\"/" \
      | sed "s/\"last_modified\": \"\"/\"last_modified\": \"$(apc_state_now)\"/" \
      > "$plugin_path/status.json"
  else
    echo '{"plugin_name":"'"$name"'","current_phase":"ideation"}' > "$plugin_path/status.json"
  fi
}

apc_complete_phase() {
  local plugin_path="$1" phase="$2"
  local status="$plugin_path/status.json"
  [[ -f "$status" ]] || return 1
  if command -v jq &>/dev/null; then
    local tmp now hist
    now="$(apc_state_now)"
    hist="$(jq -c --arg p "$phase" --arg t "$now" \
      '.phase_history + [{"phase":$p,"completed_at":$t}]' "$status")"
    jq --arg p "$phase" --arg t "$now" --argjson h "$hist" \
      '.current_phase = $p | .last_modified = $t | .phase_history = $h' "$status" > "${status}.tmp"
    mv "${status}.tmp" "$status"
  else
    echo "warn: install jq for phase updates" >&2
  fi
}

apc_set_field() {
  local plugin_path="$1" jq_expr="$2"
  local status="$plugin_path/status.json"
  command -v jq &>/dev/null || return 1
  jq "$jq_expr" "$status" > "${status}.tmp" && mv "${status}.tmp" "$status"
}
