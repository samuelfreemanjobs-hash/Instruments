import { randomUUID } from "crypto";

import { getSupabaseAdmin } from "@/lib/supabase/admin";

export const GENERATION_CREDIT_COST = 1;

export type BillingSnapshot = {
  plan: "free" | "pro";
  creditsBalance: number;
  generationCreditCost: number;
};

export async function ensureUserBilling(userId: string): Promise<void> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    return;
  }
  await admin.rpc("ensure_user_billing", { p_user_id: userId });
}

export async function getBillingSnapshot(userId: string): Promise<BillingSnapshot | null> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    return null;
  }
  await ensureUserBilling(userId);
  const { data, error } = await admin
    .from("user_billing")
    .select("plan, credits_balance")
    .eq("user_id", userId)
    .maybeSingle();

  if (error || !data) {
    return null;
  }

  const plan = data.plan === "pro" ? "pro" : "free";
  return {
    plan,
    creditsBalance: data.credits_balance ?? 0,
    generationCreditCost: GENERATION_CREDIT_COST,
  };
}

export async function spendGenerationCredit(
  userId: string,
  batchId: string,
  amount: number = GENERATION_CREDIT_COST,
): Promise<boolean> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    return true;
  }
  const { data, error } = await admin.rpc("spend_generation_credit", {
    p_user_id: userId,
    p_batch_id: batchId,
    p_amount: Math.max(1, Math.floor(amount)),
  });
  if (error) {
    console.error("spend_generation_credit", error.message);
    return false;
  }
  return Boolean(data);
}

export async function refundGenerationCredit(
  userId: string,
  batchId: string,
  amount: number = GENERATION_CREDIT_COST,
): Promise<void> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    return;
  }
  await admin.rpc("refund_generation_credit", {
    p_user_id: userId,
    p_batch_id: batchId,
    p_amount: Math.max(1, Math.floor(amount)),
  });
}

export function newBatchId(): string {
  return randomUUID();
}
