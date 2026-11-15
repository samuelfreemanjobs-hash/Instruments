#!/usr/bin/env tsx
/**
 * Smoke test published app + optional Supabase row counts via service role.
 */
const base = (process.env.BFM_LIVE_URL ?? "https://black-friday-madness.apps.grok.bot").replace(/\/$/, "");

async function main() {
  const sourceRes = await fetch(`${base}/api/catalog/source`, { cache: "no-store" });
  if (!sourceRes.ok) {
    throw new Error(`catalog/source HTTP ${sourceRes.status}`);
  }
  const source = (await sourceRes.json()) as { useSupabase?: boolean; source?: string };
  if (!source.useSupabase) {
    throw new Error(
      "Live app still on bundled demo: set USE_SUPABASE_CATALOG=1 in Cursor Publish Secrets and republish",
    );
  }
  console.log("catalog/source:", JSON.stringify(source));

  const searchRes = await fetch(`${base}/api/catalog/search?q=deal`, { cache: "no-store" });
  if (searchRes.status === 404) {
    throw new Error("catalog/search returned 404 — catalog flag off on server");
  }
  const search = (await searchRes.json()) as { count?: number; source?: string };
  console.log("catalog/search:", JSON.stringify({ count: search.count, source: search.source }));

  const url = process.env.SUPABASE_URL ?? process.env.BFM_SUPABASE_URL;
  const key = process.env.SUPABASE_SERVICE_ROLE_KEY;
  if (url && key) {
    const { createClient } = await import("@supabase/supabase-js");
    const admin = createClient(url, key, { auth: { persistSession: false, autoRefreshToken: false } });
    const { count, error } = await admin.from("product_offers").select("*", { count: "exact", head: true });
    if (error) throw error;
    console.log("supabase product_offers count:", count ?? 0);
    if ((count ?? 0) < 1) {
      throw new Error("Supabase has no product_offers — run sync:supabase");
    }
  }

  console.log("OK — live catalog path looks healthy");
}

main().catch((e) => {
  console.error(e instanceof Error ? e.message : e);
  process.exit(1);
});
