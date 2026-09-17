import { OrderStatus, PaymentProvider } from "@prisma/client";
import { prisma } from "@/lib/db";
import { sendOrderConfirmationEmail } from "@/lib/email/order-confirmation";

/** Idempotent: only transitions PENDING → PAID once; decrements stock on first pay. */
export async function fulfillOrderPayment(params: {
  orderId: string;
  provider: PaymentProvider;
  paymentRef: string;
}) {
  return prisma.$transaction(async (tx) => {
    const order = await tx.order.findUnique({
      where: { id: params.orderId },
      include: { items: true },
    });
    if (!order) return { updated: false as const, reason: "not_found" as const };
    if (order.status === OrderStatus.PAID) {
      return { updated: false as const, reason: "already_paid" as const };
    }
    if (order.status !== OrderStatus.PENDING) {
      return { updated: false as const, reason: "invalid_status" as const };
    }

    for (const item of order.items) {
      const result = await tx.product.updateMany({
        where: { id: item.productId, stock: { gte: item.quantity } },
        data: { stock: { decrement: item.quantity } },
      });
      if (result.count === 0) {
        throw new Error(`Insufficient stock for ${item.title}`);
      }
    }

    await tx.order.update({
      where: { id: params.orderId },
      data: {
        status: OrderStatus.PAID,
        paymentProvider: params.provider,
        paymentRef: params.paymentRef,
      },
    });

    return { updated: true as const };
  }).then(async (result) => {
    if (result.updated) {
      try {
        await sendOrderConfirmationEmail(params.orderId);
      } catch (e) {
        console.error("Order confirmation email failed", e);
      }
    }
    return result;
  });
}
