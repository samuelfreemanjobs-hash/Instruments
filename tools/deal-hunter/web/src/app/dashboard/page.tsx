import { fetchV1Health } from "@/lib/api";
import { fetchCatalogSourceServer } from "@/lib/server-deal-api";
import { DashboardClient } from "./dashboard-client";

export default async function DashboardPage() {
  let health: { product?: string } = {};
  let catalogSource = "local";
  try {
    health = await fetchV1Health();
    const src = await fetchCatalogSourceServer();
    catalogSource = src.source;
  } catch {
    /* client shows empty state */
  }

  return (
    <div className="space-y-6">
      <section>
        <h2 className="text-lg font-semibold">Dashboard</h2>
        <p className="mt-1 text-sm text-bfm-muted">
          Featured steals and verified drops — Deal Intelligence Score on every offer.
        </p>
        <p className="mt-1 text-xs text-bfm-accent-2">
          API: {health.product ?? "offline"} · catalog: {catalogSource}
        </p>
      </section>
      <DashboardClient />
    </div>
  );
}
