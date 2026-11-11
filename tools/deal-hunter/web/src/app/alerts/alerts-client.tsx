"use client";

import { useEffect, useRef, useState } from "react";
import Link from "next/link";
import { deletePriceAlert, togglePriceAlert } from "./actions";
import { enableStealAlerts, startStealAlertPolling } from "@/lib/steal-alert-poller";
import { patchStealAlertPrefs } from "@/lib/deal-api";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Card, CardContent } from "@/components/ui/card";
import { EmptyState } from "@/components/ui/empty-state";

export type PriceAlertRow = {
  id: string;
  external_deal_id: string | null;
  target_price: number;
  active: boolean;
  created_at: string;
};

const LS_KEY = "bfm-alert-prefs";
const ENDPOINT_KEY = "bfm-steal-endpoint";

export function AlertsClient({ serverAlerts }: { serverAlerts: PriceAlertRow[] }) {
  const [minSteal, setMinSteal] = useState(85);
  const [perm, setPerm] = useState<string>("default");
  const [status, setStatus] = useState<string | null>(null);
  const endpointRef = useRef<string | null>(null);

  useEffect(() => {
    try {
      const raw = localStorage.getItem(LS_KEY);
      if (raw) setMinSteal((JSON.parse(raw) as { minSteal: number }).minSteal);
      endpointRef.current = localStorage.getItem(ENDPOINT_KEY);
    } catch {
      /* ignore */
    }
    if ("Notification" in window) setPerm(Notification.permission);
  }, []);

  useEffect(() => {
    if (perm !== "granted") return;
    return startStealAlertPolling(minSteal);
  }, [perm, minSteal]);

  const persistPrefs = async (steal: number) => {
    localStorage.setItem(LS_KEY, JSON.stringify({ minSteal: steal }));
    const ep = endpointRef.current;
    if (ep) await patchStealAlertPrefs(ep, steal).catch(() => undefined);
  };

  const requestNotify = async () => {
    if (!("Notification" in window)) {
      setStatus("Notifications unsupported in this browser");
      return;
    }
    const p = await Notification.requestPermission();
    setPerm(p);
    if (p !== "granted") return;

    try {
      const tabId =
        typeof crypto !== "undefined" && "randomUUID" in crypto
          ? crypto.randomUUID()
          : String(Date.now());
      const endpoint = `browser-tab:${tabId}`;
      await enableStealAlerts(minSteal, undefined, endpoint);
      endpointRef.current = endpoint;
      localStorage.setItem(ENDPOINT_KEY, endpoint);
      setStatus(`Live steal scan ≥ ${minSteal} (browser + API dispatch via n8n)`);
    } catch {
      setStatus("Could not register with deal API — polling only while this tab is open");
    }
  };

  return (
    <div className="space-y-6">
      <Card>
        <CardContent className="space-y-3 pt-4">
          <h3 className="text-sm font-medium">Steal score alerts</h3>
          <p className="text-xs text-bfm-muted">
            Polls `/api/notifications/steal-hits` in this tab; cron `POST /api/notifications/dispatch-steals` relays via
            n8n for stored subscriptions.
          </p>
          <label className="flex items-center gap-2 text-sm">
            Min steal score
            <Input
              type="number"
              className="w-24"
              value={minSteal}
              onChange={(e) => setMinSteal(Number(e.target.value))}
              onBlur={() => void persistPrefs(minSteal)}
            />
          </label>
          <Button type="button" variant="outline" onClick={() => void requestNotify()}>
            {perm === "granted" ? "Steal alerts active" : "Enable steal notifications"}
          </Button>
          {status && <p className="text-xs text-bfm-muted">{status}</p>}
        </CardContent>
      </Card>

      <section>
        <h3 className="mb-2 text-sm font-medium">Supabase price watches</h3>
        {serverAlerts.length === 0 ? (
          <EmptyState
            title="No saved alerts"
            description="Set a target on a product page or sign in first."
            action={
              <Link href="/login" className="text-sm text-bfm-accent underline">
                Sign in
              </Link>
            }
          />
        ) : (
          <ul className="space-y-2 text-sm">
            {serverAlerts.map((a) => (
              <li key={a.id} className="flex flex-wrap items-center justify-between gap-2 rounded-lg border border-bfm-border px-3 py-2">
                <span>
                  {a.external_deal_id && (
                    <Link href={`/product/${a.external_deal_id}`} className="text-bfm-accent">
                      {a.external_deal_id}
                    </Link>
                  )}{" "}
                  target ${Number(a.target_price).toFixed(2)}
                </span>
                <div className="flex gap-2">
                  <Button type="button" variant="ghost" onClick={() => void togglePriceAlert(a.id, !a.active)}>
                    {a.active ? "Pause" : "Resume"}
                  </Button>
                  <Button
                    type="button"
                    variant="ghost"
                    className="text-bfm-danger"
                    onClick={() => void deletePriceAlert(a.id)}
                  >
                    Delete
                  </Button>
                </div>
              </li>
            ))}
          </ul>
        )}
      </section>
    </div>
  );
}
