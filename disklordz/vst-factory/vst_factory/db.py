from __future__ import annotations

import json
import sqlite3
from contextlib import contextmanager
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterator


def _utc_now() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


class FactoryDB:
    def __init__(self, path: str | Path) -> None:
        self.path = Path(path)
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self._conn = sqlite3.connect(str(self.path), check_same_thread=False)
        self._conn.row_factory = sqlite3.Row
        self._init_schema()

    def close(self) -> None:
        self._conn.close()

    def _init_schema(self) -> None:
        self._conn.executescript(
            """
            CREATE TABLE IF NOT EXISTS projects (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                plugin_name TEXT NOT NULL UNIQUE,
                plugin_class TEXT NOT NULL,
                state TEXT NOT NULL,
                spec_json TEXT NOT NULL,
                apc_path TEXT,
                vst_path TEXT,
                repair_attempts INTEGER NOT NULL DEFAULT 0,
                created_at TEXT NOT NULL,
                updated_at TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS runs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                project_id INTEGER NOT NULL,
                kind TEXT NOT NULL,
                ok INTEGER NOT NULL,
                output_text TEXT NOT NULL,
                created_at TEXT NOT NULL,
                FOREIGN KEY (project_id) REFERENCES projects(id)
            );
            """
        )
        self._conn.commit()

    @contextmanager
    def transaction(self) -> Iterator[sqlite3.Connection]:
        try:
            yield self._conn
            self._conn.commit()
        except Exception:
            self._conn.rollback()
            raise

    def insert_project(
        self,
        *,
        plugin_name: str,
        plugin_class: str,
        state: str,
        spec: dict[str, Any],
        apc_path: str | None = None,
        vst_path: str | None = None,
    ) -> int:
        now = _utc_now()
        with self.transaction():
            cur = self._conn.execute(
                """
                INSERT INTO projects
                (plugin_name, plugin_class, state, spec_json, apc_path, vst_path, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                """,
                (
                    plugin_name,
                    plugin_class,
                    state,
                    json.dumps(spec),
                    apc_path,
                    vst_path,
                    now,
                    now,
                ),
            )
            return int(cur.lastrowid)

    def get_project(self, project_id: int) -> dict[str, Any] | None:
        row = self._conn.execute("SELECT * FROM projects WHERE id = ?", (project_id,)).fetchone()
        if not row:
            return None
        return self._row_project(row)

    def get_project_by_name(self, name: str) -> dict[str, Any] | None:
        row = self._conn.execute("SELECT * FROM projects WHERE plugin_name = ?", (name,)).fetchone()
        if not row:
            return None
        return self._row_project(row)

    def list_projects(self, limit: int = 50) -> list[dict[str, Any]]:
        rows = self._conn.execute(
            "SELECT * FROM projects ORDER BY id DESC LIMIT ?",
            (limit,),
        ).fetchall()
        return [self._row_project(r) for r in rows]

    def update_project(
        self,
        project_id: int,
        *,
        state: str | None = None,
        spec: dict[str, Any] | None = None,
        apc_path: str | None = None,
        vst_path: str | None = None,
        repair_attempts: int | None = None,
    ) -> None:
        proj = self.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        now = _utc_now()
        new_state = state if state is not None else proj["state"]
        new_spec = json.dumps(spec) if spec is not None else json.dumps(proj["spec"])
        new_apc = apc_path if apc_path is not None else proj["apc_path"]
        new_vst = vst_path if vst_path is not None else proj["vst_path"]
        attempts = repair_attempts if repair_attempts is not None else proj["repair_attempts"]
        with self.transaction():
            self._conn.execute(
                """
                UPDATE projects SET state=?, spec_json=?, apc_path=?, vst_path=?,
                repair_attempts=?, updated_at=? WHERE id=?
                """,
                (new_state, new_spec, new_apc, new_vst, attempts, now, project_id),
            )

    def add_run(self, project_id: int, kind: str, ok: bool, output: str) -> int:
        tail = output[-50000:] if len(output) > 50000 else output
        with self.transaction():
            cur = self._conn.execute(
                """
                INSERT INTO runs (project_id, kind, ok, output_text, created_at)
                VALUES (?, ?, ?, ?, ?)
                """,
                (project_id, kind, 1 if ok else 0, tail, _utc_now()),
            )
            return int(cur.lastrowid)

    def list_runs(self, project_id: int, limit: int = 20) -> list[dict[str, Any]]:
        rows = self._conn.execute(
            "SELECT * FROM runs WHERE project_id=? ORDER BY id DESC LIMIT ?",
            (project_id, limit),
        ).fetchall()
        return [
            {
                "id": r["id"],
                "project_id": r["project_id"],
                "kind": r["kind"],
                "ok": bool(r["ok"]),
                "output_tail": (r["output_text"] or "")[-4000:],
                "created_at": r["created_at"],
            }
            for r in rows
        ]

    def count_greenfield(self) -> int:
        row = self._conn.execute(
            "SELECT COUNT(*) AS c FROM projects WHERE json_extract(spec_json, '$.track') = 'apc_greenfield'"
        ).fetchone()
        return int(row["c"]) if row else 0

    @staticmethod
    def _row_project(row: sqlite3.Row) -> dict[str, Any]:
        return {
            "id": row["id"],
            "plugin_name": row["plugin_name"],
            "plugin_class": row["plugin_class"],
            "state": row["state"],
            "spec": json.loads(row["spec_json"]),
            "apc_path": row["apc_path"],
            "vst_path": row["vst_path"],
            "repair_attempts": row["repair_attempts"],
            "created_at": row["created_at"],
            "updated_at": row["updated_at"],
        }
