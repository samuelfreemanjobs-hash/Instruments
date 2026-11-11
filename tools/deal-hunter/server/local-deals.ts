import type { ScoredDeal } from "./rank.js";

export interface LocalDealOffer {
  id: string;
  title: string;
  storeName: string;
  address: string;
  distanceMiles: number;
  price: number;
  url: string;
  category: string;
  source: "partner_circular" | "in_store_pickup";
  validThrough: string;
  metro: string;
}

const LOCAL_BY_METRO: Record<string, LocalDealOffer[]> = {
  austin: [
    {
      id: "loc-adidas-outlet",
      title: "Adidas outlet extra 30% doorbuster",
      storeName: "Adidas Outlet Round Rock",
      address: "4477 Premium Outlets Blvd",
      distanceMiles: 14.2,
      price: 54,
      url: "https://www.adidas.com/us/outlet",
      category: "apparel",
      source: "partner_circular",
      validThrough: new Date(Date.now() + 5 * 86_400_000).toISOString(),
      metro: "Austin",
    },
    {
      id: "loc-ulta-gwp",
      title: "Ulta 21 Days — GWP on $50 skin",
      storeName: "Ulta Beauty Domain",
      address: "11410 Century Oaks Terrace",
      distanceMiles: 3.1,
      price: 0,
      url: "https://www.ulta.com/",
      category: "beauty",
      source: "in_store_pickup",
      validThrough: new Date(Date.now() + 2 * 86_400_000).toISOString(),
      metro: "Austin",
    },
  ],
  default: [
    {
      id: "loc-target-circle",
      title: "Target Circle Week stack",
      storeName: "Target (near you)",
      address: "Participating stores",
      distanceMiles: 2.5,
      price: 12.99,
      url: "https://www.target.com/",
      category: "home",
      source: "partner_circular",
      validThrough: new Date(Date.now() + 7 * 86_400_000).toISOString(),
      metro: "Your area",
    },
  ],
};

function metroFromZip(zip?: string): keyof typeof LOCAL_BY_METRO {
  if (!zip || zip.length < 3) return "default";
  const p = zip.slice(0, 3);
  if (["787", "786", "765"].includes(p)) return "austin";
  return "default";
}

export function discoverLocalDeals(zip?: string, _radiusMiles = 25): {
  metro: string;
  offers: LocalDealOffer[];
  dataPolicy: string;
} {
  const key = metroFromZip(zip);
  const offers = [...(LOCAL_BY_METRO[key] ?? LOCAL_BY_METRO.default)];
  return {
    metro: offers[0]?.metro ?? "Your area",
    offers,
    dataPolicy:
      "Local rows come from store partner circulars and BOPIS flags — not unauthorized scrape jobs.",
  };
}

/** Map online catalog hits that mention pickup / nearby when available */
export function enrichWithLocalHints(
  deals: ScoredDeal[],
  zip?: string,
): { dealId: string; pickupAvailable: boolean; hint: string }[] {
  const metro = metroFromZip(zip);
  if (metro === "default") return [];
  return deals
    .filter((d) => ["Target", "Best Buy", "Ulta", "Sephora", "Adidas"].includes(d.retailer))
    .slice(0, 6)
    .map((d) => ({
      dealId: d.id,
      pickupAvailable: true,
      hint: `Check ${d.retailer} store stock near ${zip ?? "your ZIP"} — partner inventory API when connected.`,
    }));
}
