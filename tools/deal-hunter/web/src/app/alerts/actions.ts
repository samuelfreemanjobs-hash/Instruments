"use server";

import { revalidatePath } from "next/cache";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function createPriceAlert(externalDealId: string, targetPrice: number) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { data: userData } = await supabase!.auth.getUser();
  if (!userData.user) return { ok: false as const, error: "not_signed_in" };

  const { error } = await supabase!.from("price_alerts").insert({
    user_id: userData.user.id,
    external_deal_id: externalDealId,
    target_price: targetPrice,
    active: true,
  });

  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/alerts");
  return { ok: true as const };
}

export async function togglePriceAlert(alertId: string, active: boolean) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { error } = await supabase!.from("price_alerts").update({ active }).eq("id", alertId);
  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/alerts");
  return { ok: true as const };
}

export async function deletePriceAlert(alertId: string) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { error } = await supabase!.from("price_alerts").delete().eq("id", alertId);
  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/alerts");
  return { ok: true as const };
}
