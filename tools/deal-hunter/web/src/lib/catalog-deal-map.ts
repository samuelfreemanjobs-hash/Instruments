import type { Deal } from "./types";
import type { CatalogSearchHit } from "./supabase/catalog-search";

export function catalogHitsToDeals(hits: CatalogSearchHit[]): Deal[] {
  return hits.map((r) => ({
    id: r.id,
    title: r.title,
    retailer: r.retailer,
    category: r.category,
    price: r.price,
    msrp: r.msrp,
    freeShipping: true,
    url: r.url,
    image: r.image,
    stealScore: r.stealScore,
    discountPct: Math.round(((r.msrp - r.price) / Math.max(r.msrp, 1)) * 100),
    itemCondition: "new",
    trust: {
      safeUrl: true,
      hostname: (() => {
        try {
          return new URL(r.url).hostname;
        } catch {
          return "";
        }
      })(),
    },
  }));
}
