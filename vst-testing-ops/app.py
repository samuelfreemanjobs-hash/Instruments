#!/usr/bin/env python3
"""Streamlit dashboard: configure → CMake build → pluginval via test_runner.py."""

from __future__ import annotations

import os
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

import streamlit as st

OPS_ROOT = Path(__file__).resolve().parent
REPO_ROOT = OPS_ROOT.parent
ERROR_LOG = OPS_ROOT / "error_log.txt"
BUILD_LOG = OPS_ROOT / "last_build.log"
TEST_RUNNER = OPS_ROOT / "test_runner.py"

AI_PROMPT = (
    "I ran my VST3 through a headless stress test and it crashed. "
    "Review my C++ source code alongside this error log and identify the exact line "
    "causing the memory leak or DSP failure. Provide the corrected code block."
)


@dataclass(frozen=True)
class PluginTarget:
    label: str
    vst_name: str
    cmake_target: str
    project_root: Path


TARGETS: list[PluginTarget] = [
    PluginTarget(
        "JD Upgraded (monorepo)",
        "JD Upgraded.vst3",
        "JDUpgraded_VST3",
        REPO_ROOT,
    ),
    PluginTarget(
        "Wave909 (monorepo)",
        "Wave909.vst3",
        "Wave909_VST3",
        REPO_ROOT,
    ),
    PluginTarget(
        "MyFirstPlugin",
        "MyFirstPlugin.vst3",
        "MyFirstPlugin_VST3",
        REPO_ROOT / "MyFirstPlugin",
    ),
]


def ensure_cmake_configured(project_root: Path) -> tuple[bool, str]:
    build = project_root / "build"
    cache = build / "CMakeCache.txt"
    if cache.is_file():
        return True, "CMake cache present."

    build.mkdir(parents=True, exist_ok=True)
    cmd = [
        "cmake",
        "-B",
        "build",
        "-DCMAKE_BUILD_TYPE=Release",
    ]
    if sys.platform != "win32":
        cmd.extend(["-DCMAKE_CXX_COMPILER=g++-12", "-DCMAKE_C_COMPILER=gcc-12"])
    proc = subprocess.run(
        cmd,
        cwd=str(project_root),
        capture_output=True,
        text=True,
        check=False,
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode == 0, out


def run_cmake_build(target: PluginTarget) -> tuple[int, str]:
    ok, configure_out = ensure_cmake_configured(target.project_root)
    if not ok:
        return 1, configure_out

    cmd = ["cmake", "--build", "build", "-j"]
    if sys.platform == "win32":
        cmd.extend(["--config", "Release"])
    cmd.extend(["--target", target.cmake_target])

    proc = subprocess.run(
        cmd,
        cwd=str(target.project_root),
        capture_output=True,
        text=True,
        check=False,
    )
    combined = configure_out + "\n" + (proc.stdout or "") + (proc.stderr or "")
    BUILD_LOG.write_text(combined, encoding="utf-8")
    return proc.returncode, combined


def run_stress_test(vst_name: str) -> tuple[int, str]:
    env = os.environ.copy()
    env["VST_NAME"] = vst_name
    proc = subprocess.run(
        [sys.executable, str(TEST_RUNNER), vst_name],
        cwd=str(REPO_ROOT),
        capture_output=True,
        text=True,
        env=env,
        check=False,
    )
    combined = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode, combined


def log_indicates_failure(text: str) -> bool:
    lower = text.lower()
    if "failed" in lower or "failure" in lower:
        return True
    if "error" in lower and "success:" not in lower:
        return True
    return False


st.set_page_config(page_title="VST Operations Command Center", layout="wide")
st.title("VST Automated Testing Command Center")

target_labels = [t.label for t in TARGETS]
default_idx = 0
env_vst = os.environ.get("VST_NAME")
if env_vst:
    for i, t in enumerate(TARGETS):
        if t.vst_name == env_vst:
            default_idx = i
            break

col1, col2 = st.columns([1, 2])

with col1:
    st.header("1. Execution engine")
    choice = st.selectbox("Plugin target", target_labels, index=default_idx)
    target = TARGETS[target_labels.index(choice)]
    st.write(f"**VST bundle:** `{target.vst_name}`")
    st.write(f"**CMake target:** `{target.cmake_target}`")
    st.caption(f"Project root: `{target.project_root}`")

    if st.button("Compile code and run stress test", type="primary"):
        with st.spinner("Step 1: Compiling latest code changes…"):
            code, build_out = run_cmake_build(target)
        if code != 0:
            st.error("Build failed. See build log below.")
            st.code(build_out[-12000:], language="bash")
        else:
            st.success("Build successful. Starting pluginval…")
            with st.spinner("Step 2: Running headless stress test…"):
                test_code, test_out = run_stress_test(target.vst_name)
            st.session_state["last_test_output"] = test_out
            if test_code == 0:
                st.success("Test cycle complete — all stress tests passed.")
            else:
                st.error("Stress test failed. Telemetry written to error_log.txt.")

with col2:
    st.header("2. Diagnostics and telemetry")

    last_test = st.session_state.get("last_test_output", "")
    if last_test:
        st.subheader("Last test run")
        if log_indicates_failure(last_test) and "SUCCESS:" not in last_test:
            st.error("Last run reported failures.")
        else:
            st.success("Last run reported success.")
        st.code(last_test[-12000:], language="bash")

    if ERROR_LOG.is_file():
        log_content = ERROR_LOG.read_text(encoding="utf-8", errors="replace")
        st.subheader("error_log.txt")
        st.error("Test failed — agent handoff log:")
        st.code(log_content[-12000:], language="bash")
    elif BUILD_LOG.is_file() and not ERROR_LOG.is_file():
        st.info("No error_log.txt — last pluginval run passed or not run yet.")
    else:
        st.info("Awaiting first compile and test. No logs yet.")

    if BUILD_LOG.is_file():
        with st.expander("Last CMake output"):
            st.code(BUILD_LOG.read_text(encoding="utf-8", errors="replace")[-8000:], language="bash")

st.divider()
st.header("3. AI resolution protocol")
st.write(
    "If telemetry shows a failure, copy the log and use this prompt in Cursor, Claude Code, or AI Studio:"
)
st.info(AI_PROMPT)

st.caption(
    "CLI equivalent: `cmake --build build -j --target "
    f"{TARGETS[0].cmake_target}` then `python3 vst-testing-ops/test_runner.py`"
)
