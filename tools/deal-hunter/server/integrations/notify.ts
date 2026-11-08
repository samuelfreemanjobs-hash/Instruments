import { customAlphabet } from "nanoid";
import { db } from "../db.js";
import { emitN8nEvent } from "./n8n.js";

const subId = customAlphabet("23456789abcdef", 16);

export interface PushSubscriptionInput {
  endpoint: string;
  keys: { p256dh: string; auth: string };
  email?: string;
  notifyEmail?: boolean;
  minStealScore?: number;
}

export function savePushSubscription(input: PushSubscriptionInput): string {
  const id = subId();
  const now = new Date().toISOString();
  const minSteal = Math.min(100, Math.max(50, Math.round(input.minStealScore ?? 85)));
  db.prepare(
    `INSERT OR REPLACE INTO push_subscriptions (id, endpoint, p256dh, auth, email, notify_email, min_steal_score, created_at)
     VALUES (?, ?, ?, ?, ?, ?, ?, ?)`,
  ).run(
    id,
    input.endpoint,
    input.keys.p256dh,
    input.keys.auth,
    input.email?.slice(0, 120) ?? null,
    input.notifyEmail ? 1 : 0,
    minSteal,
    now,
  );
  return id;
}

export function updatePushSubscriptionPrefs(
  endpoint: string,
  prefs: { minStealScore?: number; notifyEmail?: boolean },
): boolean {
  const row = db.prepare(`SELECT id FROM push_subscriptions WHERE endpoint = ?`).get(endpoint) as
    | { id: string }
    | undefined;
  if (!row) return false;
  const minSteal =
    prefs.minStealScore != null
      ? Math.min(100, Math.max(50, Math.round(prefs.minStealScore)))
      : undefined;
  if (minSteal != null && prefs.notifyEmail != null) {
    db.prepare(`UPDATE push_subscriptions SET min_steal_score = ?, notify_email = ? WHERE id = ?`).run(
      minSteal,
      prefs.notifyEmail ? 1 : 0,
      row.id,
    );
  } else if (minSteal != null) {
    db.prepare(`UPDATE push_subscriptions SET min_steal_score = ? WHERE id = ?`).run(minSteal, row.id);
  } else if (prefs.notifyEmail != null) {
    db.prepare(`UPDATE push_subscriptions SET notify_email = ? WHERE id = ?`).run(
      prefs.notifyEmail ? 1 : 0,
      row.id,
    );
  }
  return true;
}

export function listPushSubscriptions(): PushSubscriptionInput[] {
  const rows = db
    .prepare(`SELECT endpoint, p256dh, auth, email, notify_email as notifyEmail FROM push_subscriptions`)
    .all() as {
    endpoint: string;
    p256dh: string;
    auth: string;
    email: string | null;
    notifyEmail: number;
  }[];
  return rows.map((r) => ({
    endpoint: r.endpoint,
    keys: { p256dh: r.p256dh, auth: r.auth },
    email: r.email ?? undefined,
    notifyEmail: r.notifyEmail === 1,
  }));
}

export async function sendDealAlertEmail(payload: {
  email: string;
  subject: string;
  body: string;
  dealIds?: string[];
}) {
  return emitN8nEvent("deal_alert", {
    channel: "email",
    to: payload.email,
    subject: payload.subject,
    body: payload.body,
    dealIds: payload.dealIds,
  });
}

/** Web Push send requires VAPID keys + web-push lib in production; queue via n8n for now */
export async function notifyDealAlert(payload: {
  title: string;
  body: string;
  dealId: string;
  email?: string;
}) {
  const results: { channel: string; ok: boolean }[] = [];

  if (payload.email) {
    const mail = await sendDealAlertEmail({
      email: payload.email,
      subject: payload.title,
      body: payload.body,
      dealIds: [payload.dealId],
    });
    results.push({ channel: "email_via_n8n", ok: mail.sent });
  }

  const subs = listPushSubscriptions();
  if (subs.length > 0) {
    const push = await emitN8nEvent("deal_alert", {
      channel: "web_push",
      title: payload.title,
      body: payload.body,
      dealId: payload.dealId,
      subscriptionCount: subs.length,
    });
    results.push({ channel: "web_push_via_n8n", ok: push.sent });
  }

  return { results, storedSubscriptions: subs.length };
}
