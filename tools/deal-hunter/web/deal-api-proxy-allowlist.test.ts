import { describe, expect, it } from "vitest";

import { isBlockedPublicApiPath } from "./deal-api-proxy-allowlist";

describe("isBlockedPublicApiPath", () => {
  it("blocks admin trees", () => {
    expect(isBlockedPublicApiPath("/api/admin")).toBe(true);
    expect(isBlockedPublicApiPath("/api/admin/status")).toBe(true);
  });

  it("blocks steal dispatch only", () => {
    expect(isBlockedPublicApiPath("/api/notifications/dispatch-steals")).toBe(true);
    expect(isBlockedPublicApiPath("/api/notifications/steal-hits")).toBe(false);
  });

  it("does not block similar path prefixes", () => {
    expect(isBlockedPublicApiPath("/api/administrator/ping")).toBe(false);
    expect(isBlockedPublicApiPath("/api/v1/deals")).toBe(false);
  });
});
