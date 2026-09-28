"""Safe project context loading for design reviews."""

from __future__ import annotations

from pathlib import Path

DEFAULT_MAX_BYTES = 120_000
SKIP_DIR_NAMES = {
    ".git",
    "node_modules",
    "build",
    "dist",
    "__pycache__",
    ".venv",
    "venv",
}
SKIP_FILE_SUFFIXES = {".wav", ".mp3", ".png", ".jpg", ".jpeg", ".gif", ".zip", ".rom"}


def _should_skip(path: Path) -> bool:
    for part in path.parts:
        if part in SKIP_DIR_NAMES:
            return True
    if path.suffix.lower() in SKIP_FILE_SUFFIXES:
        return True
    name = path.name
    if name.startswith(".env") or name.endswith(".pem") or name.endswith(".key"):
        return True
    return False


def read_context_paths(
    paths: list[Path],
    *,
    max_bytes: int = DEFAULT_MAX_BYTES,
) -> str:
    """Concatenate text files for the model; enforce a total byte budget."""
    chunks: list[str] = []
    used = 0
    for raw in paths:
        path = raw.resolve()
        if not path.is_file() or _should_skip(path):
            continue
        try:
            data = path.read_bytes()
        except OSError:
            continue
        if used + len(data) > max_bytes:
            remain = max_bytes - used
            if remain <= 0:
                break
            data = data[:remain]
        try:
            text = data.decode("utf-8")
        except UnicodeDecodeError:
            text = data.decode("utf-8", errors="replace")
        chunks.append(f"--- FILE: {path} ---\n{text}\n")
        used += len(data)
        if used >= max_bytes:
            break
    return "\n".join(chunks)


def expand_paths(entries: list[str], repo_root: Path | None = None) -> list[Path]:
    out: list[Path] = []
    root = repo_root or Path.cwd()
    for entry in entries:
        p = Path(entry)
        if not p.is_absolute():
            p = root / p
        if p.is_dir():
            for child in sorted(p.rglob("*")):
                if child.is_file() and not _should_skip(child):
                    out.append(child)
        elif p.is_file():
            out.append(p)
    return out
