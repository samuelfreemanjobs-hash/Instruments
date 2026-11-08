"use client";

import { fetchStealHits, registerStealAlertBackend } from "@/lib/deal-api";

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
  const ep =
    endpoint ??
    `browser-tab:${typeof crypto !== "undefined" && "randomUUID" in crypto ? crypto.randomUUID() : Date.now()}`;
  await registerStealAlertBackend({
    endpoint: ep,
    minStealScore,
    email,
  });
  return ep;
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
