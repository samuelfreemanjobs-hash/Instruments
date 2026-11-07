from __future__ import annotations

import subprocess
import sys
import textwrap
from dataclasses import dataclass
from pathlib import Path

from coding_agent_stack.config import get_settings


@dataclass
class ExecResult:
    stdout: str
    stderr: str
    exit_code: int

    def as_tool_text(self) -> str:
        parts = []
        if self.stdout:
            parts.append(self.stdout.rstrip())
        if self.stderr:
            parts.append(f"[stderr]\n{self.stderr.rstrip()}")
        parts.append(f"[exit_code={self.exit_code}]")
        return "\n".join(parts)


class LocalSandbox:
    """Isolated directory under tools/coding-agent-stack/.sandbox (gitignored)."""

    def __init__(self, root: Path | None = None) -> None:
        settings = get_settings()
        base = root or settings.repo_root / "tools/coding-agent-stack/.sandbox"
        base.mkdir(parents=True, exist_ok=True)
        self.root = base.resolve()

    def run_python(self, code: str, timeout_s: int = 60) -> ExecResult:
        script = textwrap.dedent(code).strip()
        proc = subprocess.run(
            [sys.executable, "-c", script],
            cwd=self.root,
            capture_output=True,
            text=True,
            timeout=timeout_s,
        )
        return ExecResult(proc.stdout, proc.stderr, proc.returncode)


class E2BSandbox:
    """Optional cloud micro-VM when e2b-code-interpreter is installed and keyed."""

    def __init__(self) -> None:
        settings = get_settings()
        if not settings.e2b_api_key:
            raise RuntimeError("E2B_API_KEY is not set")
        self._settings = settings

    def run_python(self, code: str, timeout_s: int = 120) -> ExecResult:
        try:
            from e2b_code_interpreter import Sandbox
        except ImportError as exc:
            raise RuntimeError("Install optional extra: pip install 'coding-agent-stack[e2b]'") from exc

        sandbox = Sandbox(api_key=self._settings.e2b_api_key)
        try:
            execution = sandbox.run_code(code, timeout=timeout_s)
            if execution.error:
                return ExecResult(
                    "",
                    f"{execution.error.name}: {execution.error.value}",
                    1,
                )
            text = execution.text or ""
            return ExecResult(text, "", 0)
        finally:
            sandbox.kill()


def get_code_sandbox(prefer_e2b: bool = False) -> LocalSandbox | E2BSandbox:
    settings = get_settings()
    if prefer_e2b and settings.e2b_api_key:
        return E2BSandbox()
    return LocalSandbox()
