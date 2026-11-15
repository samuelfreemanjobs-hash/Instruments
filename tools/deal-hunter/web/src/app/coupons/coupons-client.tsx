"use client";

import { useEffect, useState } from "react";
import Link from "next/link";

import { fetchCoupons } from "@/lib/deal-api";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { EmptyState } from "@/components/ui/empty-state";

export function CouponsClient() {
  const [coupons, setCoupons] = useState<Awaited<ReturnType<typeof fetchCoupons>>["coupons"]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    void fetchCoupons()
      .then((r) => setCoupons(r.coupons))
      .catch(() => setCoupons([]))
      .finally(() => setLoading(false));
  }, []);

  if (loading) return <Skeleton className="h-40 w-full" />;

  if (coupons.length === 0) {
    return (
      <EmptyState
        title="No verified coupons"
        description="Codes appear only when verification status is known — always screenshot checkout totals."
      />
    );
  }

  return (
    <ul className="space-y-3">
      {coupons.map((c) => (
        <li key={c.dealId}>
          <Card>
            <CardContent className="space-y-1 pt-4 text-sm">
              <p className="font-medium">
                <Link href={`/product/${c.dealId}`} className="text-bfm-accent hover:underline">
                  {c.title}
                </Link>
              </p>
              <p className="text-bfm-muted">
                {c.retailer} · steal {c.stealScore} · est. pay ${c.finalPrice.toFixed(2)}
              </p>
              <p>
                Code <span className="rounded bg-bfm-bg-2 px-2 py-0.5 font-mono text-bfm-accent-2">{c.code}</span>
              </p>
              <p className="text-xs text-bfm-muted">{c.description}</p>
              {c.expiresAt && (
                <p className="text-xs text-bfm-warning">Expires {new Date(c.expiresAt).toLocaleDateString()}</p>
              )}
            </CardContent>
          </Card>
        </li>
      ))}
    </ul>
  );
}
