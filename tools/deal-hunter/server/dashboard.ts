import type { ScoredDeal } from "./rank.js";

export interface DashboardSections {
  hiddenGems: ScoredDeal[];
  priceAlerts: ScoredDeal[];
  trendingNow: ScoredDeal[];
  endingSoon: ScoredDeal[];
  fetchedAt: string;
}

/** Hidden gems: strong value but not the loudest doorbusters */
export function pickHiddenGems(deals: ScoredDeal[]): ScoredDeal[] {
  return [...deals]
    .filter((d) => d.stealScore >= 68 && d.stealScore < 88 && d.discountPct >= 25)
    .sort((a, b) => b.discountPct - a.discountPct)
    .slice(0, 6);
}

/** Deals with active price drop signal */
export function pickPriceAlerts(
  deals: ScoredDeal[],
  droppedIds: Set<string>,
): ScoredDeal[] {
  return deals
    .filter((d) => droppedIds.has(d.id) || d.stealScore >= 85)
    .sort((a, b) => b.stealScore - a.stealScore)
    .slice(0, 6);
}

export function pickTrending(deals: ScoredDeal[]): ScoredDeal[] {
  const trendingTags = /trending|flash|black friday|cyber/i;
  return [...deals]
    .filter(
      (d) =>
        d.stealScore >= 72 ||
        d.tags.some((t) => trendingTags.test(t)) ||
        d.dealType === "flash",
    )
    .sort((a, b) => b.stealScore - a.stealScore)
    .slice(0, 8);
}

export function pickEndingSoon(deals: ScoredDeal[], now = Date.now()): ScoredDeal[] {
  const horizon = 72 * 3600 * 1000;
  return deals
    .filter((d) => {
      if (!d.endsAt) return false;
      const end = new Date(d.endsAt).getTime();
      return end > now && end - now <= horizon;
    })
    .sort((a, b) => new Date(a.endsAt!).getTime() - new Date(b.endsAt!).getTime())
    .slice(0, 6);
}

export function buildDashboard(
  deals: ScoredDeal[],
  droppedIds: Set<string>,
): DashboardSections {
  return {
    hiddenGems: pickHiddenGems(deals),
    priceAlerts: pickPriceAlerts(deals, droppedIds),
    trendingNow: pickTrending(deals),
    endingSoon: pickEndingSoon(deals),
    fetchedAt: new Date().toISOString(),
  };
}
