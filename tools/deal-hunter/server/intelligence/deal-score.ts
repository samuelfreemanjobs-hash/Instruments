import type { ScoredDeal } from "../rank.js";
import { assessDealUrl } from "../trust.js";
import { returnPolicyFor } from "../return-policy.js";

/** PRD Part 8 — explainable 0–100 score with confidence when data is missing */
export type DealClassification =
  | "exceptional"
  | "strong"
  | "good"
  | "average"
  | "weak";

export interface DealScoreComponent {
  id: string;
  label: string;
  maxPoints: number;
  earnedPoints: number;
  evidence: string;
  estimated: boolean;
}

export interface DealIntelligenceScore {
  score: number;
  maxPossible: number;
  confidence: number;
  classification: DealClassification;
  classificationLabel: string;
  lastVerifiedAt: string;
  components: DealScoreComponent[];
  explanation: string;
}

const WEIGHTS = {
  historical: 30,
  competitor: 20,
  discountCredibility: 15,
  retailerReliability: 15,
  shipping: 10,
  freshness: 10,
} as const;

function classify(score: number): DealClassification {
  if (score >= 90) return "exceptional";
  if (score >= 75) return "strong";
  if (score >= 60) return "good";
  if (score >= 40) return "average";
  return "weak";
}

const CLASS_LABELS: Record<DealClassification, string> = {
  exceptional: "Exceptional deal",
  strong: "Strong deal",
  good: "Good deal",
  average: "Average deal",
  weak: "Weak deal",
};

export function computeDealIntelligenceScore(
  deal: ScoredDeal,
  peers: ScoredDeal[],
  ctx: {
    avg30d: number;
    low30d: number;
    advertisedDiscountPct: number;
    realSavingsVsAvgPct: number;
    hasHistory: boolean;
  },
): DealIntelligenceScore {
  const components: DealScoreComponent[] = [];
  const pay = deal.pricing?.finalPrice ?? deal.price;
  const lastVerifiedAt = deal.fetchedAt ?? new Date().toISOString();

  let histEarned = 0;
  if (ctx.hasHistory && ctx.avg30d > 0) {
    const belowAvg = Math.max(0, (ctx.avg30d - pay) / ctx.avg30d);
    const atLow = ctx.low30d > 0 && pay <= ctx.low30d * 1.02;
    histEarned = Math.round(
      Math.min(WEIGHTS.historical, belowAvg * WEIGHTS.historical * 1.2 + (atLow ? 8 : 0)),
    );
    components.push({
      id: "historical",
      label: "Historical price comparison",
      maxPoints: WEIGHTS.historical,
      earnedPoints: histEarned,
      evidence: `${Math.round(belowAvg * 100)}% below 30d avg $${ctx.avg30d.toFixed(2)}; low $${ctx.low30d.toFixed(2)}`,
      estimated: false,
    });
  } else {
    histEarned = Math.round(WEIGHTS.historical * 0.35);
    components.push({
      id: "historical",
      label: "Historical price comparison",
      maxPoints: WEIGHTS.historical,
      earnedPoints: histEarned,
      evidence: "Limited history — score capped until more snapshots exist",
      estimated: true,
    });
  }

  const peerPrices = peers
    .filter((p) => p.category === deal.category && p.id !== deal.id)
    .map((p) => p.pricing?.finalPrice ?? p.price);
  let compEarned = Math.round(WEIGHTS.competitor * 0.4);
  if (peerPrices.length >= 2) {
    const minPeer = Math.min(...peerPrices);
    if (pay <= minPeer) compEarned = WEIGHTS.competitor;
    else {
      const gap = (pay - minPeer) / minPeer;
      compEarned = Math.max(0, Math.round(WEIGHTS.competitor * (1 - gap * 2)));
    }
    components.push({
      id: "competitor",
      label: "Competitor comparison",
      maxPoints: WEIGHTS.competitor,
      earnedPoints: compEarned,
      evidence: `You pay $${pay.toFixed(2)} vs category low $${minPeer.toFixed(2)} (${peerPrices.length} offers)`,
      estimated: false,
    });
  } else {
    components.push({
      id: "competitor",
      label: "Competitor comparison",
      maxPoints: WEIGHTS.competitor,
      earnedPoints: compEarned,
      evidence: "Insufficient peer offers in catalog — estimate only",
      estimated: true,
    });
  }

  const gap = Math.max(0, ctx.advertisedDiscountPct - Math.max(0, ctx.realSavingsVsAvgPct));
  const credEarned =
    gap > 20
      ? Math.round(WEIGHTS.discountCredibility * 0.25)
      : gap > 10
        ? Math.round(WEIGHTS.discountCredibility * 0.55)
        : Math.round(WEIGHTS.discountCredibility * 0.9);
  components.push({
    id: "discount_cred",
    label: "Advertised discount credibility",
    maxPoints: WEIGHTS.discountCredibility,
    earnedPoints: credEarned,
    evidence: `Tag ${ctx.advertisedDiscountPct}% vs verified ${ctx.realSavingsVsAvgPct}% real savings (gap ${gap})`,
    estimated: false,
  });

  const trust = deal.trust ?? assessDealUrl(deal.url);
  const retEarned = trust.safeUrl
    ? WEIGHTS.retailerReliability
    : trust.verifiedRetailer
      ? Math.round(WEIGHTS.retailerReliability * 0.5)
      : 0;
  components.push({
    id: "retailer",
    label: "Retailer reliability",
    maxPoints: WEIGHTS.retailerReliability,
    earnedPoints: retEarned,
    evidence: trust.safeUrl ? `Verified ${trust.hostname}` : "Unverified or risky link",
    estimated: false,
  });

  const shipEarned = deal.freeShipping ? WEIGHTS.shipping : Math.round(WEIGHTS.shipping * 0.45);
  components.push({
    id: "shipping",
    label: "Shipping & all-in cost",
    maxPoints: WEIGHTS.shipping,
    earnedPoints: shipEarned,
    evidence: deal.freeShipping ? "Free shipping" : "Shipping may apply — confirm at checkout",
    estimated: !deal.freeShipping,
  });

  const ageMs = Date.now() - new Date(lastVerifiedAt).getTime();
  const fresh = ageMs < 5 * 60_000;
  const freshEarned = fresh ? WEIGHTS.freshness : Math.round(WEIGHTS.freshness * 0.5);
  components.push({
    id: "freshness",
    label: "Freshness & availability",
    maxPoints: WEIGHTS.freshness,
    earnedPoints: freshEarned,
    evidence: fresh ? "Verified this refresh" : "Stale refresh — re-check price",
    estimated: !fresh,
  });

  const policy = returnPolicyFor(deal);
  const bonus = Math.min(5, Math.floor(policy.days / 30));
  const raw = components.reduce((s, c) => s + c.earnedPoints, 0) + bonus;
  const maxPossible = Object.values(WEIGHTS).reduce((a, b) => a + b, 0) + 5;
  const score = Math.min(100, raw);
  const estimatedCount = components.filter((c) => c.estimated).length;
  const confidence = Math.max(
    35,
    Math.min(98, 100 - estimatedCount * 12 - (gap > 15 ? 10 : 0) - (ctx.hasHistory ? 0 : 15)),
  );

  const classification = classify(score);
  const explanation = [
    `${CLASS_LABELS[classification]} (${score}/100).`,
    `Confidence ${confidence}% — ${estimatedCount} component(s) use estimates or incomplete data.`,
    returnPolicyFor(deal).summary,
  ].join(" ");

  return {
    score,
    maxPossible,
    confidence,
    classification,
    classificationLabel: CLASS_LABELS[classification],
    lastVerifiedAt,
    components,
    explanation,
  };
}
