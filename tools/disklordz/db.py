"""SQLite helpers for the Disklordz catalog."""

from __future__ import annotations

import sqlite3
import uuid
from datetime import datetime, timezone
from pathlib import Path

SCHEMA_REL = Path(__file__).resolve().parent.parent / "schema.sql"


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat()


def connect(db_path: Path) -> sqlite3.Connection:
    db_path.parent.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


def init_db(db_path: Path) -> None:
    sql = SCHEMA_REL.read_text(encoding="utf-8")
    with connect(db_path) as conn:
        conn.executescript(sql)
        conn.commit()


def new_id(prefix: str) -> str:
    return f"{prefix}_{uuid.uuid4().hex[:10]}"


def insert_batch(conn: sqlite3.Connection, kit_name: str, notes: str | None = None) -> str:
    batch_id = new_id("batch")
    conn.execute(
        "INSERT INTO batch_runs (batch_id, kit_name, notes, sample_count, timestamp) VALUES (?, ?, ?, 0, ?)",
        (batch_id, kit_name, notes, utc_now()),
    )
    return batch_id


def insert_sample(
    conn: sqlite3.Connection,
    *,
    batch_id: str,
    voice: str,
    wave_path: str,
    hardware_source: str | None,
    freq_peak: float | None,
    decay_sec: float | None,
    grit_level: float | None,
    bit_depth: int | None,
    sample_rate: int,
    metrics: dict[str, float],
    match_score: float | None = None,
) -> str:
    sample_id = new_id("smp")
    conn.execute(
        """
        INSERT INTO sample_dna (
            sample_id, batch_id, voice, hardware_source, freq_peak, decay_sec,
            grit_level, bit_depth, sample_rate, rms, crest_factor, zero_crossing_rate,
            sub_peak_hz, match_score, wave_path, status, created_at
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 'RENDERED', ?)
        """,
        (
            sample_id,
            batch_id,
            voice,
            hardware_source,
            freq_peak,
            decay_sec,
            grit_level,
            bit_depth,
            sample_rate,
            metrics.get("rms"),
            metrics.get("crest_factor"),
            metrics.get("zero_crossing_rate"),
            metrics.get("sub_peak_hz"),
            match_score,
            wave_path,
            utc_now(),
        ),
    )
    conn.execute(
        "UPDATE batch_runs SET sample_count = sample_count + 1 WHERE batch_id = ?",
        (batch_id,),
    )
    return sample_id


def insert_reference(
    conn: sqlite3.Connection,
    name: str,
    source_path: str,
    metrics: dict[str, float],
) -> str:
    ref_id = new_id("ref")
    conn.execute(
        """
        INSERT INTO spectral_references (
            ref_id, name, source_path, avg_rms, crest_factor, zero_crossing_rate, sub_peak_hz, timestamp
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """,
        (
            ref_id,
            name,
            source_path,
            metrics["rms"],
            metrics["crest_factor"],
            metrics["zero_crossing_rate"],
            metrics.get("sub_peak_hz", 0.0),
            utc_now(),
        ),
    )
    return ref_id


def mean_reference_metrics(conn: sqlite3.Connection) -> dict[str, float] | None:
    row = conn.execute(
        """
        SELECT AVG(avg_rms) AS rms, AVG(crest_factor) AS crest_factor,
               AVG(zero_crossing_rate) AS zero_crossing_rate
        FROM spectral_references
        """
    ).fetchone()
    if row is None or row["rms"] is None:
        return None
    return {
        "rms": float(row["rms"]),
        "crest_factor": float(row["crest_factor"]),
        "zero_crossing_rate": float(row["zero_crossing_rate"]),
    }
