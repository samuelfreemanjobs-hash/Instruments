"use client";

import { useEffect, useState } from "react";
import { Button } from "@/components/ui/button";
import { Card, CardContent } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { triggerCatalogSync, triggerStealDispatch, fetchAdminStatus } from "./actions";

export function AdminDashboardClient() {
  const [status, setStatus] = useState<Awaited<ReturnType<typeof fetchAdminStatus>> | null>(null);
  const [msg, setMsg] = useState<string | null>(null);
  const [loading, setLoading] = useState(true);

  const refresh = () => {
    setLoading(true);
    void fetchAdminStatus()
      .then(setStatus)
      .catch(() => setStatus({ ok: false, error: "status_failed" }))
      .finally(() => setLoading(false));
  };

  useEffect(() => {
    refresh();
  }, []);

  if (loading) return <Skeleton className="h-40 w-full" />;
  if (!status?.ok) {
    return <p className="text-sm text-bfm-danger">{status?.error ?? "Set BFM_ADMIN_SECRET + DEAL_API_ORIGIN on Vercel."}</p>;
  }

  return (
    <div className="space-y-4">
      <Card>
        <CardContent className="space-y-2 pt-4 text-sm">
          <p>
            Catalog: <strong>{status.data.catalogSource}</strong> · VAPID {status.data.vapidConfigured ? "on" : "off"}
          </p>
          <p className="text-xs text-bfm-muted">{status.data.inngestNote}</p>
        </CardContent>
      </Card>

      <div className="flex flex-wrap gap-2">
        <Button
          type="button"
          onClick={() =>
            void triggerCatalogSync().then((r) => setMsg(r.ok ? "Sync triggered" : r.error ?? "Failed"))
          }
        >
          Sync catalog → Supabase
        </Button>
        <Button
          type="button"
          variant="outline"
          onClick={() =>
            void triggerStealDispatch().then((r) => setMsg(r.ok ? "Steal dispatch triggered" : r.error ?? "Failed"))
          }
        >
          Dispatch steal alerts
        </Button>
        <Button type="button" variant="ghost" onClick={refresh}>
          Refresh status
        </Button>
      </div>
      {msg && <p className="text-xs text-bfm-muted">{msg}</p>}

      <ul className="space-y-2 text-sm">
        {status.data.integrations.map((i) => (
          <li key={i.retailerSlug} className="rounded-lg border border-bfm-border px-3 py-2">
            <span className="font-medium">{i.retailerSlug}</span>{" "}
            <span className={i.health === "healthy" ? "text-bfm-success" : "text-bfm-warning"}>{i.health}</span>
            <span className="block text-xs text-bfm-muted">fresh {i.dataFreshnessMinutes}m · sync {i.lastSyncAt}</span>
          </li>
        ))}
      </ul>
    </div>
  );
}
