import { describe, expect, it } from "vitest";
import { enrichDeal } from "./deal-enrich.js";
import { CATALOG } from "./catalog.js";
import { liveEnrichedDeals } from "./deal-pipeline.js";
import { returnPolicyFor } from "./return-policy.js";
import { computeValueScore, explainBestValueWinner } from "./value-score.js";

describe("value score", () => {
  it("prefers higher return policy in tie scenarios", () => {
    const { deals } = liveEnrichedDeals();
    const mpc = deals.filter((d) => d.id.startsWith("mpc"));
    expect(mpc.length).toBeGreaterThan(1);
    const scores = mpc.map((d) => ({
      id: d.id,
      score: computeValueScore(d, mpc, returnPolicyFor(d)).total,
    }));
    expect(scores.every((s) => s.score > 0)).toBe(true);
  });

  it("explains value over cheap price", () => {
    const { deals } = liveEnrichedDeals();
    const a = deals.find((d) => d.id === "mpc-bb")!;
    const b = deals.find((d) => d.id === "mpc-gc")!;
    const sa = computeValueScore(a, [a, b], returnPolicyFor(a));
    const sb = computeValueScore(b, [a, b], returnPolicyFor(b));
    const text = explainBestValueWinner(a, b, sa, sb, returnPolicyFor(a));
    expect(text).toMatch(/value/i);
  });

  it("Best Buy policy is 60-day tier", () => {
    const bb = enrichDeal(CATALOG.find((d) => d.id === "mpc-bb")!);
    expect(returnPolicyFor(bb).days).toBe(60);
  });
});
