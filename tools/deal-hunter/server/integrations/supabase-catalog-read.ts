import { createSupabaseAdmin } from "./supabase-sync.js";

export interface SupabaseOfferRow {
  external_product_id: string;
  current_price: number;
  deal_score: number | null;
  advertised_price: number | null;
}

export async function fetchSupabaseCatalogRows(): Promise<SupabaseOfferRow[] | null> {
  const admin = createSupabaseAdmin();
  if (!admin) return null;

  const { data: offers, error } = await admin
    .from("product_offers")
    .select(
      "external_product_id, current_price, deal_records(deal_score, advertised_price, verified_at, status)",
    )
    .eq("availability_status", "in_stock")
    .not("external_product_id", "is", null)
    .limit(500);

  if (error || !offers?.length) return null;

  const rows: SupabaseOfferRow[] = [];
  for (const o of offers) {
    const ext = o.external_product_id as string | null;
    if (!ext) continue;
    const records = (o.deal_records ?? []) as {
      deal_score: number;
      advertised_price: number | null;
      verified_at: string;
      status: string;
    }[];
    const active = records
      .filter((r) => r.status === "active")
      .sort((a, b) => b.verified_at.localeCompare(a.verified_at))[0];
    rows.push({
      external_product_id: ext,
      current_price: Number(o.current_price),
      deal_score: active?.deal_score ?? null,
      advertised_price: active?.advertised_price != null ? Number(active.advertised_price) : null,
    });
  }
  return rows.length ? rows : null;
}
