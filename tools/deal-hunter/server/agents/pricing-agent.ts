import { analyzeDealIntelligence } from "../intelligence/engine.js";
import { returnPolicyFor } from "../return-policy.js";
import { computeValueScore, explainBestValueWinner, type ValueScoreBreakdown } from "../value-score.js";
import type { ScoredDeal } from "../rank.js";

export interface PricingAgentVerdict {
  summary: string;
  valueVerdict: string;
  bestPick: ScoredDeal | null;
  bestValueScore: number;
  valueBreakdown: ValueScoreBreakdown | null;
  alternatives: ScoredDeal[];
  couponTip: string;
  savingsBreakdown: string;
  confidence: "high" | "medium";
  analyzedAt: string;
}

export function runPricingAgent(
  question: string,
  pool: ScoredDeal[],
  dealIds?: string[],
): PricingAgentVerdict {
  const analyzedAt = new Date().toISOString();
  let candidates = pool;

  if (dealIds?.length) {
    const set = new Set(dealIds);
    candidates = pool.filter((d) => set.has(d.id));
  } else if (question.trim()) {
    const q = question.toLowerCase();
    candidates = pool
      .filter((d) => {
        const hay = `${d.title} ${d.category} ${d.tags.join(" ")}`.toLowerCase();
        return q.split(/\s+/).some((t) => t.length > 2 && hay.includes(t));
      })
      .slice(0, 8);
  }

  if (candidates.length === 0) {
    return {
      summary: "No live match in this refresh — try universal search.",
      valueVerdict: "",
      bestPick: null,
      bestValueScore: 0,
      valueBreakdown: null,
      alternatives: [],
      couponTip: "Stack verified coupons only.",
      savingsBreakdown: "—",
      confidence: "medium",
      analyzedAt,
    };
  }

  const scored = candidates.map((d) => {
    const ret = returnPolicyFor(d);
    const breakdown = computeValueScore(d, candidates, ret);
    return { deal: d, breakdown, returnPolicy: ret };
  });

  scored.sort((a, b) => b.breakdown.total - a.breakdown.total);
  const best = scored[0];
  const runner = scored[1];
  const alts = scored.slice(1, 4).map((s) => s.deal);

  const pay = best.deal.pricing?.finalPrice ?? best.deal.price;
  const intel = analyzeDealIntelligence(best.deal, pool);
  const valueVerdict = explainBestValueWinner(
    best.deal,
    runner?.deal,
    best.breakdown,
    runner?.breakdown,
    best.returnPolicy,
  );

  const summary = `Best value: ${best.deal.title} at ${best.deal.retailer} — ${valueVerdict}`;

  const coupon = best.deal.pricing?.coupon?.verificationStatus === "verified"
    ? `Use ${best.deal.pricing.coupon.code}; expires ${best.deal.pricing.coupon.expiresInHours != null ? `in ~${best.deal.pricing.coupon.expiresInHours}h` : "per retailer policy"}.`
    : "Clip store coupons at checkout; screenshot final cart.";

  return {
    summary,
    valueVerdict,
    bestPick: best.deal,
    bestValueScore: best.breakdown.total,
    valueBreakdown: best.breakdown,
    alternatives: alts,
    couponTip: coupon,
    savingsBreakdown: `MSRP $${best.deal.msrp.toFixed(2)} → you pay ~$${pay.toFixed(2)} (${intel.realSavingsVsAvgPct}% below 30d avg after discounts). Returns: ${best.returnPolicy.summary}`,
    confidence: best.breakdown.total >= 75 ? "high" : "medium",
    analyzedAt,
  };
}
