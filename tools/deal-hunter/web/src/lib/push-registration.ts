"use client";

import { registerStealAlertBackend } from "@/lib/deal-api";

function urlBase64ToUint8Array(base64: string) {
  const padding = "=".repeat((4 - (base64.length % 4)) % 4);
  const b64 = (base64 + padding).replace(/-/g, "+").replace(/_/g, "/");
  const raw = atob(b64);
  const out = new Uint8Array(raw.length);
  for (let i = 0; i < raw.length; ++i) out[i] = raw.charCodeAt(i);
  return out;
}

export async function registerRealWebPush(minStealScore: number): Promise<string | null> {
  const vapid = process.env.NEXT_PUBLIC_VAPID_PUBLIC_KEY?.trim();
  if (!vapid || !("serviceWorker" in navigator) || !("PushManager" in window)) return null;

  const keyRes = await fetch("/api/notifications/vapid-public-key");
  const keyJson = (await keyRes.json()) as { configured: boolean; publicKey: string | null };
  if (!keyJson.configured || !keyJson.publicKey) return null;

  const reg = await navigator.serviceWorker.register("/sw.js");
  await navigator.serviceWorker.ready;

  const existing = await reg.pushManager.getSubscription();
  const sub =
    existing ??
    (await reg.pushManager.subscribe({
      userVisibleOnly: true,
      applicationServerKey: urlBase64ToUint8Array(keyJson.publicKey),
    }));

  const json = sub.toJSON();
  if (!json.keys?.p256dh || !json.keys?.auth) return null;

  await registerStealAlertBackend({
    endpoint: sub.endpoint,
    minStealScore,
    keys: { p256dh: json.keys.p256dh, auth: json.keys.auth },
  });

  return sub.endpoint;
}
