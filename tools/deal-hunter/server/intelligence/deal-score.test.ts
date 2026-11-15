import { describe, expect, it } from "vitest";
import { liveEnrichedDeals } from "../deal-pipeline.js";
import { computeDealIntelligenceScore } from "./deal-score.js";
import { analyzeDealIntelligence } from "./engine.js";

describe("Deal Intelligence Score (PRD Part 8)", () => {
  it("returns 0-100 with classification and confidence", () => {
    const { deals } = liveEnrichedDeals();
    const deal = deals[0];
    const intel = analyzeDealIntelligence(deal, deals);
    const score = computeDealIntelligenceScore(deal, deals, {
      avg30d: intel.priceAnalysis.average30d,
      low30d: intel.priceAnalysis.lowest30d,
      advertisedDiscountPct: intel.advertisedDiscountPct,
      realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
      hasHistory: true,
    });
    expect(score.score).toBeGreaterThanOrEqual(0);
    expect(score.score).toBeLessThanOrEqual(100);
    expect(score.confidence).toBeLessThanOrEqual(98);
    expect(["exceptional", "strong", "good", "average", "weak"]).toContain(score.classification);
    expect(score.components.length).toBeGreaterThanOrEqual(6);
  });

  it("lowers confidence when history missing", () => {
    const { deals } = liveEnrichedDeals();
    const deal = deals[0];
    const intel = analyzeDealIntelligence(deal, deals);
    const withHist = computeDealIntelligenceScore(deal, deals, {
      avg30d: intel.priceAnalysis.average30d,
      low30d: intel.priceAnalysis.lowest30d,
      advertisedDiscountPct: intel.advertisedDiscountPct,
      realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
      hasHistory: true,
    });
    const noHist = computeDealIntelligenceScore(deal, deals, {
      avg30d: intel.priceAnalysis.average30d,
      low30d: intel.priceAnalysis.lowest30d,
      advertisedDiscountPct: intel.advertisedDiscountPct,
      realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
      hasHistory: false,
    });
    expect(noHist.confidence).toBeLessThanOrEqual(withHist.confidence);
    expect(noHist.components.some((c) => c.estimated)).toBe(true);
  });
});
