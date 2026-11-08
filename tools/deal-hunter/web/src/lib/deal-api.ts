import type { CommunityDeal, Deal, ShoppingRecommendation } from "./types";

export async function universalSearch(params: Record<string, string | number | boolean | undefined>) {
  const sp = new URLSearchParams();
  Object.entries(params).forEach(([k, v]) => {
    if (v === undefined || v === "" || v === false) return;
    if (typeof v === "boolean") sp.set(k, String(v));
    else sp.set(k, String(v));
  });
  sp.set("includeIntelligence", "true");
  const res = await fetch(`/api/search/universal?${sp}`);
  if (!res.ok) throw new Error("search_failed");
  return res.json() as Promise<{ hits: Deal[]; fetchedAt: string; count: number }>;
}

export async function fetchCategories() {
  const res = await fetch("/api/categories");
  return res.json() as Promise<{ categories: string[]; retailers: string[] }>;
}

export async function chatAssistant(message: string) {
  const res = await fetch("/api/orchestrator/chat", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ message }),
  });
  if (!res.ok) throw new Error("chat_failed");
  return res.json() as Promise<{
    answer: string;
    evidence: { summary: string }[];
    agentsInvoked: string[];
    generatedAt: string;
  }>;
}

export async function chatAssistantLegacy(message: string) {
  const res = await fetch("/api/assistant/chat", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ message, watches: [] }),
  });
  if (!res.ok) throw new Error("assistant_failed");
  return res.json() as Promise<{
    message: string;
    deals: Deal[];
    intelligenceSummary?: string;
    generatedAt: string;
  }>;
}

export async function fetchCommunityDeals() {
  const res = await fetch("/api/discover/community");
  return res.json() as Promise<{ deals: CommunityDeal[] }>;
}

export async function fetchLocalDeals(zip?: string) {
  const q = zip ? `?zip=${encodeURIComponent(zip)}` : "";
  const res = await fetch(`/api/discover/local${q}`);
  return res.json() as Promise<{ metro: string; offers: { id: string; title: string; storeName: string; price: number; url: string }[] }>;
}

export async function fetchRecommendations(profile: { zip?: string; maxBudget?: number }) {
  const res = await fetch("/api/recommendations", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(profile),
  });
  return res.json() as Promise<{ recommendations: ShoppingRecommendation[] }>;
}

export async function fetchDealsList() {
  const res = await fetch("/api/v1/deals?");
  return res.json() as Promise<{ data: Deal[] }>;
}
