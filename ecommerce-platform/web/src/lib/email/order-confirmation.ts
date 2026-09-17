import { prisma } from "@/lib/db";
import { sendEmail } from "@/lib/email/mailer";
import { formatPrice } from "@/lib/utils";
import { appBaseUrl } from "@/lib/payments/stripe";

export async function sendOrderConfirmationEmail(orderId: string) {
  const order = await prisma.order.findUnique({
    where: { id: orderId },
    include: { items: true, user: true },
  });
  if (!order?.user?.email) return;

  const lines = order.items
    .map(
      (i) =>
        `${i.title} × ${i.quantity} — ${formatPrice(i.price * i.quantity)}`,
    )
    .join("\n");

  const orderUrl = `${appBaseUrl()}/account/orders/${order.id}`;
  const subject = `Order confirmed — ${order.id.slice(-8)}`;
  const text = [
    `Hi ${order.user.name ?? "there"},`,
    "",
    "Thanks for your order!",
    "",
    lines,
    "",
    `Subtotal: ${formatPrice(order.subtotal)}`,
    `Shipping: ${formatPrice(order.shipping)}`,
    order.discount > 0 ? `Discount: -${formatPrice(order.discount)}` : "",
    `Total: ${formatPrice(order.total)}`,
    "",
    `Ship to: ${order.shippingName}, ${order.shippingLine1}, ${order.shippingCity} ${order.shippingZip}, ${order.shippingCountry}`,
    "",
    `View order: ${orderUrl}`,
  ]
    .filter(Boolean)
    .join("\n");

  const html = `
    <h1>Order confirmed</h1>
    <p>Hi ${order.user.name ?? "there"},</p>
    <p>Thanks for your order <strong>#${order.id.slice(-8)}</strong>.</p>
    <table cellpadding="8" cellspacing="0" border="1" style="border-collapse:collapse">
      <tr><th align="left">Item</th><th>Qty</th><th>Line total</th></tr>
      ${order.items
        .map(
          (i) =>
            `<tr><td>${i.title}</td><td align="center">${i.quantity}</td><td align="right">${formatPrice(i.price * i.quantity)}</td></tr>`,
        )
        .join("")}
    </table>
    <p><strong>Total: ${formatPrice(order.total)}</strong></p>
    <p>Ship to:<br/>
    ${order.shippingName}<br/>
    ${order.shippingLine1}<br/>
    ${order.shippingCity}, ${order.shippingZip}<br/>
    ${order.shippingCountry}</p>
    <p><a href="${orderUrl}">View your order</a></p>
  `;

  await sendEmail({
    to: order.user.email,
    subject,
    html,
    text,
  });
}
