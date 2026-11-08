const base = () => process.env.DEAL_API_ORIGIN ?? "http://127.0.0.1:3847";

export async function fetchProductDetail(id: string) {
  const res = await fetch(`${base()}/api/v1/deals/${encodeURIComponent(id)}`, {
    next: { revalidate: 90 },
  });
  if (res.status === 404) return null;
  if (!res.ok) throw new Error("product_failed");
  return res.json() as Promise<ProductDetailPayload>;
}

export interface ProductDetailPayload {
  deal: {
    id: string;
    title: string;
    retailer: string;
    category: string;
    price: number;
    msrp: number;
    url: string;
    image: string;
    freeShipping: boolean;
    stealScore: number;
    itemCondition: string;
    trust: { safeUrl: boolean; hostname: string };
    pricing?: { finalPrice: number };
    returnPolicy?: { days: number; summary: string };
  };
  intelligence: {
    verdictHeadline: string;
    realSavingsVsAvgPct: number;
    advertisedDiscountPct: number;
    bestValueVerdict?: string;
    factors: { label: string; detail: string; impact: string }[];
    priceAnalysis: {
      current: number;
      average30d: number;
      lowest30d: number;
    };
  };
  dealIntelligenceScore: {
    score: number;
    confidence: number;
    classificationLabel: string;
    explanation: string;
    components: { label: string; earnedPoints: number; maxPoints: number; evidence: string }[];
  };
  priceHistory: { price: number; recorded_at: string }[];
}
