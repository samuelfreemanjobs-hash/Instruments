import { describe, expect, it } from "vitest";
import { findStealHits } from "./steal-dispatch.js";

describe("findStealHits", () => {
  it("returns deals at or above min steal score", () => {
    const hits = findStealHits(80);
    expect(hits.length).toBeGreaterThan(0);
    for (const h of hits) {
      expect(h.stealScore).toBeGreaterThanOrEqual(80);
    }
  });
});
