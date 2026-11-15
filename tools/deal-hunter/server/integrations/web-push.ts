import webpush from "web-push";

import type { PushSubscriptionInput } from "./notify.js";

const mailto = () => process.env.VAPID_CONTACT_EMAIL ?? "mailto:alerts@bfm.local";

export function vapidConfigured(): boolean {
  return Boolean(process.env.VAPID_PUBLIC_KEY?.trim() && process.env.VAPID_PRIVATE_KEY?.trim());
}

export function vapidPublicKey(): string | null {
  return process.env.VAPID_PUBLIC_KEY?.trim() ?? null;
}

export function isRealPushEndpoint(endpoint: string): boolean {
  return !endpoint.startsWith("browser-tab:") && !endpoint.startsWith("browser-local");
}

export async function sendWebPush(
  sub: PushSubscriptionInput,
  payload: { title: string; body: string; dealId?: string },
): Promise<boolean> {
  if (!vapidConfigured() || !isRealPushEndpoint(sub.endpoint)) return false;
  if (sub.keys.p256dh === "browser-local" || sub.keys.auth === "browser-local") return false;

  try {
    webpush.setVapidDetails(mailto(), process.env.VAPID_PUBLIC_KEY!, process.env.VAPID_PRIVATE_KEY!);
    await webpush.sendNotification(
      {
        endpoint: sub.endpoint,
        keys: sub.keys,
      },
      JSON.stringify({
        title: payload.title,
        body: payload.body,
        url: payload.dealId ? `/product/${payload.dealId}` : "/alerts",
      }),
    );
    return true;
  } catch (err) {
    console.warn("web_push_failed", String(err));
    return false;
  }
}
