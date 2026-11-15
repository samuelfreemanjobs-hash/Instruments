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

export async function fetchSupabaseOfferByExternalId(
  externalId: string,
): Promise<CatalogSearchHit | null> {
  if (!isSupabaseConfigured()) return null;
  const { url, anonKey } = supabasePublicConfig();
  const supabase = createClient(url, anonKey);
  const { data, error } = await supabase
    .from("product_offers")
    .select(
      `external_product_id, current_price, product_url, condition,
       products!inner (title, primary_image_url, specifications),
       retailers!inner (name),
       deal_records (deal_score, advertised_price, status)`,
    )
    .eq("external_product_id", externalId)
    .eq("availability_status", "in_stock")
    .maybeSingle();
  if (error || !data) return null;
  return rowToHit(data);
}

function rowToHit(row: Record<string, unknown>): CatalogSearchHit | null {
  const productRaw = row.products as
    | { title: string; primary_image_url: string | null; specifications: Record<string, unknown> }
    | { title: string; primary_image_url: string | null; specifications: Record<string, unknown> }[]
    | null;
  const retailerRaw = row.retailers as { name: string } | { name: string }[] | null;
  const product = Array.isArray(productRaw) ? productRaw[0] : productRaw;
  const retailer = Array.isArray(retailerRaw) ? retailerRaw[0] : retailerRaw;
  const ext = row.external_product_id as string | null;
  if (!product || !retailer || !ext) return null;
  const records = (row.deal_records ?? []) as { deal_score: number; advertised_price: number | null; status: string }[];
  const dr = records.find((r) => r.status === "active");
  const price = Number(row.current_price);
  const msrp = dr?.advertised_price != null ? Number(dr.advertised_price) : price * 1.2;
  const category = String(product.specifications?.category ?? "general");
  return {
    id: ext,
    title: product.title,
    retailer: retailer.name,
    category,
    price,
    msrp,
    url: row.product_url as string,
    image: product.primary_image_url ?? "https://picsum.photos/seed/bfm/400/400",
    stealScore: dr?.deal_score ?? 70,
    source: "supabase",
  };
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
    const hit = rowToHit(row as Record<string, unknown>);
    if (!hit) continue;
    const hay = `${hit.title} ${hit.category} ${hit.retailer}`.toLowerCase();
    if (q && !hay.includes(q) && !q.split(/\s+/).some((t) => t.length > 2 && hay.includes(t))) continue;
    if (cat && !hit.category.toLowerCase().includes(cat) && !hay.includes(cat)) continue;
    if (params.maxPrice != null && hit.price > params.maxPrice) continue;
    hits.push(hit);
  }

  return { hits, count: hits.length };
}
