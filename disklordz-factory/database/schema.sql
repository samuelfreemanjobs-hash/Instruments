-- DiskLordz catalog (SQLite-compatible; Postgres: use SERIAL/JSONB as needed)

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS artists (
  artist_id TEXT PRIMARY KEY,
  display_name TEXT NOT NULL,
  genres TEXT,
  bpm_min REAL,
  bpm_max REAL,
  lore TEXT,
  visual_system TEXT
);

CREATE TABLE IF NOT EXISTS batches (
  batch_id TEXT PRIMARY KEY,
  mission TEXT NOT NULL,
  target_count INTEGER NOT NULL,
  stage TEXT NOT NULL,
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  updated_at TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS opportunities (
  opportunity_id TEXT PRIMARY KEY,
  title TEXT NOT NULL,
  payload_json TEXT NOT NULL,
  score REAL,
  created_at TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS assets (
  asset_id TEXT PRIMARY KEY,
  kind TEXT NOT NULL,
  title TEXT NOT NULL,
  artist_id TEXT REFERENCES artists(artist_id),
  batch_id TEXT REFERENCES batches(batch_id),
  metadata_json TEXT,
  file_path TEXT,
  rights_status TEXT NOT NULL DEFAULT 'draft',
  youtube_url TEXT,
  streaming_url TEXT,
  product_sku TEXT,
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  updated_at TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS approval_queue (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  asset_id TEXT NOT NULL REFERENCES assets(asset_id),
  audio_qa TEXT DEFAULT 'pending',
  visual_qa TEXT DEFAULT 'pending',
  product_qa TEXT DEFAULT 'pending',
  rights_qa TEXT DEFAULT 'pending',
  status TEXT NOT NULL DEFAULT 'pending',
  approved_by TEXT,
  approved_at TEXT,
  created_at TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS asset_economics (
  asset_id TEXT PRIMARY KEY REFERENCES assets(asset_id),
  cost_usd REAL DEFAULT 0,
  revenue_usd REAL DEFAULT 0,
  notes TEXT
);

-- Seed fictional artist universe (four YouTube lanes + legacy IDs)
-- Full vintage rules: database/artist_collective_seed.json
INSERT OR IGNORE INTO artists (artist_id, display_name, genres, bpm_min, bpm_max, lore) VALUES
  ('DL001', 'DISKLORD 001', 'French touch / vintage disco-funk source', 118, 124, 'Boulevard 86 — Paris robot funk'),
  ('DL002', 'DISKLORD 002', 'Drift phonk / wave phonk / Memphis soul chops', 140, 160, 'Midnight Circuit'),
  ('DL003', 'DISKLORD 003', 'Detroit Electro', 128, 140, 'Legacy lane — not in four-brand network'),
  ('DL004', 'DISKLORD 004', 'Cyber funk / 80s digital funk source', 112, 128, 'Terminal Mirage'),
  ('DL005', 'DISKLORD 005', 'Industrial Trap', 130, 150, 'Legacy lane'),
  ('DL006', 'DISKLORD 006', 'DJ Screw / 90s phonk / slow soul source', 60, 95, 'Disklordz channel — screw');
