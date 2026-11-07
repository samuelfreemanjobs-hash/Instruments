from __future__ import annotations

import re
import subprocess
from pathlib import Path
from typing import Any, Callable

from coding_agent_stack.config import get_settings
from coding_agent_stack.harness import run_pytest_smoke, run_vst_autopilot_compile
from coding_agent_stack.sandbox import get_code_sandbox

ToolFn = Callable[..., str]

ALLOWED_SHELL_PREFIXES = (
    "cmake ",
    "python3 ",
    "pytest ",
    "git status",
    "git diff",
    "git log",
    "./scripts/vst/autopilot-compile",
    "npm run build",
    "npm test",
)


def _resolve_repo_path(rel: str) -> Path:
    settings = get_settings()
    root = settings.repo_root.resolve()
    target = (root / rel).resolve()
    if not str(target).startswith(str(root)):
        raise ValueError("path escapes repo root")
    return target


def tool_read_file(rel_path: str, max_bytes: int = 50_000) -> str:
    p = _resolve_repo_path(rel_path)
    if not p.is_file():
        return f"Error: not a file: {rel_path}"
    data = p.read_bytes()[:max_bytes]
    text = data.decode("utf-8", errors="replace")
    if len(data) >= max_bytes:
        text += "\n...[truncated]"
    return text


def tool_grep_repo(pattern: str, glob: str = "*", max_matches: int = 40) -> str:
    settings = get_settings()
    root = settings.repo_root
    try:
        proc = subprocess.run(
            [
                "rg",
                "-n",
                "--max-count",
                str(max_matches),
                "--glob",
                glob,
                pattern,
                str(root),
            ],
            capture_output=True,
            text=True,
            timeout=30,
        )
    except FileNotFoundError:
        return "Error: ripgrep (rg) not installed"
    out = (proc.stdout or proc.stderr or "").strip()
    if not out:
        return "(no matches)"
    return out[:12_000]


def tool_run_safe_shell(command: str, timeout_s: int = 120) -> str:
    cmd = command.strip()
    if not any(cmd.startswith(p) for p in ALLOWED_SHELL_PREFIXES):
        return (
            "Error: command not allowlisted. Permitted prefixes: "
            + ", ".join(ALLOWED_SHELL_PREFIXES)
        )
    settings = get_settings()
    proc = subprocess.run(
        cmd,
        shell=True,
        cwd=settings.repo_root,
        capture_output=True,
        text=True,
        timeout=timeout_s,
    )
    text = (proc.stdout or "") + "\n" + (proc.stderr or "")
    return f"{text.strip()}\n[exit={proc.returncode}]"


def tool_execute_python(code: str, use_e2b: bool = False) -> str:
    sandbox = get_code_sandbox(prefer_e2b=use_e2b)
    return sandbox.run_python(code).as_tool_text()


def tool_harness_vst_compile(target: str = "") -> str:
    args = ["--no-configure"]
    if target:
        args.extend(["--target", target])
    result = run_vst_autopilot_compile(args)
    status = "OK" if result.ok else "FAILED"
    return f"[harness {status}] {result.command}\n{result.output}"


def tool_harness_agent_tests() -> str:
    result = run_pytest_smoke()
    status = "OK" if result.ok else "FAILED"
    return f"[harness {status}]\n{result.output}"


TOOL_REGISTRY: dict[str, tuple[str, ToolFn, dict[str, Any]]] = {
    "read_file": (
        "Read a UTF-8 file relative to repo root",
        tool_read_file,
        {
            "type": "object",
            "properties": {
                "rel_path": {"type": "string"},
                "max_bytes": {"type": "integer", "default": 50000},
            },
            "required": ["rel_path"],
        },
    ),
    "grep_repo": (
        "Search repo with ripgrep",
        tool_grep_repo,
        {
            "type": "object",
            "properties": {
                "pattern": {"type": "string"},
                "glob": {"type": "string", "default": "*"},
            },
            "required": ["pattern"],
        },
    ),
    "run_safe_shell": (
        "Run allowlisted shell command in repo root",
        tool_run_safe_shell,
        {
            "type": "object",
            "properties": {"command": {"type": "string"}},
            "required": ["command"],
        },
    ),
    "execute_python": (
        "Execute Python in isolated sandbox directory",
        tool_execute_python,
        {
            "type": "object",
            "properties": {
                "code": {"type": "string"},
                "use_e2b": {"type": "boolean", "default": False},
            },
            "required": ["code"],
        },
    ),
    "harness_vst_compile": (
        "Run VST autopilot compile harness (cmake build)",
        tool_harness_vst_compile,
        {
            "type": "object",
            "properties": {"target": {"type": "string", "default": ""}},
        },
    ),
    "harness_agent_tests": (
        "Run coding-agent-stack pytest smoke",
        tool_harness_agent_tests,
        {"type": "object", "properties": {}},
    ),
}


def openai_tools_spec() -> list[dict[str, Any]]:
    return [
        {
            "type": "function",
            "function": {
                "name": name,
                "description": desc,
                "parameters": schema,
            },
        }
        for name, (desc, _fn, schema) in TOOL_REGISTRY.items()
    ]


def dispatch_tool(name: str, args: dict[str, Any]) -> str:
    if name not in TOOL_REGISTRY:
        return f"Error: unknown tool {name}"
    _desc, fn, _schema = TOOL_REGISTRY[name]
    try:
        return fn(**args)
    except TypeError:
        # tolerate extra keys from models
        import inspect

        sig = inspect.signature(fn)
        filtered = {k: v for k, v in args.items() if k in sig.parameters}
        return fn(**filtered)
    except Exception as exc:  # noqa: BLE001 — tool boundary
        return f"Error: {type(exc).__name__}: {exc}"


def stub_plan_for_prompt(prompt: str) -> list[tuple[str, dict[str, Any]]]:
    """No-API-key mode: deterministic tool plan for harness/health checks."""
    lower = prompt.lower()
    plan: list[tuple[str, dict[str, Any]]] = []
    if "compile" in lower or "cmake" in lower or "vst" in lower:
        plan.append(("harness_vst_compile", {"target": ""}))
    elif "test" in lower and "agent" in lower:
        plan.append(("harness_agent_tests", {}))
    elif m := re.search(r"read\s+(\S+)", lower):
        plan.append(("read_file", {"rel_path": m.group(1)}))
    elif "grep" in lower or "search" in lower:
        plan.append(("grep_repo", {"pattern": "processBlock", "glob": "*.cpp"}))
    else:
        plan.append(("harness_agent_tests", {}))
    return plan
