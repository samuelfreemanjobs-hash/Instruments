import { describe, expect, it } from "vitest";

import { assessProductUrl } from "./trust-url";

describe("assessProductUrl", () => {
  it("allows known retailer https links", () => {
    const r = assessProductUrl("https://www.amazon.com/dp/B000");
    expect(r.safeUrl).toBe(true);
    expect(r.hostname).toBe("www.amazon.com");
  });

  it("blocks javascript URLs", () => {
    expect(assessProductUrl("javascript:alert(1)").safeUrl).toBe(false);
  });

  it("blocks unknown hosts", () => {
    expect(assessProductUrl("https://evil-deals.example/x").safeUrl).toBe(false);
  });
});
