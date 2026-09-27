import { isStripeConfigured } from "@/lib/stripe/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export const dynamic = "force-dynamic";

/** Production readiness probe — no secrets in response. */
export async function GET() {
  const supabaseAuth = isSupabaseConfigured();
  const supabaseStorage = Boolean(process.env.SUPABASE_SERVICE_ROLE_KEY);
  const stripe = isStripeConfigured();
  const stripeWebhook = Boolean(process.env.STRIPE_WEBHOOK_SECRET);

  const guestGenerateReady = true;
  const accountsReady = supabaseAuth && supabaseStorage;
  const billingReady = stripe && stripeWebhook && accountsReady;

  const status =
    guestGenerateReady && (!supabaseAuth || accountsReady)
      ? "ok"
      : "degraded";

  return Response.json({
    status,
    service: "disklordz-drum-saas",
    version: process.env.npm_package_version ?? "0.1.0",
    features: {
      guestGenerate: guestGenerateReady,
      supabaseAuth,
      supabaseStorage,
      accountsReady,
      stripeCheckout: stripe,
      stripeWebhook,
      billingReady,
    },
    hints: {
      accounts: accountsReady
        ? null
        : "Set NEXT_PUBLIC_SUPABASE_* and SUPABASE_SERVICE_ROLE_KEY on Vercel.",
      billing: billingReady
        ? null
        : "Set STRIPE_SECRET_KEY, STRIPE_PRO_PRICE_ID, STRIPE_WEBHOOK_SECRET.",
    },
  });
}
