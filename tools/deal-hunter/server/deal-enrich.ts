import type { CatalogDeal, DealType } from "./catalog.js";

export type ItemCondition = "new" | "used" | "refurbished" | "open_box";
export type ShoppingSeason = "everyday" | "black_friday" | "cyber_monday" | "holiday" | "grocery";

export interface DealCoupon {
  code: string;
  description: string;
  /** Curated / retailer-published codes in demo; verify at checkout */
  verified: boolean;
  expiresAt?: string;
}

export interface EnrichedFields {
  itemCondition: ItemCondition;
  seasons: ShoppingSeason[];
  coupon?: DealCoupon;
  isUsed: boolean;
}

const CONDITION_BY_ID: Partial<Record<string, ItemCondition>> = {
  d1: "open_box",
  d5: "open_box",
  d10: "refurbished",
  d14: "open_box",
  d18: "open_box",
};

const USED_IDS = new Set(["u1", "u2", "d5"]);

const COUPONS: Partial<Record<string, DealCoupon>> = {
  d3: {
    code: "CYBERCLEAN25",
    description: "Extra 8% off Dyson direct",
    verified: true,
    expiresAt: "2026-11-30T23:59:59Z",
  },
  sp1: { code: "SEPHORA20", description: "20% off Sephora coupon event", verified: true, expiresAt: "2026-11-20T23:59:59Z" },
  ul2: { code: "ULTA21", description: "21 Days of Beauty", verified: true, expiresAt: "2026-11-18T23:59:59Z" },
  ad1: { code: "ADIDAS15", description: "15% off full price", verified: true, expiresAt: "2026-11-25T23:59:59Z" },
  "mpc-bb": { code: "BBDEAL50", description: "$50 off music gear $300+", verified: true, expiresAt: "2026-11-15T23:59:59Z" },
  d6: { code: "SAVE10", description: "Walmart online pickup", verified: true },
  d9: { code: "FLASHSSD", description: "Newegg cart coupon", verified: true, expiresAt: "2026-11-10T06:00:00Z" },
  d13: { code: "HOME15", description: "Kohl's stackable promo", verified: true },
  g1: { code: "PANTRY20", description: "Target Circle grocery", verified: true },
  g2: { code: "CLEAN5", description: "$5 off $30 household", verified: true },
  g3: { code: "SHIPFREE", description: "Free delivery $35+", verified: true },
  u1: { code: "EBAY15", description: "Approved refurb coupon", verified: true },
};

function defaultCondition(deal: CatalogDeal): ItemCondition {
  if (CONDITION_BY_ID[deal.id]) return CONDITION_BY_ID[deal.id]!;
  if (deal.dealType === "open_box") return "open_box";
  if (deal.tags.some((t) => /refurb/i.test(t))) return "refurbished";
  if (deal.tags.some((t) => /used|pre-owned/i.test(t))) return "used";
  return "new";
}

function seasonsFor(deal: CatalogDeal): ShoppingSeason[] {
  const s = new Set<ShoppingSeason>();
  if (deal.dealType === "black_friday") s.add("black_friday");
  if ((deal.dealType as string) === "cyber_monday") s.add("cyber_monday");
  if (deal.category === "Groceries" || deal.category === "Household") {
    s.add("grocery");
  } else if (deal.dealType !== "everyday") {
    s.add("holiday");
  }
  if (deal.dealType === "flash" || deal.category === "PC") s.add("cyber_monday");
  s.add("everyday");
  if (s.size === 1) s.add("holiday");
  return [...s];
}

export function enrichDeal(deal: CatalogDeal): CatalogDeal & EnrichedFields {
  const itemCondition = defaultCondition(deal);
  const isUsed = USED_IDS.has(deal.id) || itemCondition === "used";
  return {
    ...deal,
    itemCondition,
    isUsed,
    seasons: seasonsFor(deal),
    coupon: COUPONS[deal.id],
  };
}

export type ConditionFilter = "new_only" | "include_used" | "used_only";

export function matchesCondition(
  deal: CatalogDeal & EnrichedFields,
  filter: ConditionFilter,
): boolean {
  if (filter === "include_used") return true;
  if (filter === "used_only") {
    return deal.isUsed || deal.itemCondition === "used" || deal.itemCondition === "refurbished";
  }
  return deal.itemCondition === "new";
}

export function matchesSeason(
  deal: CatalogDeal & EnrichedFields,
  season: ShoppingSeason,
): boolean {
  if (season === "everyday") return true;
  if (deal.seasons.includes(season)) return true;
  if (season === "black_friday") {
    return ["black_friday", "clearance", "closeout", "open_box", "flash"].includes(deal.dealType);
  }
  if (season === "cyber_monday") {
    return (
      deal.dealType === "cyber_monday" ||
      deal.dealType === "flash" ||
      deal.category === "PC" ||
      deal.dealType === "black_friday"
    );
  }
  if (season === "holiday") {
    return deal.category !== "Groceries" && deal.category !== "Household";
  }
  if (season === "grocery") {
    return deal.category === "Groceries" || deal.category === "Household";
  }
  return true;
}

/** Extend deal type for cyber_monday in catalog entries */
export function isCyberType(type: DealType): boolean {
  return type === "cyber_monday" || type === "black_friday" || type === "flash";
}
