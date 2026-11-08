"use client";

import { useCallback, useEffect, useState } from "react";
import { DealCard } from "@/components/deals/DealCard";
import { DealGridSkeleton } from "@/components/deals/DealGridSkeleton";
import { EmptyState } from "@/components/ui/empty-state";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { addDealToWishlist } from "@/app/wishlist/actions";
import { fetchCategories, universalSearch } from "@/lib/deal-api";
import type { Deal } from "@/lib/types";

export function SearchClient() {
  const [q, setQ] = useState("");
  const [retailer, setRetailer] = useState("");
  const [maxPrice, setMaxPrice] = useState("");
  const [freeShip, setFreeShip] = useState(false);
  const [hits, setHits] = useState<Deal[]>([]);
  const [fetchedAt, setFetchedAt] = useState("");
  const [loading, setLoading] = useState(true);
  const [retailers, setRetailers] = useState<string[]>([]);
  const [wishlistIds, setWishlistIds] = useState<Set<string>>(new Set());
  const [err, setErr] = useState<string | null>(null);

  useEffect(() => {
    void fetchCategories().then((c) => setRetailers(c.retailers));
  }, []);

  const run = useCallback(async () => {
    setLoading(true);
    setErr(null);
    try {
      const res = await universalSearch({
        q: q || undefined,
        retailer: retailer || undefined,
        maxPrice: maxPrice ? Number(maxPrice) : undefined,
        freeShippingOnly: freeShip,
        trustedOnly: true,
      });
      setHits(res.hits);
      setFetchedAt(res.fetchedAt);
    } catch {
      setErr("Search failed — is the deal API running?");
    } finally {
      setLoading(false);
    }
  }, [q, retailer, maxPrice, freeShip]);

  useEffect(() => {
    void run();
  }, [run]);

  const addWish = async (deal: Deal) => {
    const r = await addDealToWishlist(deal.id, deal.title, deal.price);
    if (r.ok) setWishlistIds((s) => new Set(s).add(deal.id));
  };

  return (
    <div className="space-y-4">
      <form
        className="grid gap-2 sm:grid-cols-2"
        onSubmit={(e) => {
          e.preventDefault();
          void run();
        }}
      >
        <Input placeholder="Search products…" value={q} onChange={(e) => setQ(e.target.value)} />
        <select
          className="min-h-10 rounded-lg border border-bfm-border bg-bfm-bg-2 px-3 text-sm"
          value={retailer}
          onChange={(e) => setRetailer(e.target.value)}
        >
          <option value="">All retailers</option>
          {retailers.map((r) => (
            <option key={r} value={r}>
              {r}
            </option>
          ))}
        </select>
        <Input
          placeholder="Max price"
          type="number"
          value={maxPrice}
          onChange={(e) => setMaxPrice(e.target.value)}
        />
        <label className="flex items-center gap-2 text-sm text-bfm-muted">
          <input type="checkbox" checked={freeShip} onChange={(e) => setFreeShip(e.target.checked)} />
          Free shipping only
        </label>
        <Button type="submit" disabled={loading}>
          Search
        </Button>
      </form>

      {fetchedAt && <p className="text-xs text-bfm-muted">Updated {new Date(fetchedAt).toLocaleTimeString()}</p>}
      {err && <p className="text-sm text-bfm-danger">{err}</p>}
      {loading && <DealGridSkeleton />}
      {!loading && hits.length === 0 && (
        <EmptyState title="No matches" description="Try a broader query or clear retailer filters." />
      )}
      {!loading && hits.length > 0 && (
        <div className="grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
          {hits.map((d) => (
            <DealCard key={d.id} deal={d} onAddWishlist={(x) => void addWish(x)} added={wishlistIds.has(d.id)} />
          ))}
        </div>
      )}
    </div>
  );
}
