import { NextResponse } from "next/server";

import { searchCatalogInSupabase } from "@/lib/supabase/catalog-search";

export async function GET(req: Request) {
  const url = new URL(req.url);
  const q = url.searchParams.get("q") ?? undefined;
  const retailer = url.searchParams.get("retailer") ?? undefined;
  const category = url.searchParams.get("category") ?? undefined;
  const maxPrice = url.searchParams.get("maxPrice");
  const useSupabase = process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG === "1";

  if (!useSupabase) {
    return NextResponse.json({ error: "supabase_catalog_disabled" }, { status: 404 });
  }

  const result = await searchCatalogInSupabase({
    q,
    retailer,
    category,
    maxPrice: maxPrice ? Number(maxPrice) : undefined,
  });

  if (!result) {
    return NextResponse.json({ hits: [], count: 0, source: "supabase_empty" });
  }

  return NextResponse.json({ ...result, source: "supabase", fetchedAt: new Date().toISOString() });
}
