"use server";

import { OrderStatus } from "@prisma/client";
import { redirect } from "next/navigation";
import { getSession } from "@/lib/auth/session";
import { getCartSnapshot, computeShipping } from "@/lib/cart/server";
import { clearCartAction } from "@/lib/actions/cart";
import { validateCoupon } from "@/lib/checkout/coupon";
import { checkoutSchema } from "@/lib/validators/checkout";
import { prisma } from "@/lib/db";

export type CheckoutResult = { ok: true; orderId: string } | { ok: false; error: string };

export async function previewCheckoutAction(couponCode?: string) {
  const cart = await getCartSnapshot();
  if (cart.lines.length === 0) {
    return { subtotal: 0, shipping: 0, discount: 0, total: 0, couponError: "Cart is empty" as string | null };
  }
  const shipping = computeShipping(cart.subtotal);
  let discount = 0;
  let couponError: string | null = null;
  if (couponCode?.trim()) {
    const c = await validateCoupon(couponCode, cart.subtotal);
    if (c.ok) discount = c.discount;
    else couponError = c.error;
  }
  const total = Math.max(0, cart.subtotal + shipping - discount);
  return { subtotal: cart.subtotal, shipping, discount, total, couponError };
}

export async function placeOrderAction(input: unknown): Promise<CheckoutResult> {
  const session = await getSession();
  if (!session) {
    return { ok: false, error: "Sign in to checkout" };
  }

  const parsed = checkoutSchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid shipping details" };
  }

  const cart = await getCartSnapshot();
  if (cart.lines.length === 0) {
    return { ok: false, error: "Your cart is empty" };
  }

  for (const line of cart.lines) {
    if (line.quantity > line.stock) {
      return { ok: false, error: `Not enough stock for ${line.title}` };
    }
  }

  const shipping = computeShipping(cart.subtotal);
  let discount = 0;
  let couponCode: string | undefined;
  if (parsed.data.couponCode?.trim()) {
    const c = await validateCoupon(parsed.data.couponCode, cart.subtotal);
    if (!c.ok) return { ok: false, error: c.error };
    discount = c.discount;
    couponCode = c.code;
  }

  const total = Math.max(0, cart.subtotal + shipping - discount);

  try {
    const order = await prisma.$transaction(async (tx) => {
      if (couponCode) {
        await tx.coupon.update({
          where: { code: couponCode },
          data: { usedCount: { increment: 1 } },
        });
      }

      return tx.order.create({
        data: {
          userId: session.userId,
          status: OrderStatus.PENDING,
          subtotal: cart.subtotal,
          discount,
          shipping,
          total,
          shippingName: parsed.data.shippingName,
          shippingLine1: parsed.data.shippingLine1,
          shippingCity: parsed.data.shippingCity,
          shippingZip: parsed.data.shippingZip,
          shippingCountry: parsed.data.shippingCountry,
          couponCode,
          items: {
            create: cart.lines.map((l) => ({
              productId: l.productId,
              title: l.title,
              price: l.price,
              quantity: l.quantity,
            })),
          },
        },
      });
    });

    await clearCartAction();
    redirect(`/account/orders/${order.id}?placed=1`);
  } catch (e) {
    const msg = e instanceof Error ? e.message : "Could not place order";
    return { ok: false, error: msg.includes("Stock") ? msg : "Could not place order" };
  }
}
