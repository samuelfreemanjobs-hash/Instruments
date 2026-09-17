import { NextResponse } from "next/server";
import { getSession } from "@/lib/auth/session";
import { prisma } from "@/lib/db";
import { createRazorpayOrder, isRazorpayConfigured } from "@/lib/payments/razorpay";

export const runtime = "nodejs";

export async function POST(request: Request) {
  if (!isRazorpayConfigured()) {
    return NextResponse.json({ error: "RazorPay not configured" }, { status: 503 });
  }

  const session = await getSession();
  if (!session) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  const { orderId } = (await request.json()) as { orderId?: string };
  if (!orderId) {
    return NextResponse.json({ error: "orderId required" }, { status: 400 });
  }

  const order = await prisma.order.findFirst({
    where: { id: orderId, userId: session.userId, status: "PENDING" },
  });
  if (!order) {
    return NextResponse.json({ error: "Order not found" }, { status: 404 });
  }

  // Store totals in USD; RazorPay demo uses INR conversion placeholder (× 90 × 100 paise)
  const amountPaise = Math.round(order.total * 90 * 100);

  try {
    const rpOrder = await createRazorpayOrder(amountPaise, order.id.slice(-12));
    await prisma.order.update({
      where: { id: order.id },
      data: { paymentProvider: "RAZORPAY", paymentRef: rpOrder.id },
    });
    return NextResponse.json({
      keyId: process.env.RAZORPAY_KEY_ID,
      razorpayOrderId: rpOrder.id,
      amount: rpOrder.amount,
      currency: rpOrder.currency,
      shopOrderId: order.id,
    });
  } catch (e) {
    return NextResponse.json(
      { error: e instanceof Error ? e.message : "RazorPay error" },
      { status: 500 },
    );
  }
}
