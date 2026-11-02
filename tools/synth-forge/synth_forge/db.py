from __future__ import annotations

import sqlite3
from pathlib import Path

from synth_forge.models import BatchRun, Preset, Rating

DEFAULT_DB = Path(__file__).resolve().parent.parent / "synth_memory.db"


class SynthMemory:
    def __init__(self, path: Path | str | None = None) -> None:
        self.path = Path(path or DEFAULT_DB)
        self._conn: sqlite3.Connection | None = None

    def connect(self) -> sqlite3.Connection:
        if self._conn is None:
            self._conn = sqlite3.connect(self.path, check_same_thread=False)
            self._conn.row_factory = sqlite3.Row
            self._conn.execute("PRAGMA journal_mode=WAL")
            self._init_schema()
        return self._conn

    def _init_schema(self) -> None:
        assert self._conn
        self._conn.executescript(
            """
            CREATE TABLE IF NOT EXISTS presets (
                id TEXT PRIMARY KEY,
                name TEXT NOT NULL,
                synth_id TEXT NOT NULL,
                json TEXT NOT NULL,
                rating TEXT DEFAULT 'unset',
                parent_ids TEXT DEFAULT '[]',
                created_at TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS batch_runs (
                id TEXT PRIMARY KEY,
                synth_id TEXT NOT NULL,
                count INTEGER NOT NULL,
                preset_ids TEXT NOT NULL,
                created_at TEXT NOT NULL
            );
            """
        )
        self._conn.commit()

    def save_preset(self, preset: Preset) -> Preset:
        import json

        conn = self.connect()
        conn.execute(
            """
            INSERT OR REPLACE INTO presets (id, name, synth_id, json, rating, parent_ids, created_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
            """,
            (
                preset.id,
                preset.name,
                preset.synth_id,
                preset.model_dump_json(),
                preset.rating,
                json.dumps(preset.parent_ids),
                preset.created_at.isoformat(),
            ),
        )
        conn.commit()
        return preset

    def save_presets(self, presets: list[Preset]) -> None:
        import json

        conn = self.connect()
        for preset in presets:
            conn.execute(
                """
                INSERT OR REPLACE INTO presets (id, name, synth_id, json, rating, parent_ids, created_at)
                VALUES (?, ?, ?, ?, ?, ?, ?)
                """,
                (
                    preset.id,
                    preset.name,
                    preset.synth_id,
                    preset.model_dump_json(),
                    preset.rating,
                    json.dumps(preset.parent_ids),
                    preset.created_at.isoformat(),
                ),
            )
        conn.commit()

    def list_presets(self, limit: int = 100) -> list[Preset]:
        conn = self.connect()
        rows = conn.execute(
            "SELECT json, rating FROM presets ORDER BY created_at DESC LIMIT ?",
            (limit,),
        ).fetchall()
        out: list[Preset] = []
        for r in rows:
            preset = Preset.model_validate_json(r["json"])
            if r["rating"] and r["rating"] != "unset":
                preset = preset.model_copy(update={"rating": r["rating"]})
            out.append(preset)
        return out

    def set_rating(self, preset_id: str, rating: Rating) -> bool:
        conn = self.connect()
        row = conn.execute("SELECT json FROM presets WHERE id = ?", (preset_id,)).fetchone()
        if row is None:
            return False
        preset = Preset.model_validate_json(row["json"])
        preset = preset.model_copy(update={"rating": rating})
        conn.execute(
            "UPDATE presets SET rating = ?, json = ? WHERE id = ?",
            (rating, preset.model_dump_json(), preset_id),
        )
        conn.commit()
        return True

    def save_batch(self, batch: BatchRun) -> None:
        import json

        conn = self.connect()
        conn.execute(
            """
            INSERT INTO batch_runs (id, synth_id, count, preset_ids, created_at)
            VALUES (?, ?, ?, ?, ?)
            """,
            (
                batch.id,
                batch.synth_id,
                batch.count,
                json.dumps(batch.preset_ids),
                batch.created_at.isoformat(),
            ),
        )
        conn.commit()

    def close(self) -> None:
        if self._conn:
            self._conn.close()
            self._conn = None
