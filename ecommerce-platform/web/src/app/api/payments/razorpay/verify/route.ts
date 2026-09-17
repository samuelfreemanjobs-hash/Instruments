import { NextResponse } from "next/server";
import { getSession } from "@/lib/auth/session";
import { prisma } from "@/lib/db";
import { fulfillOrderPayment } from "@/lib/payments/fulfill";
import { isRazorpayConfigured, verifyRazorpayPaymentSignature } from "@/lib/payments/razorpay";
import { PaymentProvider } from "@prisma/client";

export const runtime = "nodejs";

export async function POST(request: Request) {
  if (!isRazorpayConfigured()) {
    return NextResponse.json({ error: "RazorPay not configured" }, { status: 503 });
  }

  const session = await getSession();
  if (!session) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  const body = (await request.json()) as {
    shopOrderId?: string;
    razorpay_order_id?: string;
    razorpay_payment_id?: string;
    razorpay_signature?: string;
  };

  if (
    !body.shopOrderId ||
    !body.razorpay_order_id ||
    !body.razorpay_payment_id ||
    !body.razorpay_signature
  ) {
    return NextResponse.json({ error: "Missing payment fields" }, { status: 400 });
  }

  const valid = verifyRazorpayPaymentSignature({
    orderId: body.razorpay_order_id,
    paymentId: body.razorpay_payment_id,
    signature: body.razorpay_signature,
  });
  if (!valid) {
    return NextResponse.json({ error: "Invalid signature" }, { status: 400 });
  }

  const owned = await prisma.order.findFirst({
    where: { id: body.shopOrderId, userId: session.userId, status: "PENDING" },
  });
  if (!owned) {
    return NextResponse.json({ error: "Order not found" }, { status: 404 });
  }

  try {
    const result = await fulfillOrderPayment({
      orderId: body.shopOrderId,
      provider: PaymentProvider.RAZORPAY,
      paymentRef: body.razorpay_payment_id,
    });
    return NextResponse.json({ ok: true, updated: result.updated });
  } catch (e) {
    return NextResponse.json(
      { error: e instanceof Error ? e.message : "Fulfillment failed" },
      { status: 500 },
    );
  }
}
