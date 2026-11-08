import { createClient } from "@supabase/supabase-js";

import { isSupabaseConfigured, supabasePublicConfig } from "./config";

export interface CatalogSearchHit {
  id: string;
  title: string;
  retailer: string;
  category: string;
  price: number;
  msrp: number;
  url: string;
  image: string;
  stealScore: number;
  source: "supabase";
}

export async function searchCatalogInSupabase(params: {
  q?: string;
  retailer?: string;
  category?: string;
  maxPrice?: number;
  limit?: number;
}): Promise<{ hits: CatalogSearchHit[]; count: number } | null> {
  if (!isSupabaseConfigured()) return null;
  const { url, anonKey } = supabasePublicConfig();
  const supabase = createClient(url, anonKey);

  let query = supabase
    .from("product_offers")
    .select(
      `external_product_id, current_price, product_url, condition,
       products!inner (title, primary_image_url, specifications),
       retailers!inner (name),
       deal_records (deal_score, advertised_price, status)`,
    )
    .eq("availability_status", "in_stock")
    .limit(params.limit ?? 48);

  if (params.retailer) {
    query = query.ilike("retailers.name", `%${params.retailer.slice(0, 60)}%`);
  }

  const { data, error } = await query;
  if (error || !data?.length) return null;

  const q = params.q?.toLowerCase().trim() ?? "";
  const cat = params.category?.toLowerCase().trim() ?? "";

  const hits: CatalogSearchHit[] = [];
  for (const row of data) {
    const productRaw = row.products as
      | { title: string; primary_image_url: string | null; specifications: Record<string, unknown> }
      | { title: string; primary_image_url: string | null; specifications: Record<string, unknown> }[]
      | null;
    const retailerRaw = row.retailers as { name: string } | { name: string }[] | null;
    const product = Array.isArray(productRaw) ? productRaw[0] : productRaw;
    const retailer = Array.isArray(retailerRaw) ? retailerRaw[0] : retailerRaw;
    if (!product || !retailer) continue;
    const records = (row.deal_records ?? []) as { deal_score: number; advertised_price: number | null; status: string }[];
    const dr = records.find((r) => r.status === "active");
    const title = product.title;
    const category = String(product.specifications?.category ?? "general");
    const hay = `${title} ${category} ${retailer.name}`.toLowerCase();
    if (q && !hay.includes(q) && !q.split(/\s+/).some((t) => t.length > 2 && hay.includes(t))) continue;
    if (cat && !category.toLowerCase().includes(cat) && !hay.includes(cat)) continue;
    const price = Number(row.current_price);
    if (params.maxPrice != null && price > params.maxPrice) continue;
    const ext = row.external_product_id as string | null;
    if (!ext) continue;
    const msrp = dr?.advertised_price != null ? Number(dr.advertised_price) : price * 1.2;
    hits.push({
      id: ext,
      title,
      retailer: retailer.name,
      category,
      price,
      msrp,
      url: row.product_url as string,
      image: product.primary_image_url ?? "https://picsum.photos/seed/bfm/400/400",
      stealScore: dr?.deal_score ?? 70,
      source: "supabase",
    });
  }

  return { hits, count: hits.length };
}
