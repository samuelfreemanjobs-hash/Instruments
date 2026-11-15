#!/usr/bin/env tsx
/**
 * Shrinks the committed bundled catalog so publish security review can scan the tree.
 * Run before committing catalog changes: npm run slim:hosted-catalog
 */
import { readFileSync, writeFileSync } from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const MAX_HITS = 24;
const MAX_PRICE_HISTORY = 16;
const MAX_FACTORS = 6;
const MAX_COMPONENTS = 10;

const root = path.dirname(fileURLToPath(import.meta.url));
const catalogPath = path.join(root, "../web/src/data/hosted-catalog.json");

type Payload = {
  exportedAt: string;
  droppedCount: number;
  hits: { id: string; stealScore?: number }[];
  products: Record<string, Record<string, unknown>>;
  categories: string[];
  retailers: string[];
};

const raw = JSON.parse(readFileSync(catalogPath, "utf8")) as Payload;
function slimHit(hit: Record<string, unknown>) {
  const intel = hit.intelligence as Record<string, unknown> | undefined;
  const dis = intel?.dealIntelligenceScore as Record<string, unknown> | undefined;
  return {
    id: hit.id,
    title: hit.title,
    retailer: hit.retailer,
    category: hit.category,
    price: hit.price,
    msrp: hit.msrp,
    freeShipping: hit.freeShipping,
    url: hit.url,
    image: hit.image,
    stealScore: hit.stealScore,
    discountPct: hit.discountPct,
    itemCondition: hit.itemCondition,
    trust: hit.trust,
    intelligence: intel
      ? {
          dealId: String(hit.id ?? ""),
          title: String(hit.title ?? ""),
          verdict: String(intel.verdict ?? "Bundled demo deal"),
          verdictHeadline: String(intel.verdictHeadline ?? "Demo catalog"),
          advertisedDiscountPct: intel.advertisedDiscountPct,
          realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
          dealIntelligenceScore: dis
            ? {
                score: dis.score,
                confidence: dis.confidence,
                classificationLabel: dis.classificationLabel,
                explanation: dis.explanation,
              }
            : undefined,
        }
      : undefined,
  };
}

const hits = [...raw.hits]
  .sort((a, b) => ((b.stealScore as number) ?? 0) - ((a.stealScore as number) ?? 0))
  .slice(0, MAX_HITS)
  .map((h) => slimHit(h as Record<string, unknown>));
const ids = new Set(hits.map((h) => h.id));

const products: Payload["products"] = {};
for (const id of ids) {
  const p = raw.products[id];
  if (!p) continue;
  const intel = p.intelligence as Record<string, unknown> | undefined;
  const score = p.dealIntelligenceScore as Record<string, unknown> | undefined;
  const history = (p.priceHistory as { price: number; recorded_at: string }[] | undefined) ?? [];
  products[id] = {
    deal: p.deal,
    intelligence: intel
      ? {
          verdictHeadline: intel.verdictHeadline,
          advertisedDiscountPct: intel.advertisedDiscountPct,
          realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
          bestValueVerdict: intel.bestValueVerdict,
          priceAnalysis: intel.priceAnalysis,
          factors: Array.isArray(intel.factors) ? intel.factors.slice(0, MAX_FACTORS) : [],
        }
      : {},
    dealIntelligenceScore: score
      ? {
          score: score.score,
          confidence: score.confidence,
          classificationLabel: score.classificationLabel,
          explanation: score.explanation,
          components: Array.isArray(score.components) ? score.components.slice(0, MAX_COMPONENTS) : [],
        }
      : {},
    priceHistory: history.slice(0, MAX_PRICE_HISTORY),
  };
}

const out: Payload = {
  exportedAt: raw.exportedAt,
  droppedCount: raw.droppedCount,
  hits,
  products,
  categories: raw.categories,
  retailers: raw.retailers,
};

writeFileSync(catalogPath, `${JSON.stringify(out)}\n`, "utf8");
const kb = Math.round(Buffer.byteLength(JSON.stringify(out)) / 1024);
console.log(`Slim catalog: ${hits.length} hits, ${Object.keys(products).length} products (~${kb} KB)`);