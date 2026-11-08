"use server";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { loadNotificationPrefs } from "@/app/settings/actions";

export async function saveUserPushSubscription(input: {
  endpoint: string;
  p256dh: string;
  auth: string;
  minStealScore: number;
}) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const prefs = await loadNotificationPrefs();
  if (prefs.ok && (!prefs.prefs.push_enabled || !prefs.prefs.deal_alert_enabled)) {
    return { ok: false as const, error: "notifications_disabled_in_settings" };
  }

  const supabase = await createClient();
  const { data: auth } = await supabase!.auth.getUser();
  if (!auth.user) return { ok: false as const, error: "not_signed_in" };

  const min = Math.min(100, Math.max(50, Math.round(input.minStealScore)));
  const { error } = await supabase!.from("user_push_subscriptions").upsert(
    {
      user_id: auth.user.id,
      endpoint: input.endpoint.slice(0, 512),
      p256dh: input.p256dh.slice(0, 256),
      auth: input.auth.slice(0, 128),
      min_steal_score: min,
      active: true,
      updated_at: new Date().toISOString(),
    },
    { onConflict: "user_id,endpoint" },
  );

  if (error) return { ok: false as const, error: error.message };
  return { ok: true as const };
}
