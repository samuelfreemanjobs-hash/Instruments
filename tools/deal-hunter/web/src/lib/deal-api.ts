import type { CommunityDeal, Deal, ShoppingRecommendation } from "./types";
import type { CatalogSearchHit } from "./supabase/catalog-search";
import { catalogHitsToDeals } from "./catalog-deal-map";

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

export async function fetchCatalogSource() {
  const res = await fetch("/api/catalog/source", { cache: "no-store" });
  if (!res.ok) throw new Error("catalog_source_failed");
  return res.json() as Promise<{ source: string; useSupabase: boolean }>;
}

export async function runPricingCompare(question: string, dealIds?: string[]) {
  const res = await fetch("/api/agents/pricing", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ question, dealIds }),
  });
  if (!res.ok) throw new Error("pricing_failed");
  return res.json() as Promise<{
    summary: string;
    valueVerdict: string;
    bestPick: Deal | null;
    bestValueScore: number;
    alternatives: Deal[];
    couponTip: string;
    savingsBreakdown: string;
    confidence: string;
    analyzedAt: string;
  }>;
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

export async function fetchRecommendations(profile: {
  zip?: string;
  maxBudget?: number;
  categories?: string[];
}) {
  const res = await fetch("/api/recommendations", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(profile),
  });
  return res.json() as Promise<{ recommendations: ShoppingRecommendation[] }>;
}

export async function fetchDealsList() {
  if (process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG === "1") {
    const res = await fetch("/api/catalog/search");
    if (res.ok) {
      const body = (await res.json()) as { hits: CatalogSearchHit[]; count: number };
      if (body.count > 0) return { data: catalogHitsToDeals(body.hits) };
    }
  }
  const res = await fetch("/api/v1/deals?");
  return res.json() as Promise<{ data: Deal[] }>;
}

export async function fetchProductDetailClient(id: string) {
  const res = await fetch(`/api/v1/deals/${encodeURIComponent(id)}`);
  if (res.status === 404) return null;
  if (!res.ok) throw new Error("product_failed");
  return res.json();
}

export async function fetchIntelligenceBriefing() {
  const res = await fetch("/api/intelligence/briefing");
  if (!res.ok) throw new Error("briefing_failed");
  return res.json() as Promise<{
    headline: string;
    buyNow: { title: string; verdictHeadline: string; dealId: string }[];
    waitList: { title: string; dealId: string }[];
    marketPulse: string[];
    priceDropsToday: number;
    avgRealSavingsPct: number;
    avgAdvertisedDiscountPct: number;
    categoryTrends?: { category: string; avgRealSavingsPct: number; dealCount: number }[];
    dataFreshThrough: string;
  }>;
}

export async function askIntelligence(question: string) {
  const res = await fetch("/api/intelligence/ask", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ question }),
  });
  if (!res.ok) throw new Error("ask_failed");
  return res.json() as Promise<{ answer: string }>;
}

export async function fetchAutopilotAgents() {
  const res = await fetch("/api/autopilot/agents");
  return res.json() as Promise<{ agents: { id: string; name: string; mission: string; scheduleHint: string }[] }>;
}

export async function fetchCoupons() {
  const res = await fetch("/api/coupons");
  if (!res.ok) throw new Error("coupons_failed");
  return res.json() as Promise<{
    coupons: {
      dealId: string;
      title: string;
      retailer: string;
      code: string;
      description: string;
      expiresAt?: string;
      finalPrice: number;
      stealScore: number;
    }[];
    count: number;
  }>;
}

export async function fetchRetailCalendar() {
  const res = await fetch("/api/calendar");
  if (!res.ok) throw new Error("calendar_failed");
  return res.json() as Promise<{
    current: { label: string; focus: string; season: string };
    events: {
      id: string;
      label: string;
      startsAt: string;
      description: string;
      peakEvent: string | null;
    }[];
  }>;
}

export async function fetchStealHits(minStealScore: number) {
  const res = await fetch(`/api/notifications/steal-hits?minStealScore=${minStealScore}`);
  if (!res.ok) throw new Error("steal_hits_failed");
  return res.json() as Promise<{ hits: { dealId: string; title: string; stealScore: number; price: number }[] }>;
}

export async function registerStealAlertBackend(body: {
  endpoint: string;
  minStealScore: number;
  email?: string;
  keys?: { p256dh: string; auth: string };
}) {
  const res = await fetch("/api/notifications/subscribe", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      endpoint: body.endpoint,
      keys: body.keys ?? { p256dh: "browser-local", auth: "browser-local" },
      minStealScore: body.minStealScore,
      notifyEmail: Boolean(body.email),
      email: body.email,
    }),
  });
  if (!res.ok) throw new Error("subscribe_failed");
  return res.json();
}

export async function patchStealAlertPrefs(endpoint: string, minStealScore: number) {
  await fetch("/api/notifications/subscribe", {
    method: "PATCH",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ endpoint, minStealScore }),
  });
}

export async function fetchAutopilotDigest(profile: { zip?: string; maxBudget?: number }) {
  const res = await fetch("/api/autopilot/digest", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ profile }),
  });
  if (!res.ok) throw new Error("autopilot_failed");
  return res.json() as Promise<{
    runId: string;
    generatedAt: string;
    topVerdicts: string[];
    agents: { agentId: string; title: string; narrative: string; picks: { id: string; label: string; detail: string }[] }[];
  }>;
}
