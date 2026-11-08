"use client";

import { useEffect, useState } from "react";
import Link from "next/link";
import { fetchDealsTrending } from "@/lib/api";
import { DealGridSkeleton } from "@/components/deals/DealGridSkeleton";
import { EmptyState } from "@/components/ui/empty-state";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";

export function DashboardClient() {
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [gems, setGems] = useState<{ id: string; title: string; price: number }[]>([]);
  const [alerts, setAlerts] = useState<{ id: string; title: string; stealScore: number }[]>([]);

  useEffect(() => {
    void fetchDealsTrending()
      .then((t) => {
        setGems(t.hiddenGems ?? []);
        setAlerts(t.priceAlerts ?? []);
      })
      .catch(() => setError("Start the deal API: npm run dev:server (port 3847)."))
      .finally(() => setLoading(false));
  }, []);

  if (loading) return <DealGridSkeleton count={4} />;
  if (error) return <EmptyState title="API offline" description={error} />;

  return (
    <div className="grid gap-4 sm:grid-cols-2">
      <Card>
        <CardHeader>
          <CardTitle className="text-sm text-bfm-muted">Hidden gems</CardTitle>
        </CardHeader>
        <CardContent className="space-y-2 text-sm">
          {gems.slice(0, 5).map((d) => (
            <div key={d.id} className="flex justify-between gap-2 border-b border-bfm-border/50 pb-2">
              <Link href={`/product/${d.id}`} className="line-clamp-1 text-bfm-accent hover:underline">
                {d.title}
              </Link>
              <span className="text-bfm-accent-2">${d.price.toFixed(2)}</span>
            </div>
          ))}
          {gems.length === 0 && <p className="text-bfm-muted">No gems this refresh.</p>}
        </CardContent>
      </Card>
      <Card>
        <CardHeader>
          <CardTitle className="text-sm text-bfm-muted">Price alerts</CardTitle>
        </CardHeader>
        <CardContent className="space-y-2 text-sm">
          {alerts.slice(0, 5).map((d) => (
            <div key={d.id} className="flex justify-between gap-2">
              <Link href={`/product/${d.id}`} className="line-clamp-1 hover:text-bfm-accent">
                {d.title}
              </Link>
              <span className="text-bfm-success">Steal {d.stealScore}</span>
            </div>
          ))}
          {alerts.length === 0 && <p className="text-bfm-muted">No alerts.</p>}
        </CardContent>
      </Card>
    </div>
  );
}
