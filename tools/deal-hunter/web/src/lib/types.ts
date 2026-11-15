/* Shared deal types for Next client (mirrors Vite src/types.ts) */

export interface DealIntelligence {
  dealId: string;
  title: string;
  verdict: string;
  verdictHeadline: string;
  advertisedDiscountPct?: number;
  realSavingsVsAvgPct?: number;
  dealIntelligenceScore?: {
    score: number;
    confidence: number;
    classificationLabel: string;
    explanation: string;
  };
}

export interface Deal {
  id: string;
  title: string;
  retailer: string;
  category: string;
  price: number;
  msrp: number;
  freeShipping: boolean;
  url: string;
  image: string;
  stealScore: number;
  discountPct: number;
  itemCondition: string;
  trust: { safeUrl: boolean; hostname: string };
  intelligence?: DealIntelligence;
  pricing?: { finalPrice: number; totalSavedVsMsrp: number };
}

export interface CommunityDeal {
  id: string;
  title: string;
  retailer: string;
  price: number;
  upvotes: number;
  note?: string;
}

export interface ShoppingRecommendation {
  dealId: string;
  title: string;
  price: number;
  reason: string;
  score: number;
  verdict: string;
}
