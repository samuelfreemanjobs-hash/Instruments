import { dealApiOriginForServer, shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";
import { hostedDealsList } from "@/lib/hosted-catalog";

function fetchBase() {
  if (typeof window !== "undefined") return "";
  if (!shouldProxyToExternalDealApi()) return "";
  return dealApiOriginForServer();
}

export async function fetchDealsTrending() {
  if (!shouldProxyToExternalDealApi()) {
    const deals = hostedDealsList().sort((a, b) => b.stealScore - a.stealScore);
    return {
      hiddenGems: deals.filter((d) => d.stealScore >= 78).slice(0, 8),
      priceAlerts: deals.filter((d) => d.discountPct >= 35).slice(0, 8),
      fetchedAt: new Date().toISOString(),
    };
  }
  const base = fetchBase();
  const res = await fetch(`${base}/api/v1/deals/trending`, { next: { revalidate: 90 } });
  if (!res.ok) throw new Error("trending_failed");
  return res.json();
}

export async function fetchV1Health() {
  if (!shouldProxyToExternalDealApi()) {
    return { ok: true, product: "black-friday-madness", mode: "bundled_demo" };
  }
  const base = fetchBase();
  const res = await fetch(`${base}/api/v1/health`, { cache: "no-store" });
  return res.json() as Promise<{ ok: boolean; product: string }>;
}
