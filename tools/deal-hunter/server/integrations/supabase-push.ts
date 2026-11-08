import type { PushSubscriptionInput } from "./notify.js";
import { createSupabaseAdmin } from "./supabase-sync.js";

export interface SupabasePushTarget extends PushSubscriptionInput {
  minStealScore: number;
  userId: string;
}

export async function listSupabasePushTargets(): Promise<SupabasePushTarget[]> {
  const admin = createSupabaseAdmin();
  if (!admin) return [];

  const { data: subs, error } = await admin
    .from("user_push_subscriptions")
    .select("user_id, endpoint, p256dh, auth, min_steal_score")
    .eq("active", true);
  if (error || !subs?.length) return [];

  const userIds = [...new Set(subs.map((s) => s.user_id as string))];
  const { data: prefs } = await admin
    .from("notification_preferences")
    .select("user_id, push_enabled, deal_alert_enabled")
    .in("user_id", userIds);

  const prefMap = new Map(
    (prefs ?? []).map((p) => [
      p.user_id as string,
      { push: p.push_enabled !== false, deal: p.deal_alert_enabled !== false },
    ]),
  );

  const out: SupabasePushTarget[] = [];
  for (const row of subs) {
    const uid = row.user_id as string;
    const pref = prefMap.get(uid) ?? { push: true, deal: true };
    if (!pref.push || !pref.deal) continue;
    if (row.endpoint.startsWith("browser-tab:")) continue;
    out.push({
      userId: uid,
      endpoint: row.endpoint as string,
      keys: { p256dh: row.p256dh as string, auth: row.auth as string },
      minStealScore: Number(row.min_steal_score ?? 85),
    });
  }
  return out;
}
