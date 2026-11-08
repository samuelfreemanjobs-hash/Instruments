import { analyzeDealIntelligence } from "./intelligence/engine.js";
import type { ScoredDeal } from "./rank.js";

export interface ShopperProfile {
  categories?: string[];
  maxBudget?: number;
  brands?: string[];
  preferNewOnly?: boolean;
  zip?: string;
}

export interface ShoppingRecommendation {
  dealId: string;
  title: string;
  retailer: string;
  price: number;
  reason: string;
  score: number;
  verdict: string;
}

export function buildRecommendations(
  pool: ScoredDeal[],
  profile: ShopperProfile,
): { recommendations: ShoppingRecommendation[]; profileUsed: ShopperProfile } {
  const cats = new Set((profile.categories ?? ["electronics", "beauty", "apparel"]).map((c) => c.toLowerCase()));
  const brands = new Set((profile.brands ?? []).map((b) => b.toLowerCase()));
  const max = profile.maxBudget ?? 500;

  const scored: ShoppingRecommendation[] = [];
  for (const d of pool) {
    if (d.price > max) continue;
    if (profile.preferNewOnly && d.itemCondition !== "new") continue;
    const catOk =
      cats.size === 0 || cats.has(d.category.toLowerCase()) || d.tags.some((t) => cats.has(t.toLowerCase()));
    const brandOk =
      brands.size === 0 ||
      brands.has(d.title.split(" ")[0].toLowerCase()) ||
      brands.has(d.title.split(" ")[0].toLowerCase());
    if (!catOk && !brandOk) continue;

    const intel = analyzeDealIntelligence(d, pool);
    if (intel.verdict === "skip") continue;

    let score = d.stealScore + intel.intelligenceScore * 0.4;
    if (intel.buyNowOrWait === "buy_now") score += 15;
    if (d.freeShipping) score += 5;

    scored.push({
      dealId: d.id,
      title: d.title,
      retailer: d.retailer,
      price: d.pricing?.finalPrice ?? d.price,
      reason:
        intel.bestValueVerdict ??
        `${intel.verdictHeadline} · steal ${d.stealScore} · real savings vs 30d avg ${intel.realSavingsVsAvgPct}%`,
      score: Math.round(score),
      verdict: intel.buyNowOrWait,
    });
  }

  scored.sort((a, b) => b.score - a.score);

  return {
    recommendations: scored.slice(0, 12),
    profileUsed: profile,
  };
}
