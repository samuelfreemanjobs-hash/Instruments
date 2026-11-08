"use client";

import Link from "next/link";
import type { Deal } from "@/lib/types";
import { Card, CardContent } from "@/components/ui/card";
import { Button } from "@/components/ui/button";

export function DealCard({
  deal,
  onAddWishlist,
  added,
}: {
  deal: Deal;
  onAddWishlist?: (deal: Deal) => void;
  added?: boolean;
}) {
  const score = deal.intelligence?.dealIntelligenceScore;
  const real = deal.intelligence?.realSavingsVsAvgPct ?? deal.discountPct;
  const ad = deal.intelligence?.advertisedDiscountPct ?? deal.discountPct;

  return (
    <Card className="overflow-hidden">
      <img src={deal.image} alt="" className="aspect-[4/3] w-full object-cover bg-bfm-bg-2" loading="lazy" />
      <CardContent className="space-y-2">
        {score && (
          <p className="text-xs text-bfm-accent">
            {score.classificationLabel} · {score.score}/100 · {score.confidence}% conf
          </p>
        )}
        <h4 className="line-clamp-2 text-sm font-medium">
          <Link href={`/product/${encodeURIComponent(deal.id)}`} className="hover:text-bfm-accent">
            {deal.title}
          </Link>
        </h4>
        <p className="text-xs text-bfm-muted">
          {deal.retailer} · Steal {deal.stealScore}
        </p>
        <p className="text-xs">
          <span className="text-bfm-muted">Ad {ad}%</span>
          {" → "}
          <span className="text-bfm-accent-2">Real {real}% vs 30d</span>
        </p>
        <p className="text-lg font-semibold text-white">${deal.price.toFixed(2)}</p>
        <div className="flex flex-wrap gap-2">
          <Link
            href={`/product/${encodeURIComponent(deal.id)}`}
            className="text-sm text-bfm-accent-2 hover:underline"
          >
            Intel
          </Link>
          {deal.trust?.safeUrl && (
            <a
              href={deal.url}
              target="_blank"
              rel="noreferrer noopener"
              className="text-sm text-bfm-accent hover:underline"
            >
              Retailer
            </a>
          )}
          {onAddWishlist && (
            <Button type="button" variant="outline" className="min-h-8 px-2 py-1 text-xs" onClick={() => onAddWishlist(deal)}>
              {added ? "On wishlist" : "+ Wishlist"}
            </Button>
          )}
        </div>
      </CardContent>
    </Card>
  );
}
