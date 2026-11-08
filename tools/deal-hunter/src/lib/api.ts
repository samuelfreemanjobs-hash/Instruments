import type {
  ConditionFilter,
  DashboardPayload,
  Deal,
  DealType,
  IntelligenceBriefing,
  SeasonInfo,
  ShoppingSeason,
} from "../types";

export interface SearchParams {
  q?: string;
  maxPrice?: number;
  minPrice?: number;
  minDiscount?: number;
  freeShippingOnly?: boolean;
  dealTypes?: DealType[];
  minStealScore?: number;
  category?: string;
  conditionFilter?: ConditionFilter;
  couponsOnly?: boolean;
  trustedOnly?: boolean;
  season?: ShoppingSeason;
  brand?: string;
  retailer?: string;
  model?: string;
  upc?: string;
  specKey?: string;
  specValue?: string;
  includeIntelligence?: boolean;
}

export interface UniversalSearchResult {
  hits: Deal[];
  count: number;
  fetchedAt: string;
}

export async function fetchDashboard(): Promise<DashboardPayload> {
  const res = await fetch("/api/dashboard");
  if (!res.ok) throw new Error("dashboard_failed");
  return res.json() as Promise<DashboardPayload>;
}

export async function fetchIntelligenceBriefing(): Promise<IntelligenceBriefing> {
  const res = await fetch("/api/intelligence/briefing");
  if (!res.ok) throw new Error("briefing_failed");
  return res.json() as Promise<IntelligenceBriefing>;
}

export async function askIntelligence(question: string) {
  const res = await fetch("/api/intelligence/ask", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ question }),
  });
  if (!res.ok) throw new Error("ask_failed");
  return res.json() as Promise<{ answer: string; picks: import("../types").DealIntelligence[] }>;
}

export async function universalSearch(params: SearchParams): Promise<UniversalSearchResult> {
  const sp = new URLSearchParams();
  Object.entries(params).forEach(([k, v]) => {
    if (v === undefined || v === "" || v === false) return;
    if (k === "dealTypes" && Array.isArray(v)) sp.set("dealTypes", v.join(","));
    else if (k === "includeIntelligence" && v) sp.set("includeIntelligence", "true");
    else if (typeof v === "boolean") sp.set(k, String(v));
    else sp.set(k, String(v));
  });
  const res = await fetch(`/api/search/universal?${sp}`);
  if (!res.ok) throw new Error("search_failed");
  return res.json() as Promise<UniversalSearchResult>;
}

export async function searchDeals(params: SearchParams): Promise<Deal[]> {
  const data = await universalSearch({ ...params, includeIntelligence: true });
  return data.hits;
}

export async function searchByImageRgb(r: number, g: number, b: number) {
  const res = await fetch("/api/search/image", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ r, g, b }),
  });
  if (!res.ok) throw new Error("image_search_failed");
  return res.json() as Promise<{ visualTags: string[]; hits: Deal[]; fetchedAt: string; note: string }>;
}

export async function fetchCategories(): Promise<{
  categories: string[];
  brands: string[];
  retailers: string[];
}> {
  const res = await fetch("/api/categories");
  return res.json() as Promise<{ categories: string[]; brands: string[]; retailers: string[] }>;
}

export async function fetchSeason(): Promise<SeasonInfo> {
  const res = await fetch("/api/season");
  return res.json() as Promise<SeasonInfo>;
}

export interface ListItem {
  dealId: string;
  note?: string;
  addedAt: string;
}

export async function createSharedList(title: string, items: ListItem[]): Promise<string> {
  const res = await fetch("/api/lists", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ title, items }),
  });
  if (!res.ok) throw new Error("create_list_failed");
  const data = (await res.json()) as { shareCode: string };
  return data.shareCode;
}

export async function loadSharedList(shareCode: string) {
  const res = await fetch(`/api/lists/${encodeURIComponent(shareCode)}`);
  if (!res.ok) throw new Error("list_not_found");
  return res.json() as Promise<{
    shareCode: string;
    title: string;
    updatedAt: string;
    raw: { title: string; items: ListItem[] };
  }>;
}

export async function saveSharedList(
  shareCode: string,
  title: string,
  items: ListItem[],
): Promise<void> {
  const res = await fetch(`/api/lists/${encodeURIComponent(shareCode)}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ title, items }),
  });
  if (!res.ok) throw new Error("save_list_failed");
}

export async function runPricingAgent(question: string, dealIds?: string[]) {
  const res = await fetch("/api/agents/pricing", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ question, dealIds }),
  });
  return res.json();
}

export async function chatAssistant(
  message: string,
  watches: { id: string; productQuery: string; targetPrice: number }[],
) {
  const res = await fetch("/api/assistant/chat", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ message, watches }),
  });
  if (!res.ok) throw new Error("assistant_failed");
  return res.json();
}

export async function evaluateTargetWatches(
  watches: { id: string; productQuery: string; targetPrice: number }[],
) {
  const res = await fetch("/api/target-watches/evaluate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ watches }),
  });
  if (!res.ok) throw new Error("watch_eval_failed");
  return res.json() as Promise<{
    results: {
      watchId: string;
      productQuery: string;
      targetPrice: number;
      targetReached: boolean;
      participatingOffers: { retailer: string; price: number; atTarget: boolean }[];
    }[];
    fetchedAt: string;
  }>;
}

export async function runDecorAgent(body: {
  room: string;
  style: string;
  budgetMax?: number;
  colors?: string;
}) {
  const res = await fetch("/api/agents/decor", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  return res.json();
}
