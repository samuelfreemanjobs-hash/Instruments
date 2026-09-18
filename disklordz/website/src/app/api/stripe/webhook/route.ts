import { NextResponse } from "next/server";
import type Stripe from "stripe";

import { recordTripwirePurchase } from "@/lib/stripe/launch-purchase";
import { activateProPlan, deactivateProPlan } from "@/lib/stripe/billing-sync";
import { getStripe, isStripeConfigured } from "@/lib/stripe/server";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

export const runtime = "nodejs";

export async function POST(req: Request) {
  if (!isStripeConfigured()) {
    return NextResponse.json({ error: "not_configured" }, { status: 503 });
  }

  const webhookSecret = process.env.STRIPE_WEBHOOK_SECRET;
  if (!webhookSecret) {
    return NextResponse.json({ error: "webhook_secret_missing" }, { status: 503 });
  }

  const stripe = getStripe();
  const signature = req.headers.get("stripe-signature");
  if (!signature) {
    return NextResponse.json({ error: "missing_signature" }, { status: 400 });
  }

  const body = await req.text();
  let event: Stripe.Event;
  try {
    event = stripe.webhooks.constructEvent(body, signature, webhookSecret);
  } catch {
    return NextResponse.json({ error: "invalid_signature" }, { status: 400 });
  }

  try {
    switch (event.type) {
      case "checkout.session.completed": {
        const session = event.data.object as Stripe.Checkout.Session;
        if (session.mode === "payment" && session.metadata?.sku === "tripwire_sample") {
          const admin = getSupabaseAdmin();
          if (admin) {
            await recordTripwirePurchase(admin, session);
          }
        }
        const userId = session.metadata?.user_id;
        if (userId && session.subscription && session.customer) {
          await activateProPlan(
            userId,
            String(session.customer),
            String(session.subscription),
          );
        }
        break;
      }
      case "customer.subscription.updated": {
        const sub = event.data.object as Stripe.Subscription;
        const userId = sub.metadata?.user_id;
        if (!userId) {
          break;
        }
        if (sub.status === "active" || sub.status === "trialing") {
          await activateProPlan(userId, String(sub.customer), sub.id);
        } else if (sub.status === "canceled" || sub.status === "unpaid") {
          await deactivateProPlan(userId);
        }
        break;
      }
      case "customer.subscription.deleted": {
        const sub = event.data.object as Stripe.Subscription;
        const userId = sub.metadata?.user_id;
        if (userId) {
          await deactivateProPlan(userId);
        }
        break;
      }
      default:
        break;
    }
  } catch (err) {
    console.error("stripe_webhook_handler", err);
    return NextResponse.json({ error: "handler_failed" }, { status: 500 });
  }

  return NextResponse.json({ received: true });
}
