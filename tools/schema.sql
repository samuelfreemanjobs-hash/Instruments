-- Disklordz catalog (single database; default path tools/data/disklordz.db)

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS market_gaps (
    id TEXT PRIMARY KEY,
    issue TEXT NOT NULL,
    severity INTEGER NOT NULL DEFAULT 5,
    status TEXT NOT NULL DEFAULT 'OPEN',
    timestamp TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS spectral_references (
    ref_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    source_path TEXT,
    avg_rms REAL NOT NULL,
    crest_factor REAL NOT NULL,
    zero_crossing_rate REAL NOT NULL,
    sub_peak_hz REAL,
    timestamp TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS product_ledger (
    product_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    price REAL,
    hardware_type TEXT,
    status TEXT NOT NULL DEFAULT 'DRAFTED',
    sales_count INTEGER NOT NULL DEFAULT 0,
    max_supply INTEGER NOT NULL DEFAULT 50,
    dna_profile TEXT,
    live_url TEXT
);

CREATE TABLE IF NOT EXISTS batch_runs (
    batch_id TEXT PRIMARY KEY,
    kit_name TEXT NOT NULL,
    notes TEXT,
    sample_count INTEGER NOT NULL DEFAULT 0,
    timestamp TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS sample_dna (
    sample_id TEXT PRIMARY KEY,
    batch_id TEXT,
    voice TEXT NOT NULL,
    hardware_source TEXT,
    freq_peak REAL,
    decay_sec REAL,
    grit_level REAL,
    bit_depth INTEGER,
    sample_rate INTEGER NOT NULL DEFAULT 44100,
    rms REAL,
    crest_factor REAL,
    zero_crossing_rate REAL,
    sub_peak_hz REAL,
    match_score REAL,
    vibe_score REAL,
    wave_path TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'RENDERED',
    created_at TEXT NOT NULL,
    FOREIGN KEY (batch_id) REFERENCES batch_runs(batch_id)
);

CREATE INDEX IF NOT EXISTS idx_sample_dna_batch ON sample_dna(batch_id);
CREATE INDEX IF NOT EXISTS idx_sample_dna_voice ON sample_dna(voice);
