/**
 * Retailer data must come from authorized APIs, affiliate product feeds,
 * or other contracts that explicitly permit price/deal use. Demo catalog only.
 */

export type DataSourceKind =
  | "affiliate_feed"
  | "official_api"
  | "partner_file"
  | "community_moderated"
  | "demo_curated";

export interface DataSourceRecord {
  id: string;
  retailer: string;
  kind: DataSourceKind;
  permitted: boolean;
  notes: string;
}

export const AUTHORIZED_SOURCES: readonly DataSourceRecord[] = [
  {
    id: "amazon-paapi",
    retailer: "Amazon",
    kind: "official_api",
    permitted: true,
    notes: "Product Advertising API — requires associate tag + compliance.",
  },
  {
    id: "rakuten-adidas",
    retailer: "Adidas",
    kind: "affiliate_feed",
    permitted: true,
    notes: "Network affiliate product feed; honor refresh limits.",
  },
  {
    id: "cj-sephora",
    retailer: "Sephora",
    kind: "affiliate_feed",
    permitted: true,
    notes: "Commission Junction / partner catalog.",
  },
  {
    id: "local-pos-partner",
    retailer: "Local stores",
    kind: "partner_file",
    permitted: true,
    notes: "Store-submitted circulars; geo-scoped; no drive-by scraping.",
  },
  {
    id: "community",
    retailer: "Various",
    kind: "community_moderated",
    permitted: true,
    notes: "User tips on trusted domains only; moderators verify URL allowlist.",
  },
  {
    id: "demo-catalog",
    retailer: "Demo",
    kind: "demo_curated",
    permitted: true,
    notes: "In-repo sample data for UX — not live retailer scraping.",
  },
] as const;

export function isScrapingAssumed(): false {
  return false;
}

export function policySummary(): string {
  return (
    "Black Friday HQ does not assume unrestricted scraping. Connect affiliate feeds, " +
    "PA-API, or signed partner files via n8n/ETL; block unknown origins."
  );
}
