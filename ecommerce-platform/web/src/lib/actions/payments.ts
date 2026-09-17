"use server";

import { redirect } from "next/navigation";
import { getSession } from "@/lib/auth/session";
import { prisma } from "@/lib/db";
import { appBaseUrl, getStripe, isStripeConfigured } from "@/lib/payments/stripe";
import { fulfillOrderPayment } from "@/lib/payments/fulfill";
import { PaymentProvider } from "@prisma/client";

export type PaymentActionResult = { ok: true } | { ok: false; error: string };

export async function startStripeCheckoutAction(orderId: string): Promise<PaymentActionResult> {
  const session = await getSession();
  if (!session) return { ok: false, error: "Sign in required" };

  if (!isStripeConfigured()) {
    return { ok: false, error: "Stripe is not configured. Add STRIPE_SECRET_KEY to .env.local" };
  }

  let order;
  try {
    order = await prisma.order.findFirst({
      where: { id: orderId, userId: session.userId, status: "PENDING" },
      include: { items: true },
    });
  } catch {
    return { ok: false, error: "Database unavailable" };
  }
  if (!order) return { ok: false, error: "Order not found or already paid" };

  const stripe = getStripe();
  const base = appBaseUrl();
  const amountCents = Math.round(order.total * 100);

  const checkoutSession = await stripe.checkout.sessions.create({
    mode: "payment",
    customer_email: session.email,
    line_items: [
      {
        quantity: 1,
        price_data: {
          currency: "usd",
          unit_amount: amountCents,
          product_data: {
            name: `Order ${order.id.slice(-8)}`,
            description: `${order.items.length} item(s)`,
          },
        },
      },
    ],
    metadata: { orderId: order.id },
    success_url: `${base}/account/orders/${order.id}?paid=1`,
    cancel_url: `${base}/account/orders/${order.id}?cancelled=1`,
  });

  if (!checkoutSession.url) {
    return { ok: false, error: "Could not start Stripe checkout" };
  }

  redirect(checkoutSession.url);
}

/** Development only when Stripe keys are absent — marks order PAID locally. */
export async function simulateDevPaymentAction(orderId: string): Promise<PaymentActionResult> {
  if (process.env.NODE_ENV === "production") {
    return { ok: false, error: "Not available in production" };
  }
  if (isStripeConfigured()) {
    return { ok: false, error: "Stripe is configured; use Stripe checkout" };
  }

  const session = await getSession();
  if (!session) return { ok: false, error: "Sign in required" };

  const order = await prisma.order.findFirst({
    where: { id: orderId, userId: session.userId, status: "PENDING" },
  });
  if (!order) return { ok: false, error: "Order not found" };

  try {
    await fulfillOrderPayment({
      orderId,
      provider: PaymentProvider.STRIPE,
      paymentRef: `dev_sim_${Date.now()}`,
    });
  } catch (e) {
    return { ok: false, error: e instanceof Error ? e.message : "Simulate failed" };
  }

  redirect(`/account/orders/${orderId}?paid=1`);
}
