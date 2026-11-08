import { fetchV1Health } from "@/lib/api";
import { DashboardClient } from "./dashboard-client";

export default async function DashboardPage() {
  let health: { product?: string } = {};
  try {
    health = await fetchV1Health();
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
        {health.product && <p className="mt-1 text-xs text-bfm-accent-2">API: {health.product}</p>}
      </section>
      <DashboardClient />
    </div>
  );
}
