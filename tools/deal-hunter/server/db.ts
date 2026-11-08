import Database from "better-sqlite3";
import { mkdirSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const dataDir = join(__dirname, "..", "data");
mkdirSync(dataDir, { recursive: true });

const dbPath = process.env.DEAL_HUNTER_DB ?? join(dataDir, "deal-hunter.sqlite");

export const db = new Database(dbPath);

db.exec(`
  CREATE TABLE IF NOT EXISTS shared_lists (
    share_code TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    payload_json TEXT NOT NULL,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
  );

  CREATE TABLE IF NOT EXISTS price_snapshots (
    deal_id TEXT NOT NULL,
    price REAL NOT NULL,
    recorded_at TEXT NOT NULL,
    PRIMARY KEY (deal_id, recorded_at)
  );

  CREATE INDEX IF NOT EXISTS idx_price_deal ON price_snapshots(deal_id);
`);
