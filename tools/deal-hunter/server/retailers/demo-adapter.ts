import { CATALOG } from "../catalog.js";
import type { ProductSearchHit, RetailerAdapter, RetailerAdapterStatus } from "./types.js";

function slugForRetailer(name: string): string {
  return name.toLowerCase().replace(/\s+/g, "-");
}

export function createDemoRetailerAdapter(retailerName: string): RetailerAdapter {
  const slug = slugForRetailer(retailerName);
  return {
    slug,
    displayName: retailerName,
    integrationType: "demo",
    async getStatus(): Promise<RetailerAdapterStatus> {
      return {
        retailerSlug: slug,
        authenticationOk: true,
        lastSyncAt: new Date().toISOString(),
        lastError: null,
        dataFreshnessMinutes: 2,
        rateLimitRemaining: null,
        health: "healthy",
      };
    },
    async searchProducts(query: string, limit = 12): Promise<ProductSearchHit[]> {
      const q = query.toLowerCase();
      return CATALOG.filter(
        (d) => d.retailer === retailerName && (!q || d.title.toLowerCase().includes(q)),
      )
        .slice(0, limit)
        .map((d) => ({
          externalId: d.id,
          title: d.title,
          brand: d.title.split(" ")[0],
          url: d.url,
          price: d.price,
          currency: "USD",
          availability: "in_stock",
          imageUrl: d.image,
          shippingNote: d.freeShipping ? "Free shipping" : undefined,
        }));
    },
  };
}

export function demoAdapterRegistry(): RetailerAdapter[] {
  const names = [...new Set(CATALOG.map((d) => d.retailer))];
  return names.map(createDemoRetailerAdapter);
}
