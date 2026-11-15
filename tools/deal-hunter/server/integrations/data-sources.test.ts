import { describe, expect, it } from "vitest";
import { AUTHORIZED_SOURCES, isScrapingAssumed, policySummary } from "./data-sources.js";

describe("data source policy", () => {
  it("does not assume unrestricted scraping", () => {
    expect(isScrapingAssumed()).toBe(false);
    expect(policySummary()).toMatch(/affiliate|authorized|PA-API/i);
  });

  it("lists permitted connector kinds", () => {
    const kinds = new Set(AUTHORIZED_SOURCES.map((s) => s.kind));
    expect(kinds.has("affiliate_feed")).toBe(true);
    expect(kinds.has("demo_curated")).toBe(true);
    expect(AUTHORIZED_SOURCES.every((s) => s.permitted)).toBe(true);
  });
});
