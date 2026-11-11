import { describe, expect, it } from "vitest";
import { CATALOG } from "./catalog.js";
import { enrichDeal } from "./deal-enrich.js";
import { computePricingBreakdown } from "./pricing-final.js";

describe("final pricing", () => {
  it("lowers price when verified coupon applies", () => {
    const deal = enrichDeal(CATALOG.find((d) => d.id === "d9")!);
    const p = computePricingBreakdown(deal);
    expect(p.finalPrice).toBeLessThan(p.advertisedPrice);
    expect(p.coupon?.verificationStatus).toBe("verified");
  });

  it("tracks coupon expiration state", () => {
    const deal = enrichDeal(CATALOG.find((d) => d.id === "mpc-bb")!);
    const p = computePricingBreakdown(deal);
    expect(p.coupon?.expiresInHours).toBeDefined();
  });

  it("applies student stack when requested", () => {
    const deal = enrichDeal(CATALOG.find((d) => d.id === "ad1")!);
    const base = computePricingBreakdown(deal);
    const student = computePricingBreakdown(deal, { applyStudent: true });
    expect(student.studentDiscountApplied).toBe(true);
    expect(student.finalPrice).toBeLessThanOrEqual(base.finalPrice);
  });
});
