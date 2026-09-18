import { NextResponse } from "next/server";

import { getStripe, isStripeConfigured } from "@/lib/stripe/server";
import { getSupabaseAdmin } from "@/lib/supabase/admin";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

/** Stripe Customer Portal — manage subscription / payment method (WO-SAAS-010). */
export async function POST(req: Request) {
  if (!isStripeConfigured() || !isSupabaseConfigured()) {
    return NextResponse.json({ error: "billing_not_configured" }, { status: 503 });
  }

  const supabase = await createClient();
  if (!supabase) {
    return NextResponse.json({ error: "unauthorized" }, { status: 401 });
  }

  const {
    data: { user },
  } = await supabase.auth.getUser();
  if (!user) {
    return NextResponse.json({ error: "unauthorized" }, { status: 401 });
  }

  const admin = getSupabaseAdmin();
  if (!admin) {
    return NextResponse.json({ error: "billing_unavailable" }, { status: 503 });
  }

  const { data: billing } = await admin
    .from("user_billing")
    .select("stripe_customer_id")
    .eq("user_id", user.id)
    .maybeSingle();

  const customerId = billing?.stripe_customer_id;
  if (!customerId) {
    return NextResponse.json(
      { error: "no_customer", message: "Subscribe to Pro first to manage billing." },
      { status: 400 },
    );
  }

  const origin = new URL(req.url).origin;
  const stripe = getStripe();
  const portal = await stripe.billingPortal.sessions.create({
    customer: customerId,
    return_url: `${origin}/account`,
  });

  return NextResponse.json({ url: portal.url });
}
