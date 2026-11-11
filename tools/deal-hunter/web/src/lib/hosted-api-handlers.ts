import type { Deal } from "@/lib/types";
import { hostedCatalog, hostedDealsList, hostedProductDetail, hostedUniversalSearch } from "@/lib/hosted-catalog";
import { publicRuntimeConfig } from "@/lib/public-env";

export function hostedCatalogSource() {
  const c = publicRuntimeConfig();
  return {
    source: c.useSupabaseCatalog ? "supabase_or_demo" : "bundled_demo",
    useSupabase: c.useSupabaseCatalog,
  };
}

export function hostedCategories() {
  const { categories, retailers } = hostedCatalog();
  return { categories, retailers };
}

export function hostedStealHits(min = 85) {
  const hits = hostedDealsList().filter((d) => d.stealScore >= min && d.trust?.safeUrl);
  return { hits, count: hits.length };
}

export function hostedPricing(question: string) {
  const pool = hostedDealsList() as Deal[];
  const q = question.toLowerCase();
  const candidates = pool
    .filter((d) => {
      const hay = `${d.title} ${d.category}`.toLowerCase();
      return q.split(/\s+/).some((t) => t.length > 2 && hay.includes(t));
    })
    .sort((a, b) => b.stealScore - a.stealScore);
  const best = candidates[0] ?? pool.sort((a, b) => b.stealScore - a.stealScore)[0];
  return {
    summary: best ? `Best match: ${best.title} at ${best.retailer}.` : "Try another keyword.",
    valueVerdict: best ? `Steal score ${best.stealScore} on bundled demo catalog.` : "",
    bestPick: best ?? null,
    bestValueScore: best?.stealScore ?? 0,
    alternatives: candidates.slice(1, 4),
    couponTip: "Clip verified store coupons at checkout.",
    savingsBreakdown: best ? `Listed $${best.price.toFixed(2)} vs MSRP $${best.msrp.toFixed(2)}` : "—",
    confidence: "medium",
    analyzedAt: new Date().toISOString(),
  };
}

export function hostedChatAnswer(message: string) {
  const { hits } = hostedUniversalSearch(new URLSearchParams({ q: message }));
  return {
    answer: hits.length
      ? `Found ${hits.length} matches in the live demo catalog. Open Intel on any card for score breakdown.`
      : "No match — try search with a brand or category.",
    evidence: [{ summary: "Bundled demo catalog (no external API required)." }],
    agentsInvoked: ["pricing", "discovery"],
    generatedAt: new Date().toISOString(),
    message: "Bundled assistant mode",
    deals: hits.slice(0, 6),
    intelligenceSummary: "Demo catalog refresh",
  };
}

export function hostedIntelligenceBriefing() {
  const deals = hostedDealsList().sort((a, b) => b.stealScore - a.stealScore);
  return {
    headline: "Bundled demo — peak steals in this refresh",
    buyNow: deals.slice(0, 3).map((d) => ({
      title: d.title,
      verdictHeadline: `Steal ${d.stealScore}`,
      dealId: d.id,
    })),
    waitList: deals.slice(-2).map((d) => ({ title: d.title, dealId: d.id })),
    marketPulse: ["Demo catalog · add DEAL_API_ORIGIN for live API"],
    priceDropsToday: hostedCatalog().droppedCount,
  };
}
