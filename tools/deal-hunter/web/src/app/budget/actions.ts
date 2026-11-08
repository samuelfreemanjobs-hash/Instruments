"use server";

import { revalidatePath } from "next/cache";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export type BudgetSnapshot = {
  budgetTotal: number | null;
  budgetByCategory: Record<string, number>;
};

export async function loadShoppingBudget(): Promise<{ ok: true; data: BudgetSnapshot } | { ok: false; error: string }> {
  if (!isSupabaseConfigured()) return { ok: false, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { data: auth } = await supabase!.auth.getUser();
  if (!auth.user) return { ok: false, error: "not_signed_in" };

  const { data, error } = await supabase!
    .from("user_preferences")
    .select("budget_total, budget_by_category")
    .eq("user_id", auth.user.id)
    .maybeSingle();

  if (error) return { ok: false, error: error.message };
  const row = data as { budget_total: number | null; budget_by_category: Record<string, number> } | null;
  return {
    ok: true,
    data: {
      budgetTotal: row?.budget_total != null ? Number(row.budget_total) : null,
      budgetByCategory: (row?.budget_by_category ?? {}) as Record<string, number>,
    },
  };
}

export async function saveShoppingBudget(budgetTotal: number, budgetByCategory: Record<string, number>) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  if (!Number.isFinite(budgetTotal) || budgetTotal < 0 || budgetTotal > 1_000_000) {
    return { ok: false as const, error: "invalid_budget" };
  }
  const supabase = await createClient();
  const { data: auth } = await supabase!.auth.getUser();
  if (!auth.user) return { ok: false as const, error: "not_signed_in" };

  const { error } = await supabase!
    .from("user_preferences")
    .update({
      budget_total: budgetTotal,
      budget_by_category: budgetByCategory,
      updated_at: new Date().toISOString(),
    })
    .eq("user_id", auth.user.id);

  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/budget");
  return { ok: true as const };
}
