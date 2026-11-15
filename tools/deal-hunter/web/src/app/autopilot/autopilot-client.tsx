"use client";

import { useEffect, useState } from "react";
import Link from "next/link";
import { fetchAutopilotAgents, fetchAutopilotDigest } from "@/lib/deal-api";
import { Button } from "@/components/ui/button";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { EmptyState } from "@/components/ui/empty-state";

function productDetailHref(pick: { id: string; label: string }): string | null {
  if (pick.label === "Online + pickup") return null;
  if (!pick.id || pick.id.length < 4) return null;
  return `/product/${encodeURIComponent(pick.id)}`;
}

export function AutopilotClient() {
  const [agents, setAgents] = useState<{ id: string; name: string; mission: string; scheduleHint: string }[]>([]);
  const [digest, setDigest] = useState<Awaited<ReturnType<typeof fetchAutopilotDigest>> | null>(null);
  const [loading, setLoading] = useState(false);
  const [boot, setBoot] = useState(true);

  const run = (zip = "78701", maxBudget = 400) => {
    setLoading(true);
    void fetchAutopilotDigest({ zip, maxBudget })
      .then(setDigest)
      .catch(() => setDigest(null))
      .finally(() => {
        setLoading(false);
        setBoot(false);
      });
  };

  useEffect(() => {
    void fetchAutopilotAgents().then((r) => setAgents(r.agents));
    run();
    const id = setInterval(() => run(), 180_000);
    return () => clearInterval(id);
  }, []);

  return (
    <div className="space-y-4">
      <Button type="button" disabled={loading} onClick={() => run()}>
        {loading ? "Running agents…" : "Run all agents"}
      </Button>

      <ul className="grid gap-2 sm:grid-cols-2">
        {agents.map((a) => (
          <li key={a.id} className="rounded-lg border border-bfm-border bg-bfm-card p-3 text-sm">
            <p className="font-medium">{a.name}</p>
            <p className="text-bfm-muted">{a.mission}</p>
            <p className="text-xs text-bfm-accent-2">{a.scheduleHint}</p>
          </li>
        ))}
      </ul>

      {boot && loading && <Skeleton className="h-32 w-full" />}
      {!loading && !digest && <EmptyState title="Autopilot offline" description="Check deal API." />}

      {digest && (
        <>
          <p className="text-xs text-bfm-muted">
            Run {digest.runId} · {new Date(digest.generatedAt).toLocaleTimeString()}
          </p>
          {digest.agents.map((run) => (
            <Card key={run.agentId}>
              <CardContent className="space-y-2 pt-4">
                <h3 className="font-medium">{run.title}</h3>
                <p className="text-sm text-bfm-muted">{run.narrative}</p>
                <ol className="list-decimal space-y-1 pl-5 text-sm">
                  {run.picks.map((p) => (
                    <li key={`${p.id}-${p.label}`}>
                      {productDetailHref(p) ? (
                        <Link href={productDetailHref(p)!} className="text-bfm-accent hover:underline">
                          {p.label}
                        </Link>
                      ) : (
                        p.label
                      )}
                      <span className="block text-xs text-bfm-muted">{p.detail}</span>
                    </li>
                  ))}
                </ol>
              </CardContent>
            </Card>
          ))}
        </>
      )}
    </div>
  );
}
