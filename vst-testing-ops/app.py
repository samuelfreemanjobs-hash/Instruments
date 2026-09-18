#!/usr/bin/env python3
"""Streamlit command center — run the full Instruments business pipeline."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass, replace
import os
import subprocess
from pathlib import Path

import streamlit as st

OPS_ROOT = Path(__file__).resolve().parent
if str(OPS_ROOT) not in sys.path:
    sys.path.insert(0, str(OPS_ROOT))

from business_pipeline import (  # noqa: E402
    ERROR_LOG,
    PROFILES,
    REPORT_JSON,
    PipelineConfig,
    fleet_status,
    resolve_stages,
    run_pipeline,
)

REPO_ROOT = OPS_ROOT.parent
TEST_RUNNER = OPS_ROOT / "test_runner.py"

AI_PROMPT = (
    "I ran the Instruments QA pipeline and a stage failed. "
    "Read vst-testing-ops/error_log.txt and the relevant source tree "
    "(Source/ for JD Upgraded, Wave909/ for Wave909, disklordz/ for SaaS). "
    "Identify the root cause and apply a minimal fix."
)


@dataclass(frozen=True)
class PluginTarget:
    label: str
    vst_name: str
    cmake_target: str
    project_root: Path


QUICK_TARGETS: list[PluginTarget] = [
    PluginTarget("JD Upgraded", "JD Upgraded.vst3", "JDUpgraded_VST3", REPO_ROOT),
    PluginTarget("Wave909", "Wave909.vst3", "Wave909_VST3", REPO_ROOT),
]


def run_quick_build_and_test(target: PluginTarget) -> tuple[bool, str]:
    cmd_build = ["cmake", "--build", "build", "-j", "--target", target.cmake_target]
    proc_b = subprocess.run(
        cmd_build,
        cwd=str(target.project_root),
        capture_output=True,
        text=True,
        check=False,
    )
    if proc_b.returncode != 0:
        return False, (proc_b.stdout or "") + (proc_b.stderr or "")
    env = os.environ.copy()
    env["VST_NAME"] = target.vst_name
    proc_t = subprocess.run(
        [sys.executable, str(TEST_RUNNER), target.vst_name],
        cwd=str(REPO_ROOT),
        capture_output=True,
        text=True,
        env=env,
        check=False,
    )
    out = (proc_b.stdout or "") + (proc_b.stderr or "") + (proc_t.stdout or "") + (proc_t.stderr or "")
    return proc_t.returncode == 0, out


st.set_page_config(page_title="Instruments Operations", layout="wide")
st.title("Instruments Operations Command Center")
st.caption("Build, DSP regression, golden WAVs, pluginval, and optional Disklordz SaaS smoke.")

status = fleet_status()
cols = st.columns(len(status))
for col, (name, ready) in zip(cols, status.items()):
    col.metric(name, "ready" if ready else "missing")

st.divider()

left, right = st.columns([1, 2])

with left:
    st.header("Run the business")
    profile = st.selectbox(
        "Pipeline profile",
        options=list(PROFILES.keys()),
        format_func=lambda k: {
            "ci": "CI parity (build.yml)",
            "full": "Full monorepo (+ SaaS build)",
            "plugin-quick": "pluginval only",
            "dsp-only": "OfflineRender + golden",
        }.get(k, k),
        index=0,
    )
    base = PROFILES[profile]
    include_saas = st.checkbox("Include Disklordz web build", value=base.disklordz_web)
    stop_on_fail = st.checkbox("Stop on first failure", value=True)

    if st.button("Run full pipeline", type="primary"):
        cfg = replace(base, disklordz_web=include_saas)
        total_stages = len(resolve_stages(cfg))
        progress = st.progress(0.0, text="Starting…")
        results: list = []

        def on_stage(res):
            results.append(res)
            pct = len(results) / max(total_stages, 1)
            progress.progress(min(pct, 1.0), text=f"Stage: {res.stage.value}")

        run = run_pipeline(profile, cfg, stop_on_fail=stop_on_fail, progress_cb=on_stage)
        st.session_state["last_run"] = run.to_dict()
        progress.progress(1.0, text="Complete")
        if run.ok:
            st.success(f"All stages passed ({run.to_dict()['duration_s']:.0f}s)")
        else:
            st.error("Pipeline failed — see error_log.txt and report.")

    st.subheader("Quick lane (one plugin)")
    qt = st.selectbox("Target", [t.label for t in QUICK_TARGETS])
    target = next(t for t in QUICK_TARGETS if t.label == qt)
    if st.button("Build target + pluginval only"):
        ok, out = run_quick_build_and_test(target)
        st.session_state["quick_out"] = out
        st.success("OK") if ok else st.error("Failed")

with right:
    st.header("Telemetry")
    last = st.session_state.get("last_run")
    if last:
        st.subheader(f"Last pipeline — {last['profile']} — {'PASS' if last['ok'] else 'FAIL'}")
        for stage in last["stages"]:
            icon = "✅" if stage["ok"] else "❌"
            st.markdown(f"{icon} **{stage['stage']}** ({stage['duration_s']:.1f}s)")
            if not stage["ok"]:
                st.code(stage["output"][-6000:], language="bash")
                break
        with st.expander("Full stage outputs"):
            st.json(last)

    if ERROR_LOG.is_file():
        st.error("error_log.txt (agent handoff)")
        st.code(ERROR_LOG.read_text(encoding="utf-8", errors="replace")[-12000:], language="bash")

    if REPORT_JSON.is_file():
        with st.expander("last_run_report.json"):
            st.code(REPORT_JSON.read_text(encoding="utf-8"), language="json")

    quick = st.session_state.get("quick_out")
    if quick:
        with st.expander("Last quick lane output"):
            st.code(quick[-8000:], language="bash")

st.divider()
st.header("AI resolution protocol")
st.info(AI_PROMPT)
st.caption(
    "CLI: `python3 vst-testing-ops/run_business.py --profile ci` · "
    "`python3 vst-testing-ops/test_runner.py` for single-VST stress only"
)
