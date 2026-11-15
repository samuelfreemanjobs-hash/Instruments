import { writeFileSync, mkdirSync } from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

import { liveEnrichedDeals } from "../server/catalog-pool.js";
import { universalSearch } from "../server/universal-search.js";
import { analyzeDealIntelligence } from "../server/intelligence/engine.js";
import { computeDealIntelligenceScore } from "../server/intelligence/deal-score.js";
import { db } from "../server/db.js";

const root = path.dirname(fileURLToPath(import.meta.url));
const outDir = path.join(root, "../web/src/data");
const outFile = path.join(outDir, "hosted-catalog.json");

const { deals, droppedIds } = liveEnrichedDeals();
const hits = universalSearch(deals, { includeIntelligence: true, trustedOnly: true });

const products: Record<string, unknown> = {};
for (const deal of deals) {
  const intel = analyzeDealIntelligence(deal, deals);
  const rows = db
    .prepare(`SELECT price, recorded_at FROM price_snapshots WHERE deal_id = ? ORDER BY recorded_at DESC LIMIT 40`)
    .all(deal.id) as { price: number; recorded_at: string }[];
  const dealScore = computeDealIntelligenceScore(deal, deals, {
    avg30d: intel.priceAnalysis.average30d,
    low30d: intel.priceAnalysis.lowest30d,
    advertisedDiscountPct: intel.advertisedDiscountPct,
    realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
    hasHistory: rows.length >= 3,
  });
  products[deal.id] = {
    deal,
    intelligence: intel,
    dealIntelligenceScore: dealScore,
    priceHistory: rows,
  };
}

mkdirSync(outDir, { recursive: true });
writeFileSync(
  outFile,
  JSON.stringify({
    exportedAt: new Date().toISOString(),
    droppedCount: droppedIds.size,
    hits,
    products,
    categories: [...new Set(deals.map((d) => d.category))].sort(),
    retailers: [...new Set(deals.map((d) => d.retailer))].sort(),
  }),
);
console.log(`Wrote ${hits.length} deals to ${outFile}`);
