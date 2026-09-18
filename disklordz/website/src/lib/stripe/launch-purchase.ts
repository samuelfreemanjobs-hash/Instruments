import type Stripe from "stripe";

import type { SupabaseClient } from "@supabase/supabase-js";

export async function recordTripwirePurchase(
  supabase: SupabaseClient,
  session: Stripe.Checkout.Session,
): Promise<void> {
  if (session.metadata?.sku !== "tripwire_sample") return;

  await supabase.from("launch_purchases").upsert(
    {
      sku: "tripwire_sample",
      email: session.customer_details?.email ?? session.customer_email ?? null,
      stripe_session_id: session.id,
      amount_total: session.amount_total ?? null,
      currency: session.currency ?? null,
    },
    { onConflict: "stripe_session_id" },
  );
}
