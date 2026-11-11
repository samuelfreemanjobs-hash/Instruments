"use client";

import { saveUserPushSubscription } from "@/app/alerts/push-actions";
import { fetchStealHits, registerStealAlertBackend } from "@/lib/deal-api";
import { registerRealWebPush } from "@/lib/push-registration";

const DEDUPE_KEY = "bfm-steal-notified";

function loadDedupe(): Set<string> {
  try {
    const raw = sessionStorage.getItem(DEDUPE_KEY);
    if (!raw) return new Set();
    return new Set(JSON.parse(raw) as string[]);
  } catch {
    return new Set();
  }
}

function saveDedupe(set: Set<string>) {
  sessionStorage.setItem(DEDUPE_KEY, JSON.stringify([...set].slice(-80)));
}

export async function enableStealAlerts(
  minStealScore: number,
  email?: string,
  endpoint?: string,
): Promise<string> {
  if (!endpoint) {
    const real = await registerRealWebPush(minStealScore).catch(() => null);
    if (real) {
      const json = await fetchSubscriptionKeys(real);
      if (json) {
        await saveUserPushSubscription({
          endpoint: real,
          p256dh: json.p256dh,
          auth: json.auth,
          minStealScore,
        }).catch(() => undefined);
      }
      await registerStealAlertBackend({
        endpoint: real,
        minStealScore,
        email,
        keys: json ?? { p256dh: "browser-local", auth: "browser-local" },
      });
      return real;
    }
  }
  const ep =
    endpoint ??
    `browser-tab:${typeof crypto !== "undefined" && "randomUUID" in crypto ? crypto.randomUUID() : Date.now()}`;
  await registerStealAlertBackend({
    endpoint: ep,
    minStealScore,
    email,
    keys: { p256dh: "browser-local", auth: "browser-local" },
  });
  return ep;
}

async function fetchSubscriptionKeys(endpoint: string): Promise<{ p256dh: string; auth: string } | null> {
  if (!("serviceWorker" in navigator)) return null;
  const reg = await navigator.serviceWorker.ready;
  const sub = await reg.pushManager.getSubscription();
  if (!sub || sub.endpoint !== endpoint) return null;
  const json = sub.toJSON();
  if (!json.keys?.p256dh || !json.keys?.auth) return null;
  return { p256dh: json.keys.p256dh, auth: json.keys.auth };
}

export function startStealAlertPolling(minStealScore: number, intervalMs = 90_000): () => void {
  const tick = async () => {
    if (!("Notification" in window) || Notification.permission !== "granted") return;
    try {
      const { hits } = await fetchStealHits(minStealScore);
      const dedupe = loadDedupe();
      for (const h of hits) {
        const key = `${h.dealId}:${h.stealScore}`;
        if (dedupe.has(key)) continue;
        new Notification("Black Friday Madness — steal", {
          body: `${h.title} · steal ${h.stealScore} · $${h.price.toFixed(2)}`,
          tag: h.dealId,
        });
        dedupe.add(key);
      }
      saveDedupe(dedupe);
    } catch {
      /* API offline */
    }
  };

  void tick();
  const id = setInterval(() => void tick(), intervalMs);
  return () => clearInterval(id);
}
