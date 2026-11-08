import { describe, expect, it } from "vitest";
import { listSupabasePushTargets } from "./supabase-push.js";

describe("listSupabasePushTargets", () => {
  it("returns empty when Supabase admin is not configured", async () => {
    const prev = process.env.SUPABASE_URL;
    const prevKey = process.env.SUPABASE_SERVICE_ROLE_KEY;
    delete process.env.SUPABASE_URL;
    delete process.env.SUPABASE_SERVICE_ROLE_KEY;
    await expect(listSupabasePushTargets()).resolves.toEqual([]);
    if (prev) process.env.SUPABASE_URL = prev;
    if (prevKey) process.env.SUPABASE_SERVICE_ROLE_KEY = prevKey;
  });
});
