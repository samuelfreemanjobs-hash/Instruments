"use client";

import { useEffect, useState } from "react";

import { fetchRetailCalendar } from "@/lib/deal-api";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { EmptyState } from "@/components/ui/empty-state";

export function CalendarClient() {
  const [payload, setPayload] = useState<Awaited<ReturnType<typeof fetchRetailCalendar>> | null>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    void fetchRetailCalendar()
      .then(setPayload)
      .catch(() => setPayload(null))
      .finally(() => setLoading(false));
  }, []);

  if (loading) return <Skeleton className="h-48 w-full" />;
  if (!payload) return <EmptyState title="Calendar offline" description="Start the deal API." />;

  return (
    <div className="space-y-4">
      <Card className="border-bfm-accent/30">
        <CardContent className="space-y-1 pt-4">
          <p className="text-sm font-medium">{payload.current.label}</p>
          <p className="text-xs text-bfm-muted">{payload.current.focus}</p>
        </CardContent>
      </Card>
      <ol className="space-y-2">
        {payload.events.map((e) => (
          <li key={e.id} className="rounded-lg border border-bfm-border px-3 py-2 text-sm">
            <p className="font-medium">
              {e.label}{" "}
              <span className="text-xs font-normal text-bfm-muted">
                {new Date(e.startsAt).toLocaleDateString(undefined, { month: "short", day: "numeric", year: "numeric" })}
              </span>
            </p>
            <p className="text-xs text-bfm-muted">{e.description}</p>
          </li>
        ))}
      </ol>
    </div>
  );
}
