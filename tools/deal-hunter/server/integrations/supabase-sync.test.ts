import { describe, expect, it } from "vitest";
import { syncCatalogToSupabase } from "./supabase-sync.js";

describe("supabase catalog sync", () => {
  it("skips when service role not configured", async () => {
    const prev = process.env.SUPABASE_SERVICE_ROLE_KEY;
    delete process.env.SUPABASE_SERVICE_ROLE_KEY;
    const result = await syncCatalogToSupabase();
    expect(result.ok).toBe(false);
    expect(result.skipped).toBe("missing_supabase_service_role");
    if (prev) process.env.SUPABASE_SERVICE_ROLE_KEY = prev;
  });
});
