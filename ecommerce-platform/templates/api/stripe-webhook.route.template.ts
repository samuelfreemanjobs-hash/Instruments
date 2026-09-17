// web/src/app/api/webhooks/stripe/route.ts
import { NextResponse } from "next/server";
import Stripe from "stripe";
import { stripe } from "@/lib/payments/stripe";
import { prisma } from "@/lib/db";

export async function POST(request: Request) {
  const body = await request.text();
  const sig = request.headers.get("stripe-signature");
  if (!sig) return NextResponse.json({ error: "Missing signature" }, { status: 400 });

  let event: Stripe.Event;
  try {
    event = stripe.webhooks.constructEvent(body, sig, process.env.STRIPE_WEBHOOK_SECRET!);
  } catch {
    return NextResponse.json({ error: "Invalid signature" }, { status: 400 });
  }

  if (event.type === "checkout.session.completed") {
    const session = event.data.object as Stripe.Checkout.Session;
    const orderId = session.metadata?.orderId;
    if (orderId) {
      await prisma.order.updateMany({
        where: { id: orderId, status: "PENDING" },
        data: { status: "PAID", paymentRef: session.id, paymentProvider: "STRIPE" },
      });
    }
  }

  return NextResponse.json({ received: true });
}
