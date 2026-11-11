import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import { CATALOG } from "./catalog.js";
import { enrichDeal } from "./deal-enrich.js";
import { computePricingBreakdown } from "./pricing-final.js";

// Demo coupons carry real expiry dates; pin the clock so results don't change with the calendar.
const BEFORE_DEMO_COUPONS_EXPIRE = new Date("2026-11-01T12:00:00Z");

describe("final pricing", () => {
  beforeEach(() => {
    vi.useFakeTimers();
    vi.setSystemTime(BEFORE_DEMO_COUPONS_EXPIRE);
  });

  afterEach(() => {
    vi.useRealTimers();
  });

  it("lowers price when verified coupon applies", () => {
    const deal = enrichDeal(CATALOG.find((d) => d.id === "d9")!);
    const p = computePricingBreakdown(deal);
    expect(p.finalPrice).toBeLessThan(p.advertisedPrice);
    expect(p.coupon?.verificationStatus).toBe("verified");
  });

  it("excludes an expired coupon from the final price", () => {
    vi.setSystemTime(new Date("2026-11-11T12:00:00Z"));
    const deal = enrichDeal(CATALOG.find((d) => d.id === "d9")!);
    const p = computePricingBreakdown(deal);
    expect(p.coupon?.verificationStatus).toBe("expired");
    expect(p.finalPrice).toBe(p.advertisedPrice);
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
