"use client";

import { useEffect, useState } from "react";
import {
  fetchCommunityDeals,
  fetchDealsList,
  fetchLocalDeals,
  fetchRecommendations,
} from "@/lib/deal-api";
import type { CommunityDeal, Deal, ShoppingRecommendation } from "@/lib/types";
import { DealCard } from "@/components/deals/DealCard";
import { DealGridSkeleton } from "@/components/deals/DealGridSkeleton";
import { EmptyState } from "@/components/ui/empty-state";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Button } from "@/components/ui/button";
import { addDealToWishlist } from "@/app/wishlist/actions";

export function DiscoverClient() {
  const [loading, setLoading] = useState(true);
  const [community, setCommunity] = useState<CommunityDeal[]>([]);
  const [recs, setRecs] = useState<ShoppingRecommendation[]>([]);
  const [catalog, setCatalog] = useState<Deal[]>([]);
  const [metro, setMetro] = useState("");
  const [zip, setZip] = useState("78701");
  const [budget, setBudget] = useState(400);

  const load = async () => {
    setLoading(true);
    try {
      const [c, loc, r, deals] = await Promise.all([
        fetchCommunityDeals(),
        fetchLocalDeals(zip),
        fetchRecommendations({ zip, maxBudget: budget }),
        fetchDealsList(),
      ]);
      setCommunity(c.deals);
      setMetro(loc.metro);
      setRecs(r.recommendations);
      setCatalog(deals.data ?? []);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    void load();
  }, [zip, budget]);

  const recDeals = catalog.filter((d) => recs.some((x) => x.dealId === d.id));

  return (
    <div className="space-y-6">
      <Card>
        <CardHeader>
          <CardTitle className="text-base">Your discovery profile</CardTitle>
        </CardHeader>
        <CardContent className="grid gap-2 sm:grid-cols-2">
          <label className="text-sm text-bfm-muted">
            ZIP
            <Input value={zip} onChange={(e) => setZip(e.target.value)} className="mt-1" />
          </label>
          <label className="text-sm text-bfm-muted">
            Max budget
            <Input type="number" value={budget} onChange={(e) => setBudget(Number(e.target.value))} className="mt-1" />
          </label>
        </CardContent>
      </Card>

      {loading ? (
        <DealGridSkeleton />
      ) : (
        <>
          <section>
            <h3 className="mb-2 text-sm font-medium text-bfm-muted">Recommendations</h3>
            {recs.length === 0 ? (
              <EmptyState title="No recommendations yet" description="Adjust budget or refresh when API has deals." />
            ) : (
              <ul className="mb-4 space-y-2 text-sm">
                {recs.slice(0, 5).map((r) => (
                  <li key={r.dealId} className="rounded-lg border border-bfm-border bg-bfm-card px-3 py-2">
                    <strong>{r.title}</strong> — ${r.price.toFixed(2)} · {r.reason}
                  </li>
                ))}
              </ul>
            )}
            <div className="grid gap-4 sm:grid-cols-2">
              {recDeals.slice(0, 4).map((d) => (
                <DealCard
                  key={d.id}
                  deal={d}
                  onAddWishlist={(x) => void addDealToWishlist(x.id, x.title, x.price)}
                />
              ))}
            </div>
          </section>

          <section>
            <h3 className="mb-2 text-sm font-medium text-bfm-muted">Local near {metro}</h3>
            <EmptyState
              title="Partner circulars"
              description="Local rows use authorized store feeds — demo data for Austin area when ZIP 787xx."
              className="py-6"
            />
          </section>

          <section>
            <h3 className="mb-2 text-sm font-medium text-bfm-muted">Community (unverified tips)</h3>
            {community.length === 0 ? (
              <EmptyState title="No community tips" description="Submissions must use trusted retailer URLs." />
            ) : (
              <ul className="space-y-2 text-sm">
                {community.map((c) => (
                  <li key={c.id} className="flex justify-between rounded-lg border border-bfm-border px-3 py-2">
                    <span>
                      {c.title} — {c.retailer} ${c.price.toFixed(2)}
                    </span>
                    <span className="text-bfm-muted">▲ {c.upvotes}</span>
                  </li>
                ))}
              </ul>
            )}
          </section>
        </>
      )}

      <Button type="button" variant="outline" onClick={() => void load()}>
        Refresh
      </Button>
    </div>
  );
}
