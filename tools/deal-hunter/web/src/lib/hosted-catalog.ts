import catalog from "@/data/hosted-catalog.json";
import type { Deal } from "@/lib/types";

type HostedPayload = typeof catalog;

let cache: HostedPayload | null = null;

export function hostedCatalog(): HostedPayload {
  if (!cache) cache = catalog as HostedPayload;
  return cache;
}

export function hostedDealsList(minScore = 0): Deal[] {
  const { hits } = hostedCatalog();
  if (minScore <= 0) return hits as Deal[];
  return (hits as Deal[]).filter((d) => d.stealScore >= minScore);
}

export function hostedProductDetail(id: string) {
  const products = hostedCatalog().products as Record<string, unknown>;
  return products[id] ?? null;
}

export function hostedUniversalSearch(params: URLSearchParams) {
  const q = (params.get("q") ?? "").trim().toLowerCase();
  const retailer = params.get("retailer")?.toLowerCase();
  const category = params.get("category")?.toLowerCase();
  let hits = hostedDealsList() as Deal[];
  if (q) {
    hits = hits.filter((d) => `${d.title} ${d.retailer} ${d.category}`.toLowerCase().includes(q));
  }
  if (retailer) hits = hits.filter((d) => d.retailer.toLowerCase().includes(retailer));
  if (category) hits = hits.filter((d) => d.category.toLowerCase().includes(category));
  return { hits, count: hits.length, fetchedAt: new Date().toISOString() };
}
