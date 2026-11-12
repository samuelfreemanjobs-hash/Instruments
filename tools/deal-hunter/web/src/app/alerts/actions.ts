"use server";

import { revalidatePath } from "next/cache";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { requireSignedInUser } from "@/lib/require-user";

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
  const gate = await requireSignedInUser();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  const { data, error } = await gate.supabase
    .from("price_alerts")
    .update({ active })
    .eq("id", alertId)
    .eq("user_id", gate.userId)
    .select("id")
    .maybeSingle();
  if (error) return { ok: false as const, error: error.message };
  if (!data) return { ok: false as const, error: "not_found" };
  revalidatePath("/alerts");
  return { ok: true as const };
}

export async function deletePriceAlert(alertId: string) {
  const gate = await requireSignedInUser();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  const { data, error } = await gate.supabase
    .from("price_alerts")
    .delete()
    .eq("id", alertId)
    .eq("user_id", gate.userId)
    .select("id")
    .maybeSingle();
  if (error) return { ok: false as const, error: error.message };
  if (!data) return { ok: false as const, error: "not_found" };
  revalidatePath("/alerts");
  return { ok: true as const };
}
