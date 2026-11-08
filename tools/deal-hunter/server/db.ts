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

  CREATE TABLE IF NOT EXISTS community_deals (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    retailer TEXT NOT NULL,
    price REAL NOT NULL,
    url TEXT NOT NULL,
    zip TEXT,
    category TEXT NOT NULL,
    note TEXT,
    source TEXT NOT NULL DEFAULT 'community',
    upvotes INTEGER NOT NULL DEFAULT 1,
  status TEXT NOT NULL DEFAULT 'approved',
    created_at TEXT NOT NULL
  );

  CREATE TABLE IF NOT EXISTS push_subscriptions (
    id TEXT PRIMARY KEY,
    endpoint TEXT NOT NULL UNIQUE,
    p256dh TEXT NOT NULL,
    auth TEXT NOT NULL,
    email TEXT,
    notify_email INTEGER NOT NULL DEFAULT 0,
    created_at TEXT NOT NULL
  );

  CREATE TABLE IF NOT EXISTS agent_run_log (
    id TEXT PRIMARY KEY,
    agent_id TEXT NOT NULL,
    summary TEXT NOT NULL,
    payload_json TEXT NOT NULL,
    created_at TEXT NOT NULL
  );
`);
