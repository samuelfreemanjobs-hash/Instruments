"use server";

import { revalidatePath } from "next/cache";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export type NotificationPrefs = {
  email_enabled: boolean;
  push_enabled: boolean;
  price_alert_enabled: boolean;
  coupon_alert_enabled: boolean;
  deal_alert_enabled: boolean;
};

export async function loadNotificationPrefs(): Promise<
  { ok: true; prefs: NotificationPrefs } | { ok: false; error: string }
> {
  if (!isSupabaseConfigured()) return { ok: false, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { data: auth } = await supabase!.auth.getUser();
  if (!auth.user) return { ok: false, error: "not_signed_in" };

  const { data, error } = await supabase!
    .from("notification_preferences")
    .select("email_enabled, push_enabled, price_alert_enabled, coupon_alert_enabled, deal_alert_enabled")
    .eq("user_id", auth.user.id)
    .maybeSingle();

  if (error) return { ok: false, error: error.message };
  const row = data as NotificationPrefs | null;
  return {
    ok: true,
    prefs: row ?? {
      email_enabled: true,
      push_enabled: true,
      price_alert_enabled: true,
      coupon_alert_enabled: true,
      deal_alert_enabled: true,
    },
  };
}

function sanitizePrefs(prefs: NotificationPrefs): NotificationPrefs {
  return {
    email_enabled: Boolean(prefs.email_enabled),
    push_enabled: Boolean(prefs.push_enabled),
    price_alert_enabled: Boolean(prefs.price_alert_enabled),
    coupon_alert_enabled: Boolean(prefs.coupon_alert_enabled),
    deal_alert_enabled: Boolean(prefs.deal_alert_enabled),
  };
}

export async function saveNotificationPrefs(prefs: NotificationPrefs) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { data: auth } = await supabase!.auth.getUser();
  if (!auth.user) return { ok: false as const, error: "not_signed_in" };

  const safe = sanitizePrefs(prefs);
  const { error } = await supabase!.from("notification_preferences").upsert(
    {
      ...safe,
      user_id: auth.user.id,
    },
    { onConflict: "user_id" },
  );

  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/settings");
  return { ok: true as const };
}
