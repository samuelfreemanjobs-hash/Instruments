"use server";

import { revalidatePath } from "next/cache";

import { requireSignedInUser } from "@/lib/require-user";

const MAX_ALERTS_PER_USER = 100;

export async function createPriceAlert(externalDealId: string, targetPrice: number) {
  const gate = await requireSignedInUser();
  if (!gate.ok) return { ok: false as const, error: gate.error };

  const dealId = externalDealId.trim().slice(0, 128);
  if (!dealId) return { ok: false as const, error: "invalid_deal_id" };
  if (!Number.isFinite(targetPrice) || targetPrice < 0 || targetPrice > 1_000_000) {
    return { ok: false as const, error: "invalid_target_price" };
  }

  const { count, error: countError } = await gate.supabase
    .from("price_alerts")
    .select("id", { count: "exact", head: true })
    .eq("user_id", gate.userId);
  if (countError) return { ok: false as const, error: countError.message };
  if ((count ?? 0) >= MAX_ALERTS_PER_USER) {
    return { ok: false as const, error: "alert_quota_exceeded" };
  }

  const { error } = await gate.supabase.from("price_alerts").insert({
    user_id: gate.userId,
    external_deal_id: dealId,
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
