import type { EnrichedFields } from "./deal-enrich.js";
import type { CatalogDeal } from "./catalog.js";
import { discountPct } from "./catalog.js";
import type { TrustReport } from "./trust.js";
import type { PricingBreakdown } from "./pricing-final.js";
import type { ReturnPolicy } from "./return-policy.js";

export type DealSource =
  | "amazon"
  | "walmart"
  | "target"
  | "costco"
  | "slickdeals"
  | "retailer"
  | "community";

export interface ScoredDeal extends CatalogDeal, EnrichedFields {
  discountPct: number;
  source: DealSource;
  fetchedAt: string;
  relevanceScore: number;
  rankScore: number;
  trust: TrustReport;
  savings: number;
  priceDropped?: boolean;
  priorPrice?: number;
  priceHistory?: { price: number; recorded_at: string }[];
  pricing?: PricingBreakdown;
  effectiveDiscountPct?: number;
  returnPolicy?: ReturnPolicy;
}

const RETAILER_SOURCE: Record<string, DealSource> = {
  Amazon: "amazon",
  "Amazon Warehouse": "amazon",
  Walmart: "walmart",
  Target: "target",
  Costco: "costco",
  Newegg: "community",
  eBay: "community",
};

export function inferSource(retailer: string, tags: string[]): DealSource {
  if (tags.some((t) => /slickdeals/i.test(t))) return "slickdeals";
  return RETAILER_SOURCE[retailer] ?? "retailer";
}

export function scoreRelevance(deal: CatalogDeal, query: string): number {
  const q = query.trim().toLowerCase();
  if (!q) return 50;
  const tokens = q.split(/\s+/).filter(Boolean);
  const hay = [
    deal.title,
    deal.retailer,
    deal.category,
    deal.dealType.replace("_", " "),
    ...deal.tags,
  ]
    .join(" ")
    .toLowerCase();

  let hits = 0;
  for (const t of tokens) {
    if (hay.includes(t)) hits++;
  }
  if (hay.includes(q)) hits += 2;
  return Math.min(100, Math.round((hits / Math.max(tokens.length, 1)) * 100));
}

export function rankScore(deal: ScoredDeal, query: string): number {
  const relevance = scoreRelevance(deal, query);
  const freshness = 100; // caller sets fetchedAt; all results same batch = fresh
  const value = deal.stealScore;
  const couponBoost = deal.coupon?.verified ? 8 : 0;
  return relevance * 0.45 + value * 0.35 + freshness * 0.1 + discountPct(deal) * 0.1 + couponBoost;
}
