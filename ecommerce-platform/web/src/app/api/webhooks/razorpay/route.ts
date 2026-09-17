import { NextResponse } from "next/server";
import { fulfillOrderPayment } from "@/lib/payments/fulfill";
import { isRazorpayConfigured, verifyRazorpayWebhookSignature } from "@/lib/payments/razorpay";
import { PaymentProvider } from "@prisma/client";
import { prisma } from "@/lib/db";

export const runtime = "nodejs";

export async function POST(request: Request) {
  if (!isRazorpayConfigured()) {
    return NextResponse.json({ error: "RazorPay not configured" }, { status: 503 });
  }

  const body = await request.text();
  const signature = request.headers.get("x-razorpay-signature") ?? "";
  const secret = process.env.RAZORPAY_WEBHOOK_SECRET ?? process.env.RAZORPAY_KEY_SECRET!;

  if (!verifyRazorpayWebhookSignature(body, signature, secret)) {
    return NextResponse.json({ error: "Invalid signature" }, { status: 400 });
  }

  const payload = JSON.parse(body) as {
    event?: string;
    payload?: { payment?: { entity?: { id?: string; order_id?: string } } };
  };

  if (payload.event === "payment.captured") {
    const payment = payload.payload?.payment?.entity;
    const razorpayOrderId = payment?.order_id;
    const paymentId = payment?.id;
    if (razorpayOrderId && paymentId) {
      const order = await prisma.order.findFirst({
        where: { paymentRef: razorpayOrderId, status: "PENDING" },
      });
      if (order) {
        await fulfillOrderPayment({
          orderId: order.id,
          provider: PaymentProvider.RAZORPAY,
          paymentRef: paymentId,
        });
      }
    }
  }

  return NextResponse.json({ received: true });
}
