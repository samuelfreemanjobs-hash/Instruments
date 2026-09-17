import { getSupabaseAdmin } from "@/lib/supabase/admin";

export async function activateProPlan(
  userId: string,
  stripeCustomerId: string,
  stripeSubscriptionId: string,
): Promise<void> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    throw new Error("supabase_admin_missing");
  }
  await admin.rpc("ensure_user_billing", { p_user_id: userId });
  const { error } = await admin.from("user_billing").upsert(
    {
      user_id: userId,
      plan: "pro",
      stripe_customer_id: stripeCustomerId,
      stripe_subscription_id: stripeSubscriptionId,
      updated_at: new Date().toISOString(),
    },
    { onConflict: "user_id" },
  );
  if (error) {
    throw error;
  }
  await admin.from("credit_ledger").insert({
    user_id: userId,
    delta: 0,
    reason: "stripe_pro_activated",
    idempotency_key: `pro:${stripeSubscriptionId}`,
  });
}

export async function deactivateProPlan(userId: string): Promise<void> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    throw new Error("supabase_admin_missing");
  }
  const { error } = await admin
    .from("user_billing")
    .update({
      plan: "free",
      stripe_subscription_id: null,
      updated_at: new Date().toISOString(),
    })
    .eq("user_id", userId);
  if (error) {
    throw error;
  }
}
