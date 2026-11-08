"use client";

import { useEffect, useState } from "react";
import Link from "next/link";

import { fetchRecommendations } from "@/lib/deal-api";
import { saveShoppingBudget, type BudgetSnapshot } from "./actions";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Card, CardContent } from "@/components/ui/card";
import { EmptyState } from "@/components/ui/empty-state";

const LS_KEY = "bfm-budget";

export function BudgetClient({ serverBudget }: { serverBudget: BudgetSnapshot | null }) {
  const [total, setTotal] = useState(serverBudget?.budgetTotal ?? 750);
  const [categoryCap, setCategoryCap] = useState(200);
  const [category, setCategory] = useState("Kitchen");
  const [caps, setCaps] = useState<Record<string, number>>(serverBudget?.budgetByCategory ?? {});
  const [recs, setRecs] = useState<Awaited<ReturnType<typeof fetchRecommendations>>["recommendations"]>([]);
  const [msg, setMsg] = useState<string | null>(null);

  useEffect(() => {
    if (serverBudget) return;
    try {
      const raw = localStorage.getItem(LS_KEY);
      if (raw) {
        const parsed = JSON.parse(raw) as BudgetSnapshot;
        setTotal(parsed.budgetTotal ?? 750);
        setCaps(parsed.budgetByCategory ?? {});
      }
    } catch {
      /* ignore */
    }
  }, [serverBudget]);

  useEffect(() => {
    void fetchRecommendations({ maxBudget: total, categories: [category] })
      .then((r) => setRecs(r.recommendations))
      .catch(() => setRecs([]));
  }, [total, category]);

  const saveLocal = () => {
    localStorage.setItem(LS_KEY, JSON.stringify({ budgetTotal: total, budgetByCategory: caps }));
  };

  const addCap = () => {
    if (!category.trim()) return;
    setCaps((c) => ({ ...c, [category]: categoryCap }));
  };

  const persist = async () => {
    saveLocal();
    const r = await saveShoppingBudget(total, caps);
    setMsg(r.ok ? "Budget saved to Supabase" : r.error ?? "Saved locally only — sign in to sync");
  };

  const capSum = Object.values(caps).reduce((a, b) => a + b, 0);

  return (
    <div className="space-y-4">
      <Card>
        <CardContent className="space-y-3 pt-4">
          <label className="block text-sm">
            Overall budget ($)
            <Input type="number" className="mt-1" value={total} onChange={(e) => setTotal(Number(e.target.value))} />
          </label>
          <p className="text-xs text-bfm-muted">
            Category caps: ${capSum.toFixed(0)} allocated · planner does not mark purchases complete automatically.
          </p>
          <div className="flex flex-wrap gap-2">
            <Input
              className="w-32"
              value={category}
              onChange={(e) => setCategory(e.target.value)}
              placeholder="Category"
            />
            <Input
              type="number"
              className="w-28"
              value={categoryCap}
              onChange={(e) => setCategoryCap(Number(e.target.value))}
            />
            <Button type="button" variant="outline" onClick={addCap}>
              Add cap
            </Button>
            <Button type="button" onClick={() => void persist()}>
              Save budget
            </Button>
          </div>
          {Object.keys(caps).length > 0 && (
            <ul className="text-xs text-bfm-muted">
              {Object.entries(caps).map(([k, v]) => (
                <li key={k}>
                  {k}: ${v.toFixed(0)}
                </li>
              ))}
            </ul>
          )}
          {msg && <p className="text-xs text-bfm-muted">{msg}</p>}
        </CardContent>
      </Card>

      <section>
        <h3 className="mb-2 text-sm font-medium">Picks under ${total.toFixed(0)}</h3>
        {recs.length === 0 ? (
          <EmptyState title="No picks" description="Raise budget or refresh when the deal API is online." />
        ) : (
          <ul className="space-y-2 text-sm">
            {recs.map((r) => (
              <li key={r.dealId} className="rounded-lg border border-bfm-border px-3 py-2">
                <Link href={`/product/${r.dealId}`} className="text-bfm-accent hover:underline">
                  {r.title}
                </Link>
                <span className="ml-2 text-bfm-muted">
                  ${r.price.toFixed(2)} · {r.reason}
                </span>
              </li>
            ))}
          </ul>
        )}
      </section>
    </div>
  );
}
