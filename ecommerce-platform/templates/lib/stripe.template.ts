import Stripe from "stripe";

export const stripe = new Stripe(process.env.STRIPE_SECRET_KEY!, {
  apiVersion: "2025-02-24.acacia",
});

export async function createCheckoutSession(params: {
  orderId: string;
  lineItems: Stripe.Checkout.SessionCreateParams.LineItem[];
  customerEmail?: string;
}) {
  const base = process.env.NEXT_PUBLIC_APP_URL ?? "http://localhost:3000";
  return stripe.checkout.sessions.create({
    mode: "payment",
    customer_email: params.customerEmail,
    line_items: params.lineItems,
    metadata: { orderId: params.orderId },
    success_url: `${base}/account/orders/${params.orderId}?paid=1`,
    cancel_url: `${base}/checkout?cancelled=1`,
  });
}
