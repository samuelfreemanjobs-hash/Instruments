import { customAlphabet } from "nanoid";
import { db } from "./db.js";
import { assessDealUrl } from "./trust.js";

const nanoid = customAlphabet("23456789abcdefghjkmnpqrstuvwxyz", 12);

export interface CommunityDeal {
  id: string;
  title: string;
  retailer: string;
  price: number;
  url: string;
  zip?: string;
  category: string;
  note?: string;
  source: "community";
  upvotes: number;
  status: "approved" | "pending" | "rejected";
  createdAt: string;
}

const SEED: Omit<CommunityDeal, "id">[] = [
  {
    title: "Stanley Quencher 40oz — manager stack",
    retailer: "Target",
    price: 29.99,
    url: "https://www.target.com/",
    category: "home",
    note: "In-store endcap; marked down from $45",
    source: "community",
    upvotes: 42,
    status: "approved",
    createdAt: new Date(Date.now() - 86_400_000).toISOString(),
  },
  {
    title: "Lego Botanical bundle (reader tip)",
    retailer: "Amazon",
    price: 38.5,
    url: "https://www.amazon.com/",
    category: "toys",
    note: "Lightning deal seen 9am ET",
    source: "community",
    upvotes: 31,
    status: "approved",
    createdAt: new Date(Date.now() - 172_800_000).toISOString(),
  },
];

function seedIfEmpty() {
  const row = db.prepare(`SELECT COUNT(*) as c FROM community_deals`).get() as { c: number };
  if (row.c > 0) return;
  const stmt = db.prepare(
    `INSERT INTO community_deals (id, title, retailer, price, url, zip, category, note, source, upvotes, status, created_at)
     VALUES (@id, @title, @retailer, @price, @url, @zip, @category, @note, @source, @upvotes, @status, @createdAt)`,
  );
  for (const s of SEED) {
    stmt.run({ ...s, id: nanoid(), zip: null });
  }
}

export function listCommunityDeals(limit = 24): CommunityDeal[] {
  seedIfEmpty();
  const rows = db
    .prepare(
      `SELECT id, title, retailer, price, url, zip, category, note, source, upvotes, status, created_at as createdAt
       FROM community_deals WHERE status = 'approved' ORDER BY upvotes DESC, created_at DESC LIMIT ?`,
    )
    .all(limit) as CommunityDeal[];
  return rows;
}

export function submitCommunityDeal(input: {
  title: string;
  retailer: string;
  price: number;
  url: string;
  zip?: string;
  category: string;
  note?: string;
}): { ok: true; deal: CommunityDeal } | { ok: false; error: string } {
  if (!assessDealUrl(input.url).safeUrl) {
    return { ok: false, error: "url_not_on_trusted_allowlist" };
  }
  const deal: CommunityDeal = {
    id: nanoid(),
    title: input.title.slice(0, 160),
    retailer: input.retailer.slice(0, 80),
    price: Math.max(0, input.price),
    url: input.url,
    zip: input.zip?.slice(0, 10),
    category: input.category.slice(0, 40) || "general",
    note: input.note?.slice(0, 280),
    source: "community",
    upvotes: 1,
    status: "approved",
    createdAt: new Date().toISOString(),
  };
  db.prepare(
    `INSERT INTO community_deals (id, title, retailer, price, url, zip, category, note, source, upvotes, status, created_at)
     VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
  ).run(
    deal.id,
    deal.title,
    deal.retailer,
    deal.price,
    deal.url,
    deal.zip ?? null,
    deal.category,
    deal.note ?? null,
    deal.source,
    deal.upvotes,
    deal.status,
    deal.createdAt,
  );
  return { ok: true, deal };
}

export function upvoteCommunityDeal(id: string): boolean {
  const r = db.prepare(`UPDATE community_deals SET upvotes = upvotes + 1 WHERE id = ?`).run(id);
  return r.changes > 0;
}
