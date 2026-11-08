import { fetchSupabaseOfferByExternalId } from "@/lib/supabase/catalog-search";
import { fetchProductDetail, type ProductDetailPayload } from "@/lib/server-deal-api";

const useSupabaseCatalog = () => process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG === "1";

export async function fetchProductDetailResolved(id: string): Promise<ProductDetailPayload | null> {
  let payload: ProductDetailPayload | null = null;
  try {
    payload = await fetchProductDetail(id);
  } catch {
    payload = null;
  }

  if (!useSupabaseCatalog()) return payload;

  const pg = await fetchSupabaseOfferByExternalId(id).catch(() => null);
  if (!pg) return payload;

  if (!payload) {
    return minimalPayloadFromCatalogHit(pg);
  }

  payload.deal.price = pg.price;
  payload.deal.msrp = Math.max(pg.msrp, pg.price);
  payload.deal.stealScore = pg.stealScore;
  payload.intelligence.priceAnalysis.current = pg.price;
  return payload;
}

function minimalPayloadFromCatalogHit(hit: {
  id: string;
  title: string;
  retailer: string;
  category: string;
  price: number;
  msrp: number;
  url: string;
  image: string;
  stealScore: number;
}): ProductDetailPayload {
  const ad = Math.round(((hit.msrp - hit.price) / Math.max(hit.msrp, 1)) * 100);
  return {
    deal: {
      id: hit.id,
      title: hit.title,
      retailer: hit.retailer,
      category: hit.category,
      price: hit.price,
      msrp: hit.msrp,
      url: hit.url,
      image: hit.image,
      freeShipping: true,
      stealScore: hit.stealScore,
      itemCondition: "new",
      trust: { safeUrl: true, hostname: safeHost(hit.url) },
      pricing: { finalPrice: hit.price },
    },
    intelligence: {
      verdictHeadline: "Synced from Supabase catalog",
      realSavingsVsAvgPct: Math.max(0, ad - 5),
      advertisedDiscountPct: ad,
      priceAnalysis: { current: hit.price, average30d: hit.msrp * 0.95, lowest30d: hit.price },
      factors: [],
    },
    dealIntelligenceScore: {
      score: hit.stealScore,
      confidence: 75,
      classificationLabel: hit.stealScore >= 85 ? "Steal" : "Good deal",
      explanation: "Score from synced deal_records row.",
      components: [],
    },
    priceHistory: [],
  };
}

function safeHost(url: string) {
  try {
    return new URL(url).hostname;
  } catch {
    return "";
  }
}
