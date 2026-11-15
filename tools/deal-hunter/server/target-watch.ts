import { identityFor, searchableBlob } from "./catalog-meta.js";
import { isParticipating } from "./participating-retailers.js";
import type { ScoredDeal } from "./rank.js";

export interface TargetWatchInput {
  id: string;
  productQuery: string;
  targetPrice: number;
  retailers?: string[];
  model?: string;
  upc?: string;
}

export interface TargetWatchHit {
  watchId: string;
  productQuery: string;
  targetPrice: number;
  matched: boolean;
  targetReached: boolean;
  bestDeal: ScoredDeal | null;
  participatingOffers: {
    dealId: string;
    retailer: string;
    price: number;
    url: string;
    atTarget: boolean;
  }[];
  checkedAt: string;
}

function matchesQuery(deal: ScoredDeal, query: string): boolean {
  const identity = identityFor(deal.id, deal.title, deal.retailer);
  const blob = searchableBlob(deal.title, deal.retailer, deal.category, deal.tags, identity);
  const tokens = query
    .toLowerCase()
    .split(/\s+/)
    .filter((t) => t.length > 1);
  return tokens.every((t) => blob.includes(t));
}

export function evaluateTargetWatches(
  watches: TargetWatchInput[],
  pool: ScoredDeal[],
): TargetWatchHit[] {
  const checkedAt = new Date().toISOString();
  const participating = pool.filter((d) => isParticipating(d.retailer));

  return watches.map((watch) => {
    let offers = participating.filter((d) => matchesQuery(d, watch.productQuery));

    if (watch.retailers?.length) {
      const allow = new Set(watch.retailers.map((r) => r.toLowerCase()));
      offers = offers.filter((d) => allow.has(d.retailer.toLowerCase()));
    }
    if (watch.model) {
      const m = watch.model.toLowerCase();
      offers = offers.filter((d) => {
        const id = identityFor(d.id, d.title, d.retailer);
        return (id.model ?? "").toLowerCase().includes(m) || d.title.toLowerCase().includes(m);
      });
    }

    offers.sort((a, b) => (a.pricing?.finalPrice ?? a.price) - (b.pricing?.finalPrice ?? b.price));
    const best = offers[0] ?? null;
    const targetReached = offers.some(
      (o) => (o.pricing?.finalPrice ?? o.price) <= watch.targetPrice,
    );

    return {
      watchId: watch.id,
      productQuery: watch.productQuery,
      targetPrice: watch.targetPrice,
      matched: offers.length > 0,
      targetReached,
      bestDeal: best,
      participatingOffers: offers.map((o) => ({
        dealId: o.id,
        retailer: o.retailer,
        price: o.pricing?.finalPrice ?? o.price,
        url: o.url,
        atTarget: (o.pricing?.finalPrice ?? o.price) <= watch.targetPrice,
      })),
      checkedAt,
    };
  });
}
