import { discountPct } from "./catalog.js";
import { computePricingBreakdown } from "./pricing-final.js";
import type { ScoredDeal } from "./rank.js";
import { fetchSupabaseCatalogRows } from "./integrations/supabase-catalog-read.js";
import { buildLocalEnrichedDeals } from "./deal-pipeline-local.js";

export type CatalogSource = "local" | "supabase" | "supabase_fallback";

let catalogSource: CatalogSource = "local";
let supabasePool: { deals: ScoredDeal[]; droppedIds: Set<string> } | null = null;

export function getCatalogSource(): CatalogSource {
  return catalogSource;
}

export function liveEnrichedDeals(): { deals: ScoredDeal[]; droppedIds: Set<string> } {
  if (process.env.USE_SUPABASE_CATALOG === "1" && supabasePool && supabasePool.deals.length > 0) {
    return supabasePool;
  }
  catalogSource = "local";
  return buildLocalEnrichedDeals();
}

export async function refreshSupabaseDealCache(): Promise<{ ok: boolean; count: number; source: CatalogSource }> {
  const rows = await fetchSupabaseCatalogRows();
  if (!rows?.length) {
    catalogSource = process.env.USE_SUPABASE_CATALOG === "1" ? "supabase_fallback" : "local";
    return { ok: false, count: 0, source: catalogSource };
  }

  const local = buildLocalEnrichedDeals();
  const byId = new Map(local.deals.map((d) => [d.id, d]));
  let overlaid = 0;

  for (const row of rows) {
    const deal = byId.get(row.external_product_id);
    if (!deal) continue;
    const price = row.current_price;
    deal.price = price;
    if (row.deal_score != null) deal.stealScore = row.deal_score;
    if (row.advertised_price != null) deal.msrp = Math.max(row.advertised_price, price);
    deal.pricing = computePricingBreakdown({ ...deal, price });
    deal.discountPct = discountPct({ ...deal, price });
    deal.effectiveDiscountPct = discountPct({ ...deal, price: deal.pricing.finalPrice });
    deal.savings = Math.round((deal.msrp - price) * 100) / 100;
    overlaid++;
  }

  if (overlaid === 0) {
    catalogSource = "supabase_fallback";
    return { ok: false, count: 0, source: catalogSource };
  }

  supabasePool = local;
  catalogSource = "supabase";
  return { ok: true, count: overlaid, source: catalogSource };
}
