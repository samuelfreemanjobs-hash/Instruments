import { db } from "../db.js";
import { identityFor } from "../catalog-meta.js";
import { discountPct } from "../catalog.js";
import { returnPolicyFor } from "../return-policy.js";
import { computeValueScore } from "../value-score.js";
import type { ScoredDeal } from "../rank.js";

export type IntelligenceVerdict = "buy_now" | "good_deal" | "wait" | "skip";

export interface IntelligenceFactor {
  label: string;
  impact: "positive" | "negative" | "neutral";
  detail: string;
}

export interface PriceDropPrediction {
  likelihood: "high" | "medium" | "low";
  expectedPrice: number;
  timeframeDays: number;
  rationale: string;
}

export interface AlternativeProduct {
  dealId: string;
  title: string;
  price: number;
  retailer: string;
  reason: string;
}

export interface DealIntelligence {
  dealId: string;
  title: string;
  verdict: IntelligenceVerdict;
  buyNowOrWait: IntelligenceVerdict;
  verdictHeadline: string;
  confidence: number;
  intelligenceScore: number;
  authenticityScore: number;
  advertisedDiscountPct: number;
  realSavingsVsAvgPct: number;
  discountHonestyGap: number;
  factors: IntelligenceFactor[];
  priceAnalysis: {
    current: number;
    msrp: number;
    lowest30d: number;
    average30d: number;
    percentBelowAvg: number;
    vsHistoricalLow: "at_low" | "near_low" | "above_low";
  };
  priceDropPrediction: PriceDropPrediction;
  priceComparisonSummary: string;
  couponCombinations: string[];
  alternativeProducts: AlternativeProduct[];
  bestValueVerdict?: string;
  competitorInsight: string;
  couponStrategy: string;
  riskFlags: string[];
  generatedAt: string;
}

export interface CategoryTrendPoint {
  category: string;
  avgRealSavingsPct: number;
  dealCount: number;
}

export interface IntelligenceBriefing {
  headline: string;
  buyNow: DealIntelligence[];
  waitList: DealIntelligence[];
  marketPulse: string[];
  priceDropsToday: number;
  avgRealSavingsPct: number;
  avgAdvertisedDiscountPct: number;
  categoryTrends: CategoryTrendPoint[];
  generatedAt: string;
  dataFreshThrough: string;
}

function priceStats(dealId: string): { low: number; avg: number; high: number } {
  const rows = db
    .prepare(
      `SELECT price FROM price_snapshots WHERE deal_id = ? ORDER BY recorded_at DESC LIMIT 40`,
    )
    .all(dealId) as { price: number }[];
  if (rows.length === 0) return { low: 0, avg: 0, high: 0 };
  const prices = rows.map((r) => r.price);
  return {
    low: Math.min(...prices),
    high: Math.max(...prices),
    avg: prices.reduce((a, b) => a + b, 0) / prices.length,
  };
}

function sameBrandAlts(deal: ScoredDeal, pool: ScoredDeal[]): ScoredDeal[] {
  const id = identityFor(deal.id, deal.title, deal.retailer);
  return pool.filter((d) => {
    if (d.id === deal.id) return false;
    const other = identityFor(d.id, d.title, d.retailer);
    return other.brand === id.brand || d.category === deal.category;
  });
}

function computeAuthenticity(
  deal: ScoredDeal,
  advertisedPct: number,
  realVsAvg: number,
  riskFlags: string[],
): number {
  let score = 72;
  if (deal.trust?.safeUrl) score += 12;
  if (deal.coupon?.verified) score += 6;
  const gap = advertisedPct - Math.max(0, realVsAvg);
  if (gap > 25) score -= 22;
  else if (gap > 12) score -= 10;
  if (riskFlags.includes("possible_inflated_msrp")) score -= 18;
  if (riskFlags.includes("untrusted_link")) score -= 35;
  if (deal.stealScore >= 80) score += 8;
  return Math.min(100, Math.max(0, Math.round(score)));
}

function predictPriceDrop(
  deal: ScoredDeal,
  avg: number,
  low: number,
  vsLow: DealIntelligence["priceAnalysis"]["vsHistoricalLow"],
): PriceDropPrediction {
  if (vsLow === "at_low") {
    return {
      likelihood: "low",
      expectedPrice: deal.price,
      timeframeDays: 14,
      rationale: "Already at the 30-day low — further drops are uncommon unless a new sale starts.",
    };
  }
  if (deal.price > avg * 1.06) {
    const expected = Math.round(avg * 0.96 * 100) / 100;
    return {
      likelihood: "high",
      expectedPrice: expected,
      timeframeDays: 10,
      rationale: `Price is ${Math.round(((deal.price - avg) / avg) * 100)}% above the tracked average — history suggests a retracement toward ~$${expected.toFixed(2)}.`,
    };
  }
  if (deal.price > low * 1.08) {
    return {
      likelihood: "medium",
      expectedPrice: Math.round((low + avg) / 2 * 100) / 100,
      timeframeDays: 7,
      rationale: "Not at the floor yet; modest dip possible if the promo extends or rivals match.",
    };
  }
  return {
    likelihood: "low",
    expectedPrice: deal.price,
    timeframeDays: 5,
    rationale: "Near fair value vs recent history — big drops less likely short-term.",
  };
}

function buildCouponCombinations(deal: ScoredDeal): string[] {
  const combos: string[] = [];
  if (deal.coupon?.verified) {
    combos.push(`1) Sale price $${deal.price.toFixed(2)} → 2) Store code ${deal.coupon.code}`);
    combos.push(
      `Stack: ${deal.coupon.code} + store rewards/cashback (Ulta/Sephora points, Target Circle, etc.)`,
    );
  }
  if (deal.freeShipping) {
    combos.push("Free shipping already included — avoid small add-ons that push you over a threshold unnecessarily.");
  } else {
    combos.push("Pad cart to free-shipping minimum only if the extra item is on your list anyway.");
  }
  combos.push("Pay with a credit card (not debit) for chargeback protection on big purchases.");
  return combos;
}

export function analyzeDealIntelligence(deal: ScoredDeal, pool: ScoredDeal[]): DealIntelligence {
  const generatedAt = deal.fetchedAt ?? new Date().toISOString();
  const stats = priceStats(deal.id);
  const low = stats.low || deal.price;
  const avg = stats.avg || deal.price;
  const payPrice = deal.pricing?.finalPrice ?? deal.price;
  const advertisedDiscountPct = discountPct({ ...deal, price: deal.price });
  const realSavingsVsAvgPct =
    avg > 0 ? Math.round(((avg - payPrice) / avg) * 100) : 0;
  const discountHonestyGap = Math.max(0, advertisedDiscountPct - Math.max(0, realSavingsVsAvgPct));
  const pctBelowAvg = realSavingsVsAvgPct;

  let vsHistoricalLow: DealIntelligence["priceAnalysis"]["vsHistoricalLow"] = "above_low";
  if (deal.price <= low * 1.002) vsHistoricalLow = "at_low";
  else if (deal.price <= low * 1.06) vsHistoricalLow = "near_low";

  const factors: IntelligenceFactor[] = [];
  const riskFlags: string[] = [];

  factors.push({
    label: "Advertised vs real savings",
    impact: discountHonestyGap > 15 ? "negative" : "neutral",
    detail: `Tag says ${advertisedDiscountPct}% off MSRP; after coupons/bundles you pay ~$${payPrice.toFixed(2)} (${realSavingsVsAvgPct}% below 30d avg) — marketing gap ${discountHonestyGap} pts.`,
  });

  if (deal.trust?.safeUrl) {
    factors.push({
      label: "Trusted checkout",
      impact: "positive",
      detail: `Official ${deal.trust.hostname} link.`,
    });
  } else {
    factors.push({ label: "Unverified seller", impact: "negative", detail: "Confirm domain before paying." });
    riskFlags.push("untrusted_link");
  }

  if (deal.stealScore >= 85) {
    factors.push({
      label: "Strong value score",
      impact: "positive",
      detail: `Steal score ${deal.stealScore}/100.`,
    });
  }

  if (vsHistoricalLow === "at_low") {
    factors.push({
      label: "At 30-day low",
      impact: "positive",
      detail: `Matches lowest tracked $${low.toFixed(2)}.`,
    });
  } else if (deal.price > avg * 1.03) {
    factors.push({
      label: "Above recent average",
      impact: "negative",
      detail: `Avg $${avg.toFixed(2)} — wait may pay off.`,
    });
  }

  if (advertisedDiscountPct >= 55 && deal.stealScore < 60) {
    riskFlags.push("possible_inflated_msrp");
    factors.push({
      label: "Inflated MSRP signal",
      impact: "negative",
      detail: "Big % off but weak value vs street price.",
    });
  }

  const alts = sameBrandAlts(deal, pool).sort((a, b) => a.price - b.price);
  const compareSet = [deal, ...alts.slice(0, 4)];
  const alternativeProducts: AlternativeProduct[] = alts.slice(0, 3).map((a) => {
    const aPay = a.pricing?.finalPrice ?? a.price;
    const dPay = payPrice;
    const aVal = computeValueScore(a, compareSet, returnPolicyFor(a));
    const dVal = computeValueScore(deal, compareSet, returnPolicyFor(deal));
    let reason: string;
    if (aPay < dPay && aVal.total < dVal.total) {
      reason = `Cheaper by $${(dPay - aPay).toFixed(2)} but lower value score (${aVal.total} vs ${dVal.total}) — shorter returns or weaker coupon stack.`;
    } else if (aVal.total > dVal.total) {
      reason = `Higher value score ${aVal.total} — consider if return policy or all-in price beats this listing.`;
    } else if (aPay < dPay) {
      reason = `Lower all-in price $${aPay.toFixed(2)}; compare return window (${returnPolicyFor(a).days} days).`;
    } else {
      reason = "Same category — compare specs and condition.";
    }
    return {
      dealId: a.id,
      title: a.title,
      price: aPay,
      retailer: a.retailer,
      reason,
    };
  });

  const bestAmong = [...compareSet].map((d) => ({
    d,
    v: computeValueScore(d, compareSet, returnPolicyFor(d)),
  }));
  bestAmong.sort((a, b) => b.v.total - a.v.total);
  const bestValueVerdict =
    bestAmong[0].d.id === deal.id
      ? `This listing is the best overall value in the set (score ${bestAmong[0].v.total}/100) — not merely the lowest tag price.`
      : `${bestAmong[0].d.retailer} beats this on value (${bestAmong[0].v.total} vs ${computeValueScore(deal, compareSet, returnPolicyFor(deal)).total}) even if price differs.`;

  const priceRows = [deal, ...alts.slice(0, 2)]
    .sort((a, b) => a.price - b.price)
    .map((d) => `${d.retailer}: $${d.price.toFixed(2)} (${discountPct(d)}% tag / ${d.stealScore} steal)`);
  const priceComparisonSummary = priceRows.join(" · ");

  let verdict: IntelligenceVerdict = "good_deal";
  let verdictHeadline = "Good deal if you need it — real savings vs average are modest.";
  if (!deal.trust?.safeUrl || riskFlags.includes("possible_inflated_msrp")) {
    verdict = "skip";
    verdictHeadline = "Skip until authenticity improves — don’t trust the headline % alone.";
  } else if (vsHistoricalLow === "at_low" || (realSavingsVsAvgPct >= 12 && deal.stealScore >= 82)) {
    verdict = "buy_now";
    verdictHeadline = "Buy now — real savings vs average beat the marketing gap.";
  } else if (deal.price > avg * 1.04 || realSavingsVsAvgPct < 3) {
    verdict = "wait";
    verdictHeadline = "Wait — you’re not beating the 30-day average by much.";
  }

  const authenticityScore = computeAuthenticity(
    deal,
    advertisedDiscountPct,
    realSavingsVsAvgPct,
    riskFlags,
  );

  const positive = factors.filter((f) => f.impact === "positive").length;
  const negative = factors.filter((f) => f.impact === "negative").length;
  const confidence = Math.min(
    98,
    Math.max(42, 50 + positive * 8 - negative * 10 + authenticityScore * 0.15),
  );

  const intelligenceScore = Math.round(
    authenticityScore * 0.3 +
      deal.stealScore * 0.25 +
      Math.min(100, realSavingsVsAvgPct * 4) * 0.25 +
      confidence * 0.2,
  );

  const priceDropPrediction = predictPriceDrop(deal, avg, low, vsHistoricalLow);
  const couponCombinations = buildCouponCombinations(deal);
  const couponStrategy = couponCombinations[0] ?? "Clip store offers at checkout.";

  const competitorInsight =
    alts[0] && alts[0].price < deal.price
      ? `Best alt: ${alts[0].title} at $${alts[0].price.toFixed(2)} (${alts[0].retailer}).`
      : "This listing leads similar items in today’s feed.";

  return {
    dealId: deal.id,
    title: deal.title,
    verdict,
    buyNowOrWait: verdict,
    verdictHeadline,
    confidence,
    intelligenceScore,
    authenticityScore,
    advertisedDiscountPct,
    realSavingsVsAvgPct,
    discountHonestyGap,
    factors,
    priceAnalysis: {
      current: deal.price,
      msrp: deal.msrp,
      lowest30d: low,
      average30d: Math.round(avg * 100) / 100,
      percentBelowAvg: pctBelowAvg,
      vsHistoricalLow,
    },
    priceDropPrediction,
    priceComparisonSummary,
    couponCombinations,
    alternativeProducts,
    bestValueVerdict,
    competitorInsight,
    couponStrategy,
    riskFlags,
    generatedAt,
  };
}

export function buildBriefing(pool: ScoredDeal[], droppedCount: number): IntelligenceBriefing {
  const analyzed = pool.map((d) => analyzeDealIntelligence(d, pool));
  const buyNow = analyzed
    .filter((a) => a.buyNowOrWait === "buy_now")
    .sort((a, b) => b.authenticityScore - a.authenticityScore)
    .slice(0, 5);
  const waitList = analyzed
    .filter((a) => a.buyNowOrWait === "wait")
    .slice(0, 4);

  const avgReal =
    analyzed.length > 0
      ? Math.round(analyzed.reduce((s, a) => s + a.realSavingsVsAvgPct, 0) / analyzed.length)
      : 0;
  const avgAd =
    analyzed.length > 0
      ? Math.round(analyzed.reduce((s, a) => s + a.advertisedDiscountPct, 0) / analyzed.length)
      : 0;

  const pulse = [
    `Real avg savings vs 30d price: ${avgReal}% (advertised tags average ${avgAd}% — not the same thing).`,
    `${buyNow.length} authentic “buy now” calls this refresh.`,
    `${droppedCount} live price drops detected.`,
    "Use coupon combinations only when each step is verified at checkout.",
  ];

  const byCat = new Map<string, { sum: number; n: number }>();
  for (const a of analyzed) {
    const deal = pool.find((d) => d.id === a.dealId);
    const cat = deal?.category ?? "general";
    const row = byCat.get(cat) ?? { sum: 0, n: 0 };
    row.sum += a.realSavingsVsAvgPct;
    row.n += 1;
    byCat.set(cat, row);
  }
  const categoryTrends: CategoryTrendPoint[] = [...byCat.entries()]
    .map(([category, { sum, n }]) => ({
      category,
      avgRealSavingsPct: Math.round(sum / n),
      dealCount: n,
    }))
    .sort((a, b) => b.avgRealSavingsPct - a.avgRealSavingsPct)
    .slice(0, 8);

  return {
    headline: "AI deal intelligence — authenticity, predictions, and buy/wait on live prices.",
    buyNow,
    waitList,
    marketPulse: pulse,
    priceDropsToday: droppedCount,
    avgRealSavingsPct: avgReal,
    avgAdvertisedDiscountPct: avgAd,
    categoryTrends,
    generatedAt: new Date().toISOString(),
    dataFreshThrough: pool[0]?.fetchedAt ?? new Date().toISOString(),
  };
}

export function intelligenceForQuery(question: string, pool: ScoredDeal[]) {
  const q = question.toLowerCase();
  const matched = pool
    .filter((d) => {
      const blob = `${d.title} ${d.category} ${d.retailer} ${d.tags.join(" ")}`.toLowerCase();
      return q.split(/\s+/).some((t) => t.length > 2 && blob.includes(t));
    })
    .slice(0, 6);

  const picks = matched.map((d) => analyzeDealIntelligence(d, pool));
  const best = picks.sort((a, b) => b.intelligenceScore - a.intelligenceScore)[0];

  if (!best) {
    return {
      answer: "No live match — try universal search with product name, UPC, or brand.",
      picks: [] as DealIntelligence[],
    };
  }

  const answer = `${best.verdictHeadline} Authenticity ${best.authenticityScore}/100. Real savings vs avg: ${best.realSavingsVsAvgPct}% (tag says ${best.advertisedDiscountPct}% off MSRP). ${best.priceDropPrediction.rationale}`;
  return { answer, picks: picks.slice(0, 3) };
}
