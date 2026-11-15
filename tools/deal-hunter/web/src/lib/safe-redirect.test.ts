import { describe, expect, it } from "vitest";

import { safeRedirectPath } from "./safe-redirect";

describe("safeRedirectPath", () => {
  it("allows normal paths", () => {
    expect(safeRedirectPath("/wishlist")).toBe("/wishlist");
  });

  it("rejects open redirects", () => {
    expect(safeRedirectPath("//evil.example")).toBe("/dashboard");
    expect(safeRedirectPath("https://evil.example")).toBe("/dashboard");
    expect(safeRedirectPath("/login?next=//evil")).toBe("/dashboard");
    expect(safeRedirectPath("/@evil.example")).toBe("/dashboard");
  });

  it("collapses dot segments", () => {
    expect(safeRedirectPath("/foo/../dashboard")).toBe("/dashboard");
    expect(safeRedirectPath("/../../etc")).toBe("/dashboard");
  });
});
