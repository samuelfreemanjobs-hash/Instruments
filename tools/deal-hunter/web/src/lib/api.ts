const base =
  typeof window === "undefined"
    ? process.env.DEAL_API_ORIGIN ?? "http://127.0.0.1:3847"
    : "";

export async function fetchDealsTrending() {
  const res = await fetch(`${base}/api/v1/deals/trending`, { next: { revalidate: 90 } });
  if (!res.ok) throw new Error("trending_failed");
  return res.json();
}

export async function fetchV1Health() {
  const res = await fetch(`${base}/api/v1/health`, { cache: "no-store" });
  return res.json() as Promise<{ ok: boolean; product: string }>;
}
