"use client";

import { useState } from "react";
import Link from "next/link";

import { runPricingCompare } from "@/lib/deal-api";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";

export function CompareClient() {
  const [question, setQuestion] = useState("");
  const [loading, setLoading] = useState(false);
  const [verdict, setVerdict] = useState<Awaited<ReturnType<typeof runPricingCompare>> | null>(null);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!question.trim()) return;
    setLoading(true);
    try {
      setVerdict(await runPricingCompare(question.trim()));
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-4">
      <form className="flex flex-col gap-2 sm:flex-row" onSubmit={(e) => void submit(e)}>
        <Input
          value={question}
          onChange={(e) => setQuestion(e.target.value)}
          placeholder="Best Ultraboost deal? Sephora vs Ulta serum?"
          className="flex-1"
        />
        <Button type="submit" disabled={loading}>
          {loading ? "Comparing…" : "Find best deal"}
        </Button>
      </form>

      {loading && <Skeleton className="h-32 w-full" />}

      {verdict && (
        <Card>
          <CardContent className="space-y-2 pt-4 text-sm">
            <p>{verdict.summary}</p>
            {verdict.valueVerdict && <p className="text-bfm-accent-2">{verdict.valueVerdict}</p>}
            <p className="text-bfm-muted">{verdict.savingsBreakdown}</p>
            {verdict.bestValueScore > 0 && (
              <p>Value score {verdict.bestValueScore}/100 · confidence {verdict.confidence}</p>
            )}
            <p className="text-xs text-bfm-muted">{verdict.couponTip}</p>
            {verdict.bestPick && (
              <p>
                Best pick:{" "}
                <Link href={`/product/${verdict.bestPick.id}`} className="text-bfm-accent hover:underline">
                  {verdict.bestPick.title}
                </Link>{" "}
                (${(verdict.bestPick.pricing?.finalPrice ?? verdict.bestPick.price).toFixed(2)} at{" "}
                {verdict.bestPick.retailer})
              </p>
            )}
            {verdict.alternatives?.length > 0 && (
              <ul className="list-disc pl-5 text-xs text-bfm-muted">
                {verdict.alternatives.slice(0, 3).map((d) => (
                  <li key={d.id}>
                    <Link href={`/product/${d.id}`} className="text-bfm-accent">
                      {d.title}
                    </Link>
                  </li>
                ))}
              </ul>
            )}
          </CardContent>
        </Card>
      )}
    </div>
  );
}
