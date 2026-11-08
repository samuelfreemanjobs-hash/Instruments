"use client";

import { useEffect, useState } from "react";
import Link from "next/link";
import { deletePriceAlert, togglePriceAlert } from "./actions";
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

export function AlertsClient({ serverAlerts }: { serverAlerts: PriceAlertRow[] }) {
  const [minSteal, setMinSteal] = useState(85);
  const [perm, setPerm] = useState<string>("default");

  useEffect(() => {
    try {
      const raw = localStorage.getItem(LS_KEY);
      if (raw) setMinSteal((JSON.parse(raw) as { minSteal: number }).minSteal);
    } catch {
      /* ignore */
    }
    if ("Notification" in window) setPerm(Notification.permission);
  }, []);

  const saveLocal = () => {
    localStorage.setItem(LS_KEY, JSON.stringify({ minSteal }));
  };

  const requestNotify = async () => {
    if (!("Notification" in window)) return;
    const p = await Notification.requestPermission();
    setPerm(p);
  };

  return (
    <div className="space-y-6">
      <Card>
        <CardContent className="space-y-3 pt-4">
          <h3 className="text-sm font-medium">Browser notifications</h3>
          <p className="text-xs text-bfm-muted">Web push for steals ≥ threshold (local until push backend wired).</p>
          <label className="flex items-center gap-2 text-sm">
            Min steal score
            <Input
              type="number"
              className="w-24"
              value={minSteal}
              onChange={(e) => setMinSteal(Number(e.target.value))}
              onBlur={saveLocal}
            />
          </label>
          <Button type="button" variant="outline" onClick={() => void requestNotify()}>
            {perm === "granted" ? "Notifications enabled" : "Enable notifications"}
          </Button>
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
                  <Button
                    type="button"
                    variant="ghost"
                    onClick={() => void togglePriceAlert(a.id, !a.active)}
                  >
                    {a.active ? "Pause" : "Resume"}
                  </Button>
                  <Button type="button" variant="ghost" className="text-bfm-danger" onClick={() => void deletePriceAlert(a.id)}>
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
