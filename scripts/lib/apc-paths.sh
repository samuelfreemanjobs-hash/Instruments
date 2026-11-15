#!/usr/bin/env bash
# Resolve APC paths from apc.config.json (Instruments repo).

apc_repo_root() {
  local here dir
  here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  dir="$here/../.."
  if [[ -f "$dir/apc.config.example.json" ]] || [[ -f "$dir/AGENTS.md" ]]; then
    cd "$dir" && pwd
    return
  fi
  pwd
}

apc_resolve_path() {
  local value="$1" root="$2"
  if [[ "$value" = /* ]]; then
    printf '%s\n' "$value"
  else
    local joined="$root/$value"
    if command -v python3 >/dev/null 2>&1; then
      python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "$joined"
    else
      printf '%s\n' "$joined"
    fi
  fi
}

apc_read_config_field() {
  local root="$1" jq_path="$2" default="$3"
  local cfg="$root/apc.config.json"
  local example="$root/apc.config.example.json"
  local file=""
  [[ -f "$cfg" ]] && file="$cfg"
  [[ -z "$file" && -f "$example" ]] && file="$example"
  [[ -z "$file" ]] && { printf '%s\n' "$default"; return; }
  if command -v jq &>/dev/null; then
    local val
    val="$(jq -r "$jq_path // empty" "$file" 2>/dev/null || true)"
    if [[ -n "$val" && "$val" != "null" ]]; then
      printf '%s\n' "$val"
      return
    fi
  fi
  printf '%s\n' "$default"
}

apc_load_paths() {
  APC_REPO_ROOT="$(apc_repo_root)"
  local plugins_rel vst_rel
  plugins_rel="$(apc_read_config_field "$APC_REPO_ROOT" ".paths.plugins_dir" "plugins")"
  vst_rel="$(apc_read_config_field "$APC_REPO_ROOT" ".paths.vst_dir" "vst")"
  APC_PLUGINS_DIR="$(apc_resolve_path "$plugins_rel" "$APC_REPO_ROOT")"
  APC_VST_DIR="$(apc_resolve_path "$vst_rel" "$APC_REPO_ROOT")"
  APC_BUILD_DIR="$(apc_resolve_path "$(apc_read_config_field "$APC_REPO_ROOT" ".paths.build_dir" "build")" "$APC_REPO_ROOT")"
  APC_RELEASE_DIR="$(apc_resolve_path "$(apc_read_config_field "$APC_REPO_ROOT" ".paths.release_dir" "release")" "$APC_REPO_ROOT")"
  APC_REF_AUDIO_DIR="$(apc_resolve_path "$(apc_read_config_field "$APC_REPO_ROOT" ".paths.reference_audio_dir" "reference-audio")" "$APC_REPO_ROOT")"
  export APC_REPO_ROOT APC_PLUGINS_DIR APC_VST_DIR APC_BUILD_DIR APC_RELEASE_DIR APC_REF_AUDIO_DIR
}

apc_plugin_path() {
  local name="$1"
  [[ -n "${APC_PLUGINS_DIR:-}" ]] || apc_load_paths
  printf '%s\n' "$APC_PLUGINS_DIR/$name"
}

apc_vst_path() {
  local name="$1"
  [[ -n "${APC_VST_DIR:-}" ]] || apc_load_paths
  printf '%s\n' "$APC_VST_DIR/$name"
}

apc_setup_completed() {
  local root cfg
  root="$(apc_repo_root)"
  cfg="$root/apc.config.json"
  [[ -f "$cfg" ]] || return 1
  if command -v jq &>/dev/null; then
    [[ "$(jq -r '.setup.completed // false' "$cfg")" == "true" ]]
  else
    grep -q '"completed"[[:space:]]*:[[:space:]]*true' "$cfg"
  fi
}
