import type { DealType } from "./catalog.js";
import { discountPct } from "./catalog.js";
import {
  identityFor,
  searchableBlob,
  type ProductIdentity,
} from "./catalog-meta.js";
import { matchesCondition, matchesSeason, type ConditionFilter } from "./deal-enrich.js";
import type { ShoppingSeason } from "./season.js";
import { scoreRelevance, type ScoredDeal } from "./rank.js";
import { analyzeDealIntelligence } from "./intelligence/engine.js";

export interface UniversalSearchQuery {
  q?: string;
  model?: string;
  upc?: string;
  brand?: string;
  retailer?: string;
  category?: string;
  minPrice?: number;
  maxPrice?: number;
  minDiscount?: number;
  freeShippingOnly?: boolean;
  dealTypes?: DealType[];
  conditionFilter?: ConditionFilter;
  couponsOnly?: boolean;
  trustedOnly?: boolean;
  season?: ShoppingSeason;
  specKey?: string;
  specValue?: string;
  visualTags?: string[];
  includeIntelligence?: boolean;
}

export interface UniversalSearchHit extends ScoredDeal {
  identity: ProductIdentity;
  relevanceScore: number;
  rankScore: number;
  intelligence?: ReturnType<typeof analyzeDealIntelligence>;
}

function tokenMatch(hay: string, needle: string, mode: "all" | "any" = "all"): boolean {
  const tokens = needle
    .trim()
    .toLowerCase()
    .split(/\s+/)
    .filter(Boolean);
  if (tokens.length === 0) return true;
  if (mode === "any") return tokens.some((t) => hay.includes(t));
  return tokens.every((t) => hay.includes(t));
}

export function universalSearch(
  pool: ScoredDeal[],
  query: UniversalSearchQuery,
): UniversalSearchHit[] {
  const filtered = pool.filter((deal) => {
    const identity = identityFor(deal.id, deal.title, deal.retailer);
    const blob = searchableBlob(deal.title, deal.retailer, deal.category, deal.tags, identity);

    if (query.q && !tokenMatch(blob, query.q, "any")) return false;
    if (query.brand && !identity.brand.toLowerCase().includes(query.brand.toLowerCase()))
      return false;
    if (query.retailer && !deal.retailer.toLowerCase().includes(query.retailer.toLowerCase()))
      return false;
    if (query.category && deal.category.toLowerCase() !== query.category.toLowerCase()) return false;
    if (query.model) {
      const m = (identity.model ?? "").toLowerCase();
      if (!m.includes(query.model.toLowerCase()) && !blob.includes(query.model.toLowerCase()))
        return false;
    }
    if (query.upc) {
      const u = (identity.upc ?? "").replace(/\D/g, "");
      const q = query.upc.replace(/\D/g, "");
      if (!u.includes(q) && q.length >= 6) return false;
      if (q.length < 6 && !blob.includes(query.upc.toLowerCase())) return false;
    }
    if (query.minPrice != null && deal.price < query.minPrice) return false;
    if (query.maxPrice != null && deal.price > query.maxPrice) return false;
    if (query.minDiscount != null && discountPct(deal) < query.minDiscount) return false;
    if (query.freeShippingOnly && !deal.freeShipping) return false;
    if (query.dealTypes?.length && !query.dealTypes.includes(deal.dealType)) return false;
    if (query.conditionFilter && !matchesCondition(deal, query.conditionFilter)) return false;
    if (query.couponsOnly && !deal.coupon?.verified) return false;
    if (query.season && !matchesSeason(deal, query.season)) return false;
    if (!deal.trust?.safeUrl && query.trustedOnly !== false) return false;

    if (query.specKey && query.specValue) {
      const val = identity.specs[query.specKey.toLowerCase()];
      if (!val || !val.toLowerCase().includes(query.specValue.toLowerCase())) return false;
    }

    if (query.visualTags?.length) {
      const vis = identity.visualTags.map((v) => v.toLowerCase());
      if (!query.visualTags.some((t) => vis.includes(t.toLowerCase()))) return false;
    }

    return true;
  });

  const q = query.q ?? "";
  const hits: UniversalSearchHit[] = filtered.map((deal) => {
    const identity = identityFor(deal.id, deal.title, deal.retailer);
    const relevance = scoreRelevance(
      { ...deal, tags: [...deal.tags, identity.brand, identity.model ?? ""] },
      q,
    );
    const rank =
      relevance * 0.45 + deal.stealScore * 0.35 + deal.discountPct * 0.15 + (deal.coupon ? 5 : 0);
    const row: UniversalSearchHit = {
      ...deal,
      identity,
      relevanceScore: relevance,
      rankScore: rank,
    };
    if (query.includeIntelligence) {
      row.intelligence = analyzeDealIntelligence(deal, pool);
    }
    return row;
  });

  hits.sort((a, b) => b.rankScore - a.rankScore);
  return hits;
}

/** Map image dominant color to visual tags */
export function visualTagsFromRgb(r: number, g: number, b: number): string[] {
  const tags: string[] = [];
  if (r > 180 && g < 100 && b < 100) tags.push("red", "beauty");
  if (r > 200 && g > 180 && b > 180) tags.push("white", "sneaker");
  if (r < 80 && g < 80 && b < 80) tags.push("black", "electronics");
  if (g > r && g > b) tags.push("beauty", "skincare");
  if (r > 160 && g > 120 && b < 100) tags.push("gold", "skincare", "ulta");
  if (tags.length === 0) tags.push("electronics");
  return tags;
}
