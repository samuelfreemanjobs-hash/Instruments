import { createClient, type SupabaseClient } from "@supabase/supabase-js";

import catalog from "@/data/hosted-catalog.json";
import type { Deal } from "@/lib/types";
import { publicRuntimeConfig } from "@/lib/public-env";

function slug(name: string): string {
  return name.toLowerCase().replace(/[^a-z0-9]+/g, "-").replace(/^-|-$/g, "");
}

function createAdmin(): SupabaseClient | null {
  const url = publicRuntimeConfig().supabaseUrl || process.env.SUPABASE_URL;
  const key = process.env.SUPABASE_SERVICE_ROLE_KEY;
  if (!url || !key) return null;
  return createClient(url, key, { auth: { persistSession: false, autoRefreshToken: false } });
}

export type BundledSyncResult = {
  ok: boolean;
  retailers: number;
  products: number;
  offers: number;
  dealRecords: number;
  error?: string;
};

async function ensureRetailer(admin: SupabaseClient, name: string, sampleUrl: string) {
  const rslug = slug(name);
  const { data: existing } = await admin.from("retailers").select("id").eq("slug", rslug).maybeSingle();
  if (existing) return existing.id as string;
  const { data, error } = await admin
    .from("retailers")
    .insert({ name, slug: rslug, website_url: sampleUrl, integration_type: "demo", active: true })
    .select("id")
    .single();
  if (error) throw error;
  return data.id as string;
}

async function ensureProduct(admin: SupabaseClient, deal: Deal) {
  const norm = deal.title.toLowerCase();
  const { data: existing } = await admin.from("products").select("id").eq("normalized_title", norm).maybeSingle();
  if (existing) return existing.id as string;
  const { data, error } = await admin
    .from("products")
    .insert({
      title: deal.title,
      normalized_title: norm,
      primary_image_url: deal.image,
      specifications: { category: deal.category },
    })
    .select("id")
    .single();
  if (error) throw error;
  return data.id as string;
}

/** Push shipped bundled JSON into Supabase (no GitHub / no Express API). */
export async function syncBundledCatalogToSupabase(): Promise<BundledSyncResult> {
  const admin = createAdmin();
  if (!admin) {
    return {
      ok: false,
      retailers: 0,
      products: 0,
      offers: 0,
      dealRecords: 0,
      error: "missing_supabase_service_role",
    };
  }

  const hits = (catalog.hits as Deal[]) ?? [];
  const retailerCache = new Map<string, string>();
  let productCount = 0;
  let offerCount = 0;
  let dealCount = 0;

  for (const deal of hits) {
    let retailerId = retailerCache.get(deal.retailer);
    if (!retailerId) {
      retailerId = await ensureRetailer(admin, deal.retailer, deal.url);
      retailerCache.set(deal.retailer, retailerId);
    }

    const productId = await ensureProduct(admin, deal);
    productCount++;

    const pay = deal.pricing?.finalPrice ?? deal.price;
    const score = deal.intelligence?.dealIntelligenceScore?.score ?? deal.stealScore;

    const { data: offer, error: oErr } = await admin
      .from("product_offers")
      .upsert(
        {
          product_id: productId,
          retailer_id: retailerId,
          external_product_id: deal.id,
          product_url: deal.url,
          current_price: pay,
          availability_status: "in_stock",
          condition: deal.itemCondition ?? "new",
          last_verified_at: new Date().toISOString(),
        },
        { onConflict: "retailer_id,external_product_id" },
      )
      .select("id")
      .single();

    if (oErr || !offer) continue;
    offerCount++;

    await admin.from("deal_records").upsert(
      {
        offer_id: offer.id,
        advertised_price: deal.msrp,
        current_price: pay,
        deal_score: score,
        confidence: (deal.intelligence?.dealIntelligenceScore?.confidence ?? 75) / 100,
        classification: deal.intelligence?.dealIntelligenceScore?.classificationLabel ?? "demo",
        explanation: { text: "Synced from bundled catalog", source: "hosted-catalog.json" },
        verified_at: new Date().toISOString(),
        status: "active",
      },
      { onConflict: "offer_id" },
    );
    dealCount++;
  }

  return {
    ok: true,
    retailers: retailerCache.size,
    products: productCount,
    offers: offerCount,
    dealRecords: dealCount,
  };
}