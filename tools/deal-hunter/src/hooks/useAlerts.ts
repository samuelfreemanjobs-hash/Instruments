import { useCallback, useEffect, useRef } from "react";
import type { Deal } from "../types";
import {
  loadAlertSettings,
  loadTargetWatches,
  loadWatchlist,
  saveTargetWatches,
  saveWatchlist,
} from "../lib/storage";
import { evaluateTargetWatches } from "../lib/api";

export function useDealAlerts(deals: Deal[]) {
  const notifiedRef = useRef(new Set<string>());

  const checkAndNotify = useCallback(async () => {
    if (!("Notification" in window)) return;
    if (Notification.permission !== "granted") return;

    const settings = loadAlertSettings();
    const watchlist = loadWatchlist();

    for (const deal of deals) {
      const key = `${deal.id}-${deal.price}`;
      if (notifiedRef.current.has(key)) continue;

      const watched = watchlist.find((w) => w.dealId === deal.id);
      const stealHit = settings.notifySteals && deal.stealScore >= settings.minStealScore;
      const dropHit =
        settings.notifyPriceDrops &&
        watched &&
        deal.priceDropped &&
        deal.price < watched.lastKnownPrice;

      const targetHit = watched?.targetPrice != null && deal.price <= watched.targetPrice;

      if (stealHit || dropHit || targetHit) {
        const parts: string[] = [];
        if (stealHit) parts.push(`Steal score ${deal.stealScore}`);
        if (dropHit) parts.push(`Price dropped to $${deal.price.toFixed(2)}`);
        if (targetHit) parts.push(`At your target $${watched!.targetPrice!.toFixed(2)}`);

        new Notification("Deal Hunter — too good to skip", {
          body: `${deal.title}: ${parts.join(" · ")}`,
          icon: "/pwa-192.png",
          tag: deal.id,
        });
        notifiedRef.current.add(key);

        if (watched) {
          const next = watchlist.map((w) =>
            w.dealId === deal.id ? { ...w, lastKnownPrice: deal.price } : w,
          );
          saveWatchlist(next);
        }
      }
    }
  }, [deals]);

  useEffect(() => {
    void checkAndNotify();
    const watches = loadTargetWatches();
    if (watches.length === 0) return;
    void evaluateTargetWatches(
      watches.map((w) => ({ id: w.id, productQuery: w.productQuery, targetPrice: w.targetPrice })),
    ).then((res) => {
      if (!("Notification" in window) || Notification.permission !== "granted") return;
      for (const r of res.results) {
        if (!r.targetReached) continue;
        const w = watches.find((x) => x.id === r.watchId);
        if (w?.notifiedAt) continue;
        const offer = r.participatingOffers.find((o) => o.atTarget);
        new Notification("Target price hit!", {
          body: `${r.productQuery} is $${offer?.price.toFixed(2) ?? r.targetPrice} at ${offer?.retailer ?? "a store"}`,
          icon: "/pwa-192.png",
        });
        const updated = watches.map((x) =>
          x.id === r.watchId ? { ...x, notifiedAt: new Date().toISOString() } : x,
        );
        saveTargetWatches(updated);
      }
    });
  }, [checkAndNotify, deals]);

  return {
    requestPermission: async () => {
      if (!("Notification" in window)) return "unsupported" as const;
      return Notification.requestPermission();
    },
  };
}
