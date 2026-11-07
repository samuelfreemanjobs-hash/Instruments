from __future__ import annotations

import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

from coding_agent_stack.config import get_settings


@dataclass
class HarnessResult:
    ok: bool
    command: str
    output: str
    error_log_path: Path | None


def run_vst_autopilot_compile(extra_args: list[str] | None = None, timeout_s: int = 600) -> HarnessResult:
    """Run scripts/vst/autopilot-compile.sh; on failure surfaces vst-testing-ops/error_log.txt."""
    settings = get_settings()
    repo = settings.repo_root
    script = repo / "scripts/vst/autopilot-compile.sh"
    if not script.is_file():
        return HarnessResult(
            False,
            str(script),
            "autopilot-compile.sh not found — is REPO_ROOT correct?",
            None,
        )

    cmd = [str(script)] + (extra_args or [])
    proc = subprocess.run(
        cmd,
        cwd=repo,
        capture_output=True,
        text=True,
        timeout=timeout_s,
    )
    error_log = repo / "vst-testing-ops/error_log.txt"
    chunks = [proc.stdout or "", proc.stderr or ""]
    if proc.returncode != 0 and error_log.is_file():
        chunks.append("--- error_log.txt ---\n")
        chunks.append(error_log.read_text(encoding="utf-8", errors="replace")[-8000:])
    output = "\n".join(c for c in chunks if c).strip()
    return HarnessResult(proc.returncode == 0, " ".join(cmd), output, error_log if error_log.is_file() else None)


def run_pytest_smoke(timeout_s: int = 120) -> HarnessResult:
    """Quick agent-stack self-test (not full monorepo CI)."""
    settings = get_settings()
    pkg = settings.repo_root / "tools/coding-agent-stack"
    cmd = [sys.executable, "-m", "pytest", "tests", "-q", "--tb=short"]
    proc = subprocess.run(cmd, cwd=pkg, capture_output=True, text=True, timeout=timeout_s)
    output = (proc.stdout or "") + "\n" + (proc.stderr or "")
    return HarnessResult(proc.returncode == 0, " ".join(cmd), output.strip(), None)
