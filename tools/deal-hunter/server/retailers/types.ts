/** PRD Part 6 — common retailer adapter contract (authorized sources only) */

export type IntegrationHealth = "healthy" | "degraded" | "offline" | "unconfigured";

export interface RetailerAdapterStatus {
  retailerSlug: string;
  authenticationOk: boolean;
  lastSyncAt: string | null;
  lastError: string | null;
  dataFreshnessMinutes: number | null;
  rateLimitRemaining: number | null;
  health: IntegrationHealth;
}

export interface ProductSearchHit {
  externalId: string;
  title: string;
  brand?: string;
  model?: string;
  upc?: string;
  url: string;
  price: number | null;
  currency: string;
  availability: "in_stock" | "limited" | "out_of_stock" | "unknown";
  imageUrl?: string;
  shippingNote?: string;
  dataUnavailable?: boolean;
}

export interface RetailerAdapter {
  readonly slug: string;
  readonly displayName: string;
  readonly integrationType: "official_api" | "affiliate_feed" | "partner_file" | "demo";
  getStatus(): Promise<RetailerAdapterStatus>;
  searchProducts(query: string, limit?: number): Promise<ProductSearchHit[]>;
  getProduct?(externalId: string): Promise<ProductSearchHit | null>;
}
