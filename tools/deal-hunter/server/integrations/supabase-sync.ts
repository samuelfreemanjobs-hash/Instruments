import { createClient, type SupabaseClient } from "@supabase/supabase-js";
import { liveEnrichedDeals } from "../deal-pipeline.js";
import type { ScoredDeal } from "../rank.js";
import { analyzeDealIntelligence } from "../intelligence/engine.js";
import { computeDealIntelligenceScore } from "../intelligence/deal-score.js";

function slug(s: string): string {
  return s.toLowerCase().replace(/[^a-z0-9]+/g, "-").replace(/^-|-$/g, "");
}

export function createSupabaseAdmin(): SupabaseClient | null {
  const url = process.env.SUPABASE_URL ?? process.env.NEXT_PUBLIC_SUPABASE_URL;
  const key = process.env.SUPABASE_SERVICE_ROLE_KEY;
  if (!url || !key) return null;
  return createClient(url, key, { auth: { persistSession: false, autoRefreshToken: false } });
}

export interface SyncResult {
  ok: boolean;
  retailers: number;
  products: number;
  offers: number;
  dealRecords: number;
  skipped?: string;
}

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

async function ensureProduct(admin: SupabaseClient, deal: ScoredDeal) {
  const norm = deal.title.toLowerCase();
  const { data: existing } = await admin.from("products").select("id").eq("normalized_title", norm).maybeSingle();
  if (existing) return existing.id as string;
  const { data, error } = await admin
    .from("products")
    .insert({
      title: deal.title,
      normalized_title: norm,
      model_number: deal.identity?.model ?? null,
      upc: deal.identity?.upc ?? null,
      primary_image_url: deal.image,
      specifications: deal.identity?.specs ?? {},
    })
    .select("id")
    .single();
  if (error) throw error;
  return data.id as string;
}

export async function syncCatalogToSupabase(): Promise<SyncResult> {
  const admin = createSupabaseAdmin();
  if (!admin) {
    return { ok: false, retailers: 0, products: 0, offers: 0, dealRecords: 0, skipped: "missing_supabase_service_role" };
  }

  const { deals } = liveEnrichedDeals();
  const retailerCache = new Map<string, string>();
  let productCount = 0;
  let offerCount = 0;
  let dealCount = 0;

  for (const deal of deals) {
    let retailerId = retailerCache.get(deal.retailer);
    if (!retailerId) {
      retailerId = await ensureRetailer(admin, deal.retailer, deal.url);
      retailerCache.set(deal.retailer, retailerId);
    }

    const productId = await ensureProduct(admin, deal);
    productCount++;

    const intel = analyzeDealIntelligence(deal, deals);
    const score = computeDealIntelligenceScore(deal, deals, {
      avg30d: intel.priceAnalysis.average30d,
      low30d: intel.priceAnalysis.lowest30d,
      advertisedDiscountPct: intel.advertisedDiscountPct,
      realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
      hasHistory: true,
    });

    const pay = deal.pricing?.finalPrice ?? deal.price;
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
          condition: deal.itemCondition,
          last_verified_at: deal.fetchedAt ?? new Date().toISOString(),
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
        deal_score: score.score,
        confidence: score.confidence / 100,
        classification: score.classification,
        explanation: { text: score.explanation, components: score.components },
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
