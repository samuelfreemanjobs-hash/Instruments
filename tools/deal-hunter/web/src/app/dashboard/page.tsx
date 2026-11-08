import { fetchDealsTrending, fetchV1Health } from "@/lib/api";

export const dynamic = "force-dynamic";

export default async function DashboardPage() {
  let trending: Awaited<ReturnType<typeof fetchDealsTrending>> | null = null;
  let error: string | null = null;
  let health: { product?: string } = {};

  try {
    health = await fetchV1Health();
    trending = await fetchDealsTrending();
  } catch {
    error = "Start the deal API: npm run dev:server (port 3847).";
  }

  const gems = trending?.hiddenGems ?? [];
  const alerts = trending?.priceAlerts ?? [];

  return (
    <div className="space-y-6">
      <section>
        <h2 className="text-lg font-semibold">Dashboard</h2>
        <p className="mt-1 text-sm text-bfm-muted">
          Command center — featured steals, drops, and personalized feed. Deal scores use verified history, not ad %.
        </p>
        {health.product && (
          <p className="mt-2 text-xs text-bfm-accent-2">API: {health.product}</p>
        )}
      </section>

      {error && (
        <div className="rounded-lg border border-bfm-danger/50 bg-bfm-card p-4 text-sm text-bfm-danger">{error}</div>
      )}

      <section className="grid gap-4 sm:grid-cols-2">
        <article className="rounded-xl border border-bfm-border bg-bfm-card p-4">
          <h3 className="text-sm font-medium text-bfm-muted">Hidden gems</h3>
          <ul className="mt-3 space-y-2 text-sm">
            {gems.slice(0, 5).map((d: { id: string; title: string; price: number }) => (
              <li key={d.id} className="flex justify-between gap-2 border-b border-bfm-border/60 pb-2">
                <span className="line-clamp-1">{d.title}</span>
                <span className="shrink-0 text-bfm-accent">${d.price.toFixed(2)}</span>
              </li>
            ))}
            {gems.length === 0 && <li className="text-bfm-muted">No data yet.</li>}
          </ul>
        </article>
        <article className="rounded-xl border border-bfm-border bg-bfm-card p-4">
          <h3 className="text-sm font-medium text-bfm-muted">Price alerts</h3>
          <ul className="mt-3 space-y-2 text-sm">
            {alerts.slice(0, 5).map((d: { id: string; title: string; stealScore: number }) => (
              <li key={d.id} className="flex justify-between gap-2">
                <span className="line-clamp-1">{d.title}</span>
                <span className="text-bfm-success">Steal {d.stealScore}</span>
              </li>
            ))}
            {alerts.length === 0 && <li className="text-bfm-muted">No alerts this refresh.</li>}
          </ul>
        </article>
      </section>

      <p className="text-xs text-bfm-muted">
        Legacy Vite PWA remains at <code className="text-white">npm run dev:legacy</code> until feature parity.
      </p>
    </div>
  );
}
