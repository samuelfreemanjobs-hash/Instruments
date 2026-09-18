import { NextResponse } from "next/server";

import { getStripe } from "@/lib/stripe/server";

const EMAIL_RE = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

export async function POST(req: Request) {
  const priceId = process.env.STRIPE_TRIPWIRE_PRICE_ID;
  if (!priceId || !process.env.STRIPE_SECRET_KEY) {
    return NextResponse.json({ error: "tripwire_checkout_not_configured" }, { status: 503 });
  }

  let body: unknown;
  try {
    body = await req.json();
  } catch {
    body = {};
  }

  const email =
    typeof body === "object" && body !== null && "email" in body
      ? String((body as { email: unknown }).email).trim().toLowerCase()
      : "";

  if (email && (email.length > 320 || !EMAIL_RE.test(email))) {
    return NextResponse.json({ error: "invalid_email" }, { status: 400 });
  }

  const origin =
    process.env.LAUNCH_PUBLIC_BASE_URL ??
    (process.env.VERCEL_URL ? `https://${process.env.VERCEL_URL}` : new URL(req.url).origin);

  const stripe = getStripe();
  const session = await stripe.checkout.sessions.create({
    mode: "payment",
    line_items: [{ price: priceId, quantity: 1 }],
    success_url: `${origin}/launch/thank-you?purchase=success`,
    cancel_url: `${origin}/launch/thank-you?purchase=cancel`,
    customer_email: email || undefined,
    metadata: {
      sku: "tripwire_sample",
      product_id: "DL-LAUNCH-TRIPWIRE-SAMPLE",
    },
  });

  if (!session.url) {
    return NextResponse.json({ error: "checkout_failed" }, { status: 500 });
  }

  return NextResponse.json({ url: session.url });
}
