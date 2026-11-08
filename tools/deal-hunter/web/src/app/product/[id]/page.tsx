import Link from "next/link";
import { notFound } from "next/navigation";

import { PriceHistoryChart } from "@/components/charts/PriceHistoryChart";
import { ProductActions } from "./product-actions";
import { fetchProductDetail } from "@/lib/server-deal-api";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";

type Props = { params: Promise<{ id: string }> };

export default async function ProductDetailPage({ params }: Props) {
  const { id } = await params;
  let payload;
  try {
    payload = await fetchProductDetail(id);
  } catch {
    return (
      <p className="text-sm text-bfm-danger">Deal API unreachable — run npm run dev:server.</p>
    );
  }
  if (!payload) notFound();

  const { deal, intelligence, dealIntelligenceScore, priceHistory } = payload;
  const pay = deal.pricing?.finalPrice ?? deal.price;

  return (
    <div className="space-y-6">
      <Link href="/search" className="text-sm text-bfm-accent hover:underline">
        ← Back to search
      </Link>

      <div className="grid gap-6 lg:grid-cols-2">
        <img src={deal.image} alt="" className="w-full rounded-xl border border-bfm-border object-cover" />
        <div className="space-y-4">
          <h2 className="text-xl font-semibold">{deal.title}</h2>
          <p className="text-sm text-bfm-muted">
            {deal.retailer} · {deal.category} · {deal.itemCondition}
          </p>

          <div className="rounded-lg border border-bfm-accent/40 bg-bfm-bg-2 p-3">
            <p className="text-2xl font-bold text-bfm-accent">
              {dealIntelligenceScore.score}/100 — {dealIntelligenceScore.classificationLabel}
            </p>
            <p className="text-xs text-bfm-muted">{dealIntelligenceScore.confidence}% confidence · {dealIntelligenceScore.explanation}</p>
          </div>

          <p className="text-3xl font-bold">${pay.toFixed(2)}</p>
          <p className="text-sm">
            <span className="text-bfm-muted">Advertised {intelligence.advertisedDiscountPct}%</span>
            {" · "}
            <span className="text-bfm-accent-2">Real {intelligence.realSavingsVsAvgPct}% vs 30d avg</span>
          </p>
          <p className="text-sm text-bfm-muted">{intelligence.verdictHeadline}</p>
          {intelligence.bestValueVerdict && (
            <p className="text-sm text-white">{intelligence.bestValueVerdict}</p>
          )}

          {deal.returnPolicy && (
            <p className="text-xs text-bfm-muted">Returns: {deal.returnPolicy.summary}</p>
          )}

          <ProductActions
            dealId={deal.id}
            title={deal.title}
            price={pay}
            safeUrl={deal.trust.safeUrl}
            storeUrl={deal.url}
          />
        </div>
      </div>

      <Card>
        <CardHeader>
          <CardTitle className="text-base">30-day price history</CardTitle>
        </CardHeader>
        <CardContent>
          <p className="mb-2 text-xs text-bfm-muted">
            Avg ${intelligence.priceAnalysis.average30d.toFixed(2)} · Low ${intelligence.priceAnalysis.lowest30d.toFixed(2)}
          </p>
          <PriceHistoryChart rows={priceHistory} />
        </CardContent>
      </Card>

      <Card>
        <CardHeader>
          <CardTitle className="text-base">Score breakdown</CardTitle>
        </CardHeader>
        <CardContent className="space-y-2 text-sm">
          {dealIntelligenceScore.components.map((c) => (
            <div key={c.label} className="flex justify-between gap-2 border-b border-bfm-border/50 py-1">
              <span>
                {c.label} ({c.earnedPoints}/{c.maxPoints})
              </span>
              <span className="text-right text-xs text-bfm-muted">{c.evidence}</span>
            </div>
          ))}
        </CardContent>
      </Card>
    </div>
  );
}
