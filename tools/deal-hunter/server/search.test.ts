import { describe, expect, it } from "vitest";
import { CATALOG, discountPct } from "./catalog";

describe("catalog deals", () => {
  it("has positive discounts for promotional items", () => {
    for (const deal of CATALOG) {
      expect(discountPct(deal)).toBeGreaterThan(0);
      expect(deal.price).toBeLessThan(deal.msrp);
    }
  });

  it("includes black friday and open box types", () => {
    const types = new Set(CATALOG.map((d) => d.dealType));
    expect(types.has("black_friday")).toBe(true);
    expect(types.has("open_box")).toBe(true);
    expect(types.has("clearance")).toBe(true);
    expect(types.has("cyber_monday")).toBe(true);
  });

  it("includes grocery and used inventory", () => {
    expect(CATALOG.some((d) => d.category === "Groceries")).toBe(true);
    expect(CATALOG.some((d) => d.tags.some((t) => t.includes("used")))).toBe(true);
  });
});
