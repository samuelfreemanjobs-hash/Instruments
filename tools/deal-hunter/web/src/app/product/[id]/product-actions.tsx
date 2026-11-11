"use client";

import Link from "next/link";

import { addDealToWishlist } from "@/app/wishlist/actions";
import { createPriceAlert } from "@/app/alerts/actions";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { useState } from "react";

export function ProductActions({
  dealId,
  title,
  price,
  safeUrl,
  storeUrl,
}: {
  dealId: string;
  title: string;
  price: number;
  safeUrl: boolean;
  storeUrl: string;
}) {
  const [target, setTarget] = useState(String(Math.max(1, Math.floor(price * 0.9))));
  const [msg, setMsg] = useState<string | null>(null);

  return (
    <div className="flex flex-col gap-2">
      <div className="flex flex-wrap gap-2">
        {safeUrl && (
          <a href={storeUrl} target="_blank" rel="noreferrer noopener" className="text-sm text-bfm-accent underline">
            Open at retailer (checkout there)
          </a>
        )}
        <Button
          type="button"
          variant="outline"
          onClick={() =>
            void addDealToWishlist(dealId, title, price).then((r) =>
              setMsg(r.ok ? "Added to wishlist" : "Sign in to sync wishlist"),
            )
          }
        >
          + Wishlist
        </Button>
      </div>
      <form
        className="flex flex-wrap items-end gap-2"
        onSubmit={(e) => {
          e.preventDefault();
          void createPriceAlert(dealId, Number(target)).then((r) =>
            setMsg(r.ok ? "Price alert saved" : r.error ?? "Could not save alert"),
          );
        }}
      >
        <label className="text-xs text-bfm-muted">
          Target price $
          <Input type="number" value={target} onChange={(e) => setTarget(e.target.value)} className="mt-1 w-28" />
        </label>
        <Button type="submit" variant="outline">
          Watch price
        </Button>
      </form>
      {msg && <p className="text-xs text-bfm-muted">{msg}</p>}
      <p className="text-xs text-bfm-muted">
        <Link href="/login" className="text-bfm-accent">
          Sign in
        </Link>{" "}
        to persist alerts in Supabase.
      </p>
    </div>
  );
}
