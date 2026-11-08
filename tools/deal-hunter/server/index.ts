import cors from "cors";
import express from "express";
import { customAlphabet } from "nanoid";
import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";
import { CATALOG, type DealType } from "./catalog.js";
import { runDecorAgent } from "./agents/decor-agent.js";
import { runPricingAgent } from "./agents/pricing-agent.js";
import { buildDashboard } from "./dashboard.js";
import { liveEnrichedDeals } from "./deal-pipeline.js";
import {
  buildBriefing,
  analyzeDealIntelligence,
  intelligenceForQuery,
} from "./intelligence/engine.js";
import { detectSeason, type ShoppingSeason } from "./season.js";
import {
  universalSearch,
  visualTagsFromRgb,
  type UniversalSearchQuery,
} from "./universal-search.js";
import type { ConditionFilter } from "./deal-enrich.js";
import { db } from "./db.js";
import { runShoppingAssistant } from "./agents/shopping-assistant.js";
import { PARTICIPATING_RETAILERS } from "./participating-retailers.js";
import { evaluateTargetWatches, type TargetWatchInput } from "./target-watch.js";

const shareCodeAlphabet = customAlphabet("23456789ABCDEFGHJKLMNPQRSTUVWXYZ", 8);
const app = express();
const PORT = Number(process.env.PORT ?? 3847);

app.use(cors());
app.use(express.json({ limit: "2mb" }));

function freshPool() {
  return liveEnrichedDeals();
}

app.get("/api/health", (_req, res) => {
  res.json({ ok: true, product: "black-friday-hq" });
});

app.get("/api/season", (_req, res) => {
  res.json(detectSeason());
});

app.get("/api/trust/tips", (_req, res) => {
  res.json({
    tips: [
      "We only surface links to known retailer domains (Amazon, Adidas, Sephora, Ulta, etc.).",
      "Never pay with gift cards, wire, Zelle, or crypto for “deals.”",
      "Intelligence flags inflated MSRPs — trust steal score, not % alone.",
      "Stack verified coupon codes at checkout — screenshot your cart total.",
    ],
  });
});

/** Black Friday HQ dashboard */
app.get("/api/dashboard", (_req, res) => {
  const { deals, droppedIds } = freshPool();
  res.json({
    ...buildDashboard(deals, droppedIds),
    universalSearchHint: "Search by name, UPC, model, brand, or image.",
  });
});

/** AI Deal Intelligence — briefing */
app.get("/api/intelligence/briefing", (_req, res) => {
  const { deals, droppedIds } = freshPool();
  res.json(buildBriefing(deals, droppedIds.size));
});

app.get("/api/intelligence/deal/:id", (req, res) => {
  const { deals } = freshPool();
  const deal = deals.find((d) => d.id === req.params.id);
  if (!deal) {
    res.status(404).json({ error: "not_found" });
    return;
  }
  res.json(analyzeDealIntelligence(deal, deals));
});

app.post("/api/intelligence/ask", (req, res) => {
  const question = String(req.body?.question ?? "").slice(0, 500);
  const { deals } = freshPool();
  res.json(intelligenceForQuery(question, deals));
});

/** Universal product search */
app.get("/api/search/universal", (req, res) => {
  const query = parseUniversalQuery(req.query);
  const { deals } = freshPool();
  const hits = universalSearch(deals, query);
  res.json({
    hits,
    count: hits.length,
    fetchedAt: deals[0]?.fetchedAt ?? new Date().toISOString(),
  });
});

app.post("/api/search/image", (req, res) => {
  const r = Number(req.body?.r ?? 128);
  const g = Number(req.body?.g ?? 128);
  const b = Number(req.body?.b ?? 128);
  const tags = visualTagsFromRgb(r, g, b);
  const { deals } = freshPool();
  const hits = universalSearch(deals, {
    visualTags: tags,
    includeIntelligence: true,
    trustedOnly: true,
  });
  res.json({
    visualTags: tags,
    hits: hits.slice(0, 12),
    fetchedAt: deals[0]?.fetchedAt ?? new Date().toISOString(),
    note: "Visual match uses color/category hints — confirm the product photo before you buy.",
  });
});

/** Legacy search route */
app.get("/api/deals/search", (req, res) => {
  const query = { ...parseUniversalQuery(req.query), includeIntelligence: req.query.intelligence === "true" };
  const { deals } = freshPool();
  const hits = universalSearch(deals, query);
  res.json({ deals: hits, count: hits.length, fetchedAt: deals[0]?.fetchedAt });
});

app.get("/api/deals/:id", (req, res) => {
  const { deals } = freshPool();
  const deal = deals.find((d) => d.id === req.params.id);
  if (!deal) {
    res.status(404).json({ error: "not_found" });
    return;
  }
  res.json({ ...deal, intelligence: analyzeDealIntelligence(deal, deals) });
});

app.get("/api/categories", (_req, res) => {
  const cats = [...new Set(CATALOG.map((d) => d.category))].sort();
  const brands = [...new Set(CATALOG.map((d) => d.title.split(" ")[0]))].sort();
  const retailers = [...new Set(CATALOG.map((d) => d.retailer))].sort();
  res.json({ categories: cats, brands, retailers });
});

app.post("/api/agents/pricing", (req, res) => {
  const { deals } = freshPool();
  const verdict = runPricingAgent(
    String(req.body?.question ?? ""),
    deals,
    req.body?.dealIds as string[] | undefined,
  );
  res.json(verdict);
});

app.get("/api/retailers/participating", (_req, res) => {
  res.json({ retailers: PARTICIPATING_RETAILERS });
});

app.post("/api/assistant/chat", (req, res) => {
  const message = String(req.body?.message ?? "").slice(0, 800);
  const watches = (req.body?.watches ?? []) as TargetWatchInput[];
  const { deals } = freshPool();
  res.json(runShoppingAssistant(message, deals, watches));
});

app.post("/api/target-watches/evaluate", (req, res) => {
  const watches = (req.body?.watches ?? []) as TargetWatchInput[];
  const { deals } = freshPool();
  res.json({
    results: evaluateTargetWatches(watches, deals),
    fetchedAt: deals[0]?.fetchedAt ?? new Date().toISOString(),
  });
});

app.post("/api/agents/decor", (req, res) => {
  const { deals } = freshPool();
  res.json(
    runDecorAgent(
      {
        room: req.body?.room ?? "living",
        style: String(req.body?.style ?? "cozy"),
        budgetMax: req.body?.budgetMax ? Number(req.body.budgetMax) : undefined,
        colors: req.body?.colors,
      },
      deals,
    ),
  );
});

export interface SharedListPayload {
  title: string;
  items: { dealId: string; note?: string; addedAt: string }[];
}

app.post("/api/lists", (req, res) => {
  const title = String(req.body?.title ?? "Our deal list").slice(0, 120);
  const items = req.body?.items ?? [];
  const shareCode = shareCodeAlphabet();
  const now = new Date().toISOString();
  const payload: SharedListPayload = { title, items };
  db.prepare(
    `INSERT INTO shared_lists (share_code, title, payload_json, created_at, updated_at) VALUES (?, ?, ?, ?, ?)`,
  ).run(shareCode, title, JSON.stringify(payload), now, now);
  res.status(201).json({ shareCode, title });
});

app.get("/api/lists/:shareCode", (req, res) => {
  const row = db
    .prepare(`SELECT title, payload_json, updated_at FROM shared_lists WHERE share_code = ?`)
    .get(req.params.shareCode.toUpperCase()) as
    | { title: string; payload_json: string; updated_at: string }
    | undefined;
  if (!row) {
    res.status(404).json({ error: "not_found" });
    return;
  }
  const payload = JSON.parse(row.payload_json) as SharedListPayload;
  const enriched = payload.items.map((item) => ({
    ...item,
    deal: CATALOG.find((d) => d.id === item.dealId) ?? null,
  }));
  res.json({
    shareCode: req.params.shareCode.toUpperCase(),
    title: row.title,
    updatedAt: row.updated_at,
    items: enriched,
    raw: payload,
  });
});

app.put("/api/lists/:shareCode", (req, res) => {
  const code = req.params.shareCode.toUpperCase();
  const existing = db
    .prepare(`SELECT title FROM shared_lists WHERE share_code = ?`)
    .get(code) as { title: string } | undefined;
  if (!existing) {
    res.status(404).json({ error: "not_found" });
    return;
  }
  const title = String(req.body?.title ?? existing.title).slice(0, 120);
  const items = req.body?.items ?? [];
  const payload: SharedListPayload = { title, items };
  const now = new Date().toISOString();
  db.prepare(
    `UPDATE shared_lists SET title = ?, payload_json = ?, updated_at = ? WHERE share_code = ?`,
  ).run(title, JSON.stringify(payload), now, code);
  res.json({ ok: true, updatedAt: now });
});

function parseUniversalQuery(q: Record<string, unknown>): UniversalSearchQuery {
  const out: UniversalSearchQuery = {
    q: q.q as string | undefined,
    model: q.model as string | undefined,
    upc: q.upc as string | undefined,
    brand: q.brand as string | undefined,
    retailer: q.retailer as string | undefined,
    category: q.category as string | undefined,
    minDiscount: q.minDiscount ? Number(q.minDiscount) : undefined,
    freeShippingOnly: q.freeShippingOnly === "true",
    conditionFilter: (q.conditionFilter as ConditionFilter) || "include_used",
    couponsOnly: q.couponsOnly === "true",
    trustedOnly: q.trustedOnly !== "false",
    season: q.season as ShoppingSeason | undefined,
    specKey: q.specKey as string | undefined,
    specValue: q.specValue as string | undefined,
    includeIntelligence: q.intelligence === "true" || q.includeIntelligence === "true",
  };
  if (q.maxPrice !== undefined && q.maxPrice !== "") {
    out.maxPrice = Number(q.maxPrice);
  }
  if (q.minPrice !== undefined && q.minPrice !== "") {
    out.minPrice = Number(q.minPrice);
  }
  if (q.dealTypes) {
    out.dealTypes = String(q.dealTypes).split(",") as DealType[];
  }
  return out;
}

const __dirname = dirname(fileURLToPath(import.meta.url));
const distPath = join(__dirname, "..", "dist");

if (process.env.NODE_ENV === "production") {
  app.use(express.static(distPath));
  app.get("*", (_req, res) => {
    res.sendFile(join(distPath, "index.html"));
  });
}

app.listen(PORT, "127.0.0.1", () => {
  console.log(`Black Friday HQ API on http://127.0.0.1:${PORT}`);
});
