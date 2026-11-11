import { liveEnrichedDeals } from "../catalog-pool.js";
import { db } from "../db.js";
import { notifyDealAlert, type PushSubscriptionInput } from "./notify.js";
import { listSupabasePushTargets } from "./supabase-push.js";

export interface StealHit {
  dealId: string;
  title: string;
  stealScore: number;
  price: number;
}

export function listSubscriptionsWithThreshold(): (PushSubscriptionInput & { id: string; minStealScore: number })[] {
  const rows = db
    .prepare(
      `SELECT id, endpoint, p256dh, auth, email, notify_email as notifyEmail, min_steal_score as minStealScore
       FROM push_subscriptions`,
    )
    .all() as {
    id: string;
    endpoint: string;
    p256dh: string;
    auth: string;
    email: string | null;
    notifyEmail: number;
    minStealScore: number;
  }[];

  return rows.map((r) => ({
    id: r.id,
    endpoint: r.endpoint,
    keys: { p256dh: r.p256dh, auth: r.auth },
    email: r.email ?? undefined,
    notifyEmail: r.notifyEmail === 1,
    minStealScore: r.minStealScore ?? 85,
  }));
}

export function findStealHits(minStealScore: number): StealHit[] {
  const { deals } = liveEnrichedDeals();
  return deals
    .filter((d) => d.stealScore >= minStealScore && d.trust.safeUrl)
    .sort((a, b) => b.stealScore - a.stealScore)
    .slice(0, 12)
    .map((d) => ({
      dealId: d.id,
      title: d.title,
      stealScore: d.stealScore,
      price: d.pricing?.finalPrice ?? d.price,
    }));
}

type SubRow = PushSubscriptionInput & { id: string; minStealScore: number };

async function allPushTargets(): Promise<SubRow[]> {
  const local = listSubscriptionsWithThreshold();
  const remote = await listSupabasePushTargets();
  const seen = new Set<string>();
  const merged: SubRow[] = [];

  for (const s of local) {
    if (seen.has(s.endpoint)) continue;
    seen.add(s.endpoint);
    merged.push(s);
  }
  for (const s of remote) {
    if (seen.has(s.endpoint)) continue;
    seen.add(s.endpoint);
    merged.push({
      id: `supabase:${s.userId}:${s.endpoint.slice(-12)}`,
      endpoint: s.endpoint,
      keys: s.keys,
      email: undefined,
      notifyEmail: false,
      minStealScore: s.minStealScore,
    });
  }
  return merged;
}

export async function dispatchStealAlerts(): Promise<{
  subscriptions: number;
  notifications: number;
  hits: StealHit[];
}> {
  const subs = await allPushTargets();
  if (subs.length === 0) {
    return { subscriptions: 0, notifications: 0, hits: [] };
  }

  const maxMin = Math.max(...subs.map((s) => s.minStealScore));
  const hits = findStealHits(maxMin);
  let notifications = 0;

  for (const sub of subs) {
    const subHits = hits.filter((h) => h.stealScore >= sub.minStealScore);
    const top = subHits[0];
    if (!top) continue;
    const dedupeKey = `${sub.id}:${top.dealId}:${top.price}`;
    const seen = db
      .prepare(`SELECT 1 FROM steal_alert_log WHERE id = ?`)
      .get(dedupeKey) as { 1: number } | undefined;
    if (seen) continue;

    const body = `${top.title} — steal ${top.stealScore} at $${top.price.toFixed(2)}`;
    await notifyDealAlert({
      title: "Black Friday Madness — steal alert",
      body,
      dealId: top.dealId,
      email: sub.notifyEmail ? sub.email : undefined,
      pushSubscription: { endpoint: sub.endpoint, keys: sub.keys, email: sub.email },
    });
    db.prepare(`INSERT INTO steal_alert_log (id, created_at) VALUES (?, ?)`).run(
      dedupeKey,
      new Date().toISOString(),
    );
    notifications++;
  }

  return { subscriptions: subs.length, notifications, hits };
}
