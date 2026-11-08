import { CATALOG, discountPct } from "./catalog.js";
import { db } from "./db.js";
import { enrichDeal } from "./deal-enrich.js";
import { assessDealUrl } from "./trust.js";
import { inferSource, rankScore, scoreRelevance, type ScoredDeal } from "./rank.js";
import { computePricingBreakdown, type PricingBreakdown } from "./pricing-final.js";
import { returnPolicyFor, type ReturnPolicy } from "./return-policy.js";

export function buildLocalEnrichedDeals(): { deals: ScoredDeal[]; droppedIds: Set<string> } {
  const fetchedAt = new Date().toISOString();
  const droppedIds = new Set<string>();

  const deals = CATALOG.map(enrichDeal).map((base) => {
    const history = db
      .prepare(
        `SELECT price, recorded_at FROM price_snapshots WHERE deal_id = ? ORDER BY recorded_at DESC LIMIT 30`,
      )
      .all(base.id) as { price: number; recorded_at: string }[];

    const row = history[0];
    const basePrice = row?.price ?? base.price;
    let drift =
      Math.round(
        basePrice *
          (Math.sin(Date.now() / 3600000 + base.id.charCodeAt(0)) * 0.008 + 1) *
          100,
      ) / 100;

    if (base.id.startsWith("mpc")) {
      const wave = Math.sin(Date.now() / 3_600_000 + base.id.length);
      if (wave < -0.72) {
        drift = Math.min(drift, base.id === "mpc-bb" ? 289.0 : 299.0);
      }
    }

    const now = new Date().toISOString();
    db.prepare(
      `INSERT OR IGNORE INTO price_snapshots (deal_id, price, recorded_at) VALUES (?, ?, ?)`,
    ).run(base.id, base.price, new Date(0).toISOString());
    db.prepare(
      `INSERT OR IGNORE INTO price_snapshots (deal_id, price, recorded_at) VALUES (?, ?, ?)`,
    ).run(base.id, drift, now);

    const prior = history[1]?.price ?? drift;
    const priceDropped = prior > drift;
    if (priceDropped) droppedIds.add(base.id);

    const trust = assessDealUrl(base.url);
    const priced = { ...base, price: drift, coupon: base.coupon };
    const pricing: PricingBreakdown = computePricingBreakdown(priced);
    const returnPolicy: ReturnPolicy = returnPolicyFor({
      retailer: base.retailer,
      dealType: base.dealType,
      itemCondition: base.itemCondition,
    });
    const effectiveDiscount = discountPct({ ...base, price: pricing.finalPrice });
    const scored: ScoredDeal = {
      ...base,
      price: drift,
      pricing,
      returnPolicy,
      effectiveDiscountPct: effectiveDiscount,
      discountPct: discountPct({ ...base, price: drift }),
      source: inferSource(base.retailer, base.tags),
      fetchedAt,
      relevanceScore: 0,
      rankScore: 0,
      trust,
      savings: Math.round((base.msrp - drift) * 100) / 100,
      priceDropped,
      priorPrice: prior,
      priceHistory: [...history].reverse(),
    };
    scored.relevanceScore = scoreRelevance(scored, "");
    scored.rankScore = rankScore(scored, "");
    return scored;
  });

  return { deals, droppedIds };
}
