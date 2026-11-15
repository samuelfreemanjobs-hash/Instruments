"use client";

import { useState } from "react";
import { chatAssistant, chatAssistantLegacy } from "@/lib/deal-api";
import type { Deal } from "@/lib/types";
import { DealCard } from "@/components/deals/DealCard";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { addDealToWishlist } from "@/app/wishlist/actions";

const STARTERS = [
  "Is this a legitimate Black Friday deal?",
  "Find the best laptop under $500",
  "Compare these TVs for real savings",
  "Build a shopping list for a $750 budget",
];

export function AssistantClient() {
  const [input, setInput] = useState("");
  const [log, setLog] = useState<{ role: "user" | "assistant"; text: string }[]>([]);
  const [deals, setDeals] = useState<Deal[]>([]);
  const [loading, setLoading] = useState(false);

  const send = async (text: string) => {
    const msg = text.trim();
    if (!msg) return;
    setLoading(true);
    setLog((l) => [...l, { role: "user", text: msg }]);
    setInput("");
    try {
      const orch = await chatAssistant(msg);
      const legacy = await chatAssistantLegacy(msg).catch(() => null);
      const answer = orch.answer || legacy?.message || "No response.";
      setLog((l) => [...l, { role: "assistant", text: answer }]);
      if (legacy?.deals?.length) setDeals(legacy.deals);
      else {
        const list = await fetch("/api/v1/deals?").then((r) => r.json()) as { data: Deal[] };
        setDeals(list.data?.slice(0, 4) ?? []);
      }
    } catch {
      setLog((l) => [...l, { role: "assistant", text: "Assistant unavailable — start the deal API." }]);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-4">
      <div className="flex flex-wrap gap-2">
        {STARTERS.map((s) => (
          <Button key={s} type="button" variant="outline" className="text-xs" onClick={() => void send(s)}>
            {s}
          </Button>
        ))}
      </div>

      <form
        className="flex gap-2"
        onSubmit={(e) => {
          e.preventDefault();
          void send(input);
        }}
      >
        <Input
          value={input}
          onChange={(e) => setInput(e.target.value)}
          placeholder="Ask about deals, budgets, or verification…"
          className="flex-1"
        />
        <Button type="submit" disabled={loading}>
          Ask
        </Button>
      </form>

      <Card>
        <CardContent className="max-h-64 space-y-3 overflow-y-auto pt-4 text-sm">
          {log.length === 0 && <p className="text-bfm-muted">Orchestrator + legacy assistant with pricing evidence.</p>}
          {log.map((m, i) => (
            <p key={i} className={m.role === "user" ? "text-bfm-accent-2" : "text-bfm-muted"}>
              <strong>{m.role === "user" ? "You" : "BFM"}:</strong> {m.text}
            </p>
          ))}
          {loading && <Skeleton className="h-12 w-full" />}
        </CardContent>
      </Card>

      {deals.length > 0 && (
        <div className="grid gap-4 sm:grid-cols-2">
          {deals.map((d) => (
            <DealCard key={d.id} deal={d} onAddWishlist={(x) => void addDealToWishlist(x.id, x.title, x.price)} />
          ))}
        </div>
      )}
    </div>
  );
}
