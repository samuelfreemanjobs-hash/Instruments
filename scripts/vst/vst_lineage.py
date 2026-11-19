#!/usr/bin/env python3
"""SQLite lineage for in-plugin VST presets (VST Preset Agent)."""

from __future__ import annotations

import hashlib
import json
import sqlite3
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

REPO = Path(__file__).resolve().parents[2]
DB_PATH = REPO / "data/vst-preset-lineage/presets.sqlite"
ACTIVE_SEEDS = REPO / "data/vst-preset-lineage/active_seeds.json"

MAX_ACTIVE_PER_PRODUCT = 64
DAILY_PRODUCTS = ("wave9090", "pluggva", "jd-upgraded")


@dataclass(frozen=True)
class PresetRecord:
    product: str
    name: str
    category: str
    param_hash: str
    params_json: str
    source: str
    batch_id: str | None = None


class VstLineage:
    def __init__(self, path: Path | None = None) -> None:
        self.path = path or DB_PATH
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self._conn = sqlite3.connect(self.path)
        self._conn.row_factory = sqlite3.Row
        self._conn.execute("PRAGMA journal_mode=WAL")
        self._init()

    def _init(self) -> None:
        self._conn.executescript(
            """
            CREATE TABLE IF NOT EXISTS presets (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                product TEXT NOT NULL,
                name TEXT NOT NULL,
                category TEXT NOT NULL,
                param_hash TEXT NOT NULL,
                params_json TEXT NOT NULL,
                source TEXT NOT NULL,
                batch_id TEXT,
                created_at TEXT NOT NULL,
                UNIQUE (product, name),
                UNIQUE (product, param_hash)
            );
            CREATE TABLE IF NOT EXISTS batches (
                batch_id TEXT PRIMARY KEY,
                created_at TEXT NOT NULL,
                note TEXT
            );
            """
        )
        self._conn.commit()

    @staticmethod
    def param_hash(params: dict[str, Any]) -> str:
        canonical = json.dumps(params, sort_keys=True, separators=(",", ":"))
        return hashlib.sha256(canonical.encode("utf-8")).hexdigest()[:32]

    def has_duplicate(self, product: str, name: str, param_hash: str) -> bool:
        row = self._conn.execute(
            """
            SELECT 1 FROM presets
            WHERE product = ? AND (name = ? OR param_hash = ?)
            LIMIT 1
            """,
            (product, name, param_hash),
        ).fetchone()
        return row is not None

    def insert(self, rec: PresetRecord) -> bool:
        if self.has_duplicate(rec.product, rec.name, rec.param_hash):
            return False
        self._conn.execute(
            """
            INSERT INTO presets (product, name, category, param_hash, params_json, source, batch_id, created_at)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                rec.product,
                rec.name,
                rec.category,
                rec.param_hash,
                rec.params_json,
                rec.source,
                rec.batch_id,
                datetime.now(timezone.utc).isoformat(),
            ),
        )
        self._conn.commit()
        return True

    def record_batch(self, batch_id: str, note: str) -> None:
        self._conn.execute(
            "INSERT OR REPLACE INTO batches (batch_id, created_at, note) VALUES (?, ?, ?)",
            (batch_id, datetime.now(timezone.utc).isoformat(), note),
        )
        self._conn.commit()

    def count(self, product: str | None = None) -> int:
        if product:
            row = self._conn.execute(
                "SELECT COUNT(*) AS c FROM presets WHERE product = ?", (product,)
            ).fetchone()
        else:
            row = self._conn.execute("SELECT COUNT(*) AS c FROM presets").fetchone()
        return int(row["c"])

    def close(self) -> None:
        self._conn.close()


def load_active_seeds() -> dict[str, list[dict[str, Any]]]:
    if not ACTIVE_SEEDS.exists():
        return {p: [] for p in DAILY_PRODUCTS}
    data = json.loads(ACTIVE_SEEDS.read_text(encoding="utf-8"))
    for p in DAILY_PRODUCTS:
        data.setdefault(p, [])
    return data


def save_active_seeds(data: dict[str, list[dict[str, Any]]]) -> None:
    ACTIVE_SEEDS.parent.mkdir(parents=True, exist_ok=True)
    for p in DAILY_PRODUCTS:
        items = data.get(p, [])
        if len(items) > MAX_ACTIVE_PER_PRODUCT:
            data[p] = items[-MAX_ACTIVE_PER_PRODUCT:]
    ACTIVE_SEEDS.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
