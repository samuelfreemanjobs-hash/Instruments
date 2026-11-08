"use client";

import { useEffect, useState } from "react";
import Link from "next/link";
import { askIntelligence, fetchIntelligenceBriefing } from "@/lib/deal-api";
import { CategoryTrendChart } from "@/components/charts/CategoryTrendChart";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { EmptyState } from "@/components/ui/empty-state";

export function IntelClient() {
  const [briefing, setBriefing] = useState<Awaited<ReturnType<typeof fetchIntelligenceBriefing>> | null>(null);
  const [loading, setLoading] = useState(true);
  const [question, setQuestion] = useState("");
  const [answer, setAnswer] = useState<string | null>(null);
  const [asking, setAsking] = useState(false);

  const load = () => {
    setLoading(true);
    void fetchIntelligenceBriefing()
      .then(setBriefing)
      .catch(() => setBriefing(null))
      .finally(() => setLoading(false));
  };

  useEffect(() => {
    load();
    const id = setInterval(load, 120_000);
    return () => clearInterval(id);
  }, []);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!question.trim()) return;
    setAsking(true);
    try {
      const res = await askIntelligence(question);
      setAnswer(res.answer);
    } finally {
      setAsking(false);
    }
  };

  if (loading) return <Skeleton className="h-48 w-full" />;
  if (!briefing) return <EmptyState title="Intelligence offline" description="Start the deal API." />;

  return (
    <div className="space-y-6">
      <p className="text-xs text-bfm-muted">
        Through {new Date(briefing.dataFreshThrough).toLocaleTimeString()} · {briefing.priceDropsToday} drops · real avg{" "}
        {briefing.avgRealSavingsPct}% vs advertised {briefing.avgAdvertisedDiscountPct}%
      </p>

      <form className="flex gap-2" onSubmit={(e) => void submit(e)}>
        <Input
          value={question}
          onChange={(e) => setQuestion(e.target.value)}
          placeholder="Should I buy now or wait?"
          className="flex-1"
        />
        <Button type="submit" disabled={asking}>
          Analyze
        </Button>
      </form>
      {answer && <p className="text-sm text-bfm-muted">{answer}</p>}

      {briefing.categoryTrends && briefing.categoryTrends.length > 0 && (
        <CategoryTrendChart data={briefing.categoryTrends} />
      )}

      <div className="grid gap-4 sm:grid-cols-2">
        <Card>
          <CardContent className="pt-4">
            <h3 className="mb-2 text-sm font-medium text-bfm-success">Buy now</h3>
            <ul className="space-y-2 text-sm">
              {briefing.buyNow.map((b) => (
                <li key={b.dealId}>
                  <Link href={`/product/${b.dealId}`} className="text-bfm-accent hover:underline">
                    {b.title}
                  </Link>
                  <p className="text-xs text-bfm-muted">{b.verdictHeadline}</p>
                </li>
              ))}
            </ul>
          </CardContent>
        </Card>
        <Card>
          <CardContent className="pt-4">
            <h3 className="mb-2 text-sm font-medium text-bfm-warning">Wait</h3>
            <ul className="space-y-2 text-sm">
              {briefing.waitList.map((w) => (
                <li key={w.dealId}>
                  <Link href={`/product/${w.dealId}`} className="text-bfm-accent hover:underline">
                    {w.title}
                  </Link>
                </li>
              ))}
            </ul>
          </CardContent>
        </Card>
      </div>

      <ul className="list-disc space-y-1 pl-5 text-sm text-bfm-muted">
        {briefing.marketPulse.map((line) => (
          <li key={line}>{line}</li>
        ))}
      </ul>
    </div>
  );
}
