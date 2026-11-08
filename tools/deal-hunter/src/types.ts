export type DealType =
  | "black_friday"
  | "cyber_monday"
  | "clearance"
  | "closeout"
  | "open_box"
  | "flash"
  | "everyday";

export type ItemCondition = "new" | "used" | "refurbished" | "open_box";

export type ConditionFilter = "new_only" | "include_used" | "used_only";

export type ShoppingSeason = "black_friday" | "cyber_monday" | "holiday" | "grocery";

export type IntelligenceVerdict = "buy_now" | "good_deal" | "wait" | "skip";

export type CouponVerificationStatus = "verified" | "expired" | "unverified";

export interface DealCoupon {
  code: string;
  description: string;
  verified: boolean;
  expiresAt?: string;
  verificationStatus?: CouponVerificationStatus;
  expiresInHours?: number;
  successNote?: string;
}

export interface PricingLine {
  label: string;
  amount: number;
  kind: "sale" | "coupon" | "student" | "bundle" | "total";
}

export interface PricingBreakdown {
  advertisedPrice: number;
  finalPrice: number;
  finalPriceWithStudent?: number;
  totalSavedVsMsrp: number;
  totalSavedVsAdvertised: number;
  lines: PricingLine[];
  linesWithStudent?: PricingLine[];
  coupon?: DealCoupon;
  studentEligible: boolean;
  studentDiscountApplied: boolean;
  bundle?: { description: string; amount: number };
}

export interface TrustReport {
  verifiedRetailer: boolean;
  safeUrl: boolean;
  hostname: string;
  shopperTips: string[];
}

export interface ProductIdentity {
  brand: string;
  model?: string;
  upc?: string;
  specs: Record<string, string>;
  visualTags: string[];
}

export interface DealIntelligence {
  dealId: string;
  title: string;
  verdict: IntelligenceVerdict;
  buyNowOrWait?: IntelligenceVerdict;
  verdictHeadline: string;
  confidence: number;
  intelligenceScore: number;
  authenticityScore?: number;
  advertisedDiscountPct?: number;
  realSavingsVsAvgPct?: number;
  discountHonestyGap?: number;
  factors: { label: string; impact: "positive" | "negative" | "neutral"; detail: string }[];
  priceAnalysis: {
    current: number;
    msrp: number;
    lowest30d: number;
    average30d: number;
    percentBelowAvg: number;
    vsHistoricalLow: "at_low" | "near_low" | "above_low";
  };
  priceDropPrediction?: {
    likelihood: string;
    expectedPrice: number;
    timeframeDays: number;
    rationale: string;
  };
  priceComparisonSummary?: string;
  couponCombinations?: string[];
  alternativeProducts?: { dealId: string; title: string; price: number; retailer: string; reason: string }[];
  bestValueVerdict?: string;
  competitorInsight: string;
  couponStrategy: string;
  riskFlags: string[];
  generatedAt: string;
}

export interface ReturnPolicy {
  days: number;
  tier: "30" | "60" | "90" | "other";
  summary: string;
}

export interface Deal {
  id: string;
  title: string;
  retailer: string;
  category: string;
  dealType: DealType;
  msrp: number;
  price: number;
  freeShipping: boolean;
  url: string;
  image: string;
  tags: string[];
  endsAt?: string;
  stealScore: number;
  discountPct: number;
  effectiveDiscountPct?: number;
  priceHistory?: { price: number; recorded_at: string }[];
  priceDropped?: boolean;
  priorPrice?: number;
  itemCondition: ItemCondition;
  isUsed: boolean;
  seasons: ShoppingSeason[];
  coupon?: DealCoupon;
  pricing?: PricingBreakdown;
  returnPolicy?: ReturnPolicy;
  trust: TrustReport;
  savings: number;
  identity?: ProductIdentity;
  relevanceScore?: number;
  rankScore?: number;
  intelligence?: DealIntelligence;
  fetchedAt?: string;
  source?: string;
}

export interface IntelligenceBriefing {
  headline: string;
  buyNow: DealIntelligence[];
  waitList: DealIntelligence[];
  marketPulse: string[];
  priceDropsToday: number;
  avgSavingsPct?: number;
  avgRealSavingsPct?: number;
  avgAdvertisedDiscountPct?: number;
  generatedAt: string;
  dataFreshThrough: string;
}

export interface DashboardPayload {
  hiddenGems: Deal[];
  priceAlerts: Deal[];
  trendingNow: Deal[];
  endingSoon: Deal[];
  fetchedAt: string;
}

export interface WatchEntry {
  dealId: string;
  targetPrice?: number;
  alertSteal: boolean;
  lastKnownPrice: number;
  title: string;
}

export interface AlertSettings {
  minStealScore: number;
  notifyPriceDrops: boolean;
  notifySteals: boolean;
}

export interface SeasonInfo {
  season: ShoppingSeason;
  label: string;
  focus: string;
  auto: boolean;
}

export const DEAL_TYPE_LABELS: Record<DealType, string> = {
  black_friday: "Black Friday",
  cyber_monday: "Cyber Monday",
  clearance: "Clearance",
  closeout: "Closeout",
  open_box: "Open box",
  flash: "Flash deal",
  everyday: "Everyday deal",
};

export const CONDITION_LABELS: Record<ItemCondition, string> = {
  new: "New",
  used: "Used",
  refurbished: "Refurb",
  open_box: "Open box",
};

export const SEASON_LABELS: Record<ShoppingSeason, string> = {
  black_friday: "Black Friday",
  cyber_monday: "Cyber Monday",
  holiday: "Holiday gifts",
  grocery: "Groceries & home",
};

export const VERDICT_LABELS: Record<IntelligenceVerdict, string> = {
  buy_now: "Buy now",
  good_deal: "Good deal",
  wait: "Wait",
  skip: "Skip",
};
