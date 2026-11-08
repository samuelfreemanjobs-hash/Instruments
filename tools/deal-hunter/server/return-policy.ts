import type { CatalogDeal } from "./catalog.js";
import type { ItemCondition } from "./deal-enrich.js";

export interface ReturnPolicy {
  days: number;
  tier: "30" | "60" | "90" | "other";
  summary: string;
}

const BY_RETAILER: Record<string, ReturnPolicy> = {
  Amazon: { days: 30, tier: "30", summary: "30-day returns on most items (policy varies by category)." },
  "Amazon Warehouse": { days: 30, tier: "30", summary: "30-day return window; condition as listed." },
  Walmart: { days: 30, tier: "30", summary: "30-day free returns on millions of items." },
  Target: { days: 30, tier: "30", summary: "30-day returns; 90 days with Target Circle card on many items." },
  "Best Buy": { days: 60, tier: "60", summary: "60-day return & exchange on most products (15 days on activatable devices)." },
  Costco: { days: 90, tier: "90", summary: "Most electronics: 90-day return; satisfaction guarantee on many goods." },
  Sephora: { days: 30, tier: "30", summary: "30-day returns with receipt; saleable condition." },
  Ulta: { days: 30, tier: "30", summary: "30-day returns; opened beauty may be store credit only." },
  Adidas: { days: 30, tier: "30", summary: "30-day free returns on adidas.com orders." },
  "Guitar Center": { days: 45, tier: "other", summary: "45-day hassle-free returns on gear." },
  Sweetwater: { days: 30, tier: "30", summary: "30-day money-back guarantee on most gear." },
  REI: { days: 365, tier: "other", summary: "One-year satisfaction guarantee for members on most items." },
  Chewy: { days: 365, tier: "other", summary: "365-day satisfaction guarantee on pet food." },
  eBay: { days: 30, tier: "30", summary: "Seller/eBay Money Back Guarantee — often 30 days if eligible." },
};

const OPEN_BOX_POLICY: ReturnPolicy = {
  days: 15,
  tier: "30",
  summary: "Open-box: typically 15–30 days; confirm on product page.",
};

export function returnPolicyFor(deal: {
  retailer: string;
  dealType: CatalogDeal["dealType"];
  itemCondition?: ItemCondition;
}): ReturnPolicy {
  if (deal.dealType === "open_box" || deal.itemCondition === "open_box") {
    return OPEN_BOX_POLICY;
  }
  return (
    BY_RETAILER[deal.retailer] ?? {
      days: 30,
      tier: "30",
      summary: "30-day returns assumed — verify on retailer site before buying.",
    }
  );
}
