import { Router } from "express";
import { liveEnrichedDeals } from "../../deal-pipeline.js";
import { universalSearch } from "../../universal-search.js";
import { analyzeDealIntelligence } from "../../intelligence/engine.js";
import { computeDealIntelligenceScore } from "../../intelligence/deal-score.js";
import { buildDashboard } from "../../dashboard.js";
import { listCommunityDeals } from "../../community.js";
import { runShoppingOrchestrator } from "../../agents/orchestrator.js";
import { demoAdapterRegistry } from "../../retailers/demo-adapter.js";
import { db } from "../../db.js";

export const v1 = Router();

v1.get("/health", (_req, res) => {
  res.json({ ok: true, version: "v1", product: "black-friday-madness" });
});

v1.get("/deals", (req, res) => {
  const { deals, droppedIds } = liveEnrichedDeals();
  const minScore = req.query.minDealScore ? Number(req.query.minDealScore) : 0;
  const hits = universalSearch(deals, { includeIntelligence: true, trustedOnly: true });
  const filtered =
    minScore > 0
      ? hits.filter((d) => (d.intelligence?.intelligenceScore ?? d.stealScore) >= minScore)
      : hits;
  res.json({ data: filtered, count: filtered.length, priceDrops: droppedIds.size });
});

v1.get("/deals/trending", (_req, res) => {
  const { deals, droppedIds } = liveEnrichedDeals();
  res.json(buildDashboard(deals, droppedIds));
});

v1.get("/deals/:id", (req, res) => {
  const { deals } = liveEnrichedDeals();
  const deal = deals.find((d) => d.id === req.params.id);
  if (!deal) {
    res.status(404).json({ error: "not_found" });
    return;
  }
  const intel = analyzeDealIntelligence(deal, deals);
  const rows = db.prepare(`SELECT price, recorded_at FROM price_snapshots WHERE deal_id = ? ORDER BY recorded_at DESC LIMIT 40`).all(deal.id) as { price: number; recorded_at: string }[];
  const dealScore = computeDealIntelligenceScore(deal, deals, {
    avg30d: intel.priceAnalysis.average30d,
    low30d: intel.priceAnalysis.lowest30d,
    advertisedDiscountPct: intel.advertisedDiscountPct,
    realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
    hasHistory: rows.length >= 3,
  });
  res.json({ deal, intelligence: intel, dealIntelligenceScore: dealScore, priceHistory: rows });
});

v1.get("/search", (req, res) => {
  const q = String(req.query.q ?? "");
  const { deals } = liveEnrichedDeals();
  const hits = universalSearch(deals, { q, includeIntelligence: true, trustedOnly: true });
  res.json({ hits, count: hits.length });
});

v1.post("/ai/chat", (req, res) => {
  const message = String(req.body?.message ?? "").slice(0, 800);
  const { deals } = liveEnrichedDeals();
  res.json(runShoppingOrchestrator({ message, profile: req.body?.profile, dealIds: req.body?.dealIds }, deals));
});

v1.get("/retailers/integrations", async (_req, res) => {
  const adapters = demoAdapterRegistry();
  const status = await Promise.all(adapters.map((a) => a.getStatus()));
  res.json({ integrations: status });
});

v1.get("/community/deals", (_req, res) => {
  res.json({ data: listCommunityDeals() });
});
