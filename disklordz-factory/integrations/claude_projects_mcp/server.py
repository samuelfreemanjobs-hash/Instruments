#!/usr/bin/env python3
"""
MCP server: browse Claude Code project workspaces on disk (~/.claude/projects).

Use from Cursor via .cursor/mcp.json so agents can find repos created in Claude Code
(e.g. sp-1200) and read/copy files into this monorepo.
"""

from __future__ import annotations

import json
import os
from pathlib import Path
from typing import Any

from mcp.server.fastmcp import FastMCP

DEFAULT_ROOT = Path.home() / ".claude" / "projects"
MAX_READ_BYTES = 512_000

mcp = FastMCP(
    "claude-projects",
    instructions=(
        "Browse Claude Code on-disk project directories. Use list_claude_projects to "
        "find workspaces, then list_claude_project_files and read_claude_project_file."
    ),
)


def _projects_root() -> Path:
    raw = os.environ.get("CLAUDE_PROJECTS_ROOT", str(DEFAULT_ROOT))
    return Path(raw).expanduser()


def _resolve_project(project_id: str) -> Path:
    root = _projects_root()
    candidate = (root / project_id).resolve()
    if not str(candidate).startswith(str(root.resolve())):
        raise ValueError("invalid project path")
    if not candidate.is_dir():
        raise FileNotFoundError(f"project not found: {project_id}")
    return candidate


def _resolve_file(project_id: str, rel_path: str) -> Path:
    project = _resolve_project(project_id)
    target = (project / rel_path).resolve()
    if not str(target).startswith(str(project.resolve())):
        raise ValueError("path escapes project root")
    if not target.is_file():
        raise FileNotFoundError(rel_path)
    return target


@mcp.tool()
def claude_projects_root() -> str:
    """Return the directory scanned for Claude Code project workspaces."""
    return str(_projects_root())


@mcp.tool()
def list_claude_projects(limit: int = 50) -> list[dict[str, Any]]:
    """List Claude Code project folders (newest mtime first)."""
    root = _projects_root()
    if not root.is_dir():
        return []
    entries: list[dict[str, Any]] = []
    for child in sorted(root.iterdir(), key=lambda p: p.stat().st_mtime, reverse=True):
        if not child.is_dir():
            continue
        meta = {"project_id": child.name, "path": str(child)}
        readme = child / "README.md"
        if readme.is_file():
            meta["readme_preview"] = readme.read_text(encoding="utf-8", errors="replace")[:400]
        entries.append(meta)
        if len(entries) >= max(1, min(limit, 200)):
            break
    return entries


@mcp.tool()
def search_claude_projects(query: str, limit: int = 20) -> list[dict[str, Any]]:
    """Find Claude project folders whose names or file paths match query (case-insensitive)."""
    q = query.lower().strip()
    if not q:
        return []
    root = _projects_root()
    if not root.is_dir():
        return []
    hits: list[dict[str, Any]] = []
    for child in root.iterdir():
        if not child.is_dir():
            continue
        if q in child.name.lower():
            hits.append({"project_id": child.name, "path": str(child), "match": "project_id"})
            continue
        for path in child.rglob("*"):
            if len(hits) >= limit:
                break
            if q in str(path.relative_to(child)).lower():
                hits.append(
                    {
                        "project_id": child.name,
                        "path": str(child),
                        "match": str(path.relative_to(child)),
                    }
                )
        if len(hits) >= limit:
            break
    return hits[:limit]


@mcp.tool()
def list_claude_project_files(project_id: str, subpath: str = "", limit: int = 200) -> list[str]:
    """List relative file paths under a Claude Code project (non-hidden, skip .git objects)."""
    project = _resolve_project(project_id)
    base = (project / subpath).resolve() if subpath else project
    if not str(base).startswith(str(project.resolve())):
        raise ValueError("subpath escapes project")
    if not base.is_dir():
        raise NotADirectoryError(subpath or ".")
    out: list[str] = []
    for path in base.rglob("*"):
        if not path.is_file():
            continue
        rel = str(path.relative_to(project))
        if "/.git/" in f"/{rel}/" or rel.startswith(".git/"):
            continue
        out.append(rel)
        if len(out) >= max(1, min(limit, 1000)):
            break
    return sorted(out)


@mcp.tool()
def read_claude_project_file(project_id: str, path: str, max_bytes: int = MAX_READ_BYTES) -> str:
    """Read a UTF-8 text file from a Claude Code project workspace."""
    target = _resolve_file(project_id, path)
    cap = max(1024, min(max_bytes, MAX_READ_BYTES))
    data = target.read_bytes()[:cap]
    return data.decode("utf-8", errors="replace")


@mcp.tool()
def export_claude_project_manifest(project_id: str) -> str:
    """JSON manifest of project_id, root path, and file listing (for import planning)."""
    project = _resolve_project(project_id)
    files = list_claude_project_files(project_id=project_id, limit=1000)
    return json.dumps(
        {"project_id": project_id, "root": str(project), "file_count": len(files), "files": files},
        indent=2,
    )


if __name__ == "__main__":
    mcp.run()
