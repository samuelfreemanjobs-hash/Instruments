import Link from "next/link";
import { notFound, redirect } from "next/navigation";
import { getSession } from "@/lib/auth/session";
import { prisma } from "@/lib/db";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

export default async function OrderDetailPage({
  params,
  searchParams,
}: {
  params: Promise<{ id: string }>;
  searchParams: Promise<{ placed?: string }>;
}) {
  const session = await getSession();
  if (!session) redirect("/auth/login");

  const { id } = await params;
  const { placed } = await searchParams;

  let order = null;
  try {
    order = await prisma.order.findFirst({
      where: { id, userId: session.userId },
      include: { items: true },
    });
  } catch {
    order = null;
  }
  if (!order) notFound();

  return (
    <div className="mx-auto max-w-3xl px-4 py-16">
      {placed === "1" ? (
        <div className="mb-6 rounded-md bg-green-50 px-4 py-3 text-sm text-green-900">
          Order placed — payment integration (Stripe/RazorPay) comes in SOP-08.
        </div>
      ) : null}
      <Link href="/account/orders" className="text-sm text-slate-600 hover:underline">
        ← All orders
      </Link>
      <h1 className="mt-4 text-2xl font-semibold">Order details</h1>
      <p className="text-sm text-slate-600">
        Status: <strong>{order.status}</strong>
      </p>
      <section className="mt-8 rounded-lg border p-4">
        <h2 className="font-medium">Items</h2>
        <ul className="mt-3 space-y-2 text-sm">
          {order.items.map((item) => (
            <li key={item.id} className="flex justify-between">
              <span>
                {item.title} × {item.quantity}
              </span>
              <span>{formatPrice(item.price * item.quantity)}</span>
            </li>
          ))}
        </ul>
        <dl className="mt-4 space-y-1 border-t pt-4 text-sm">
          <div className="flex justify-between">
            <dt>Subtotal</dt>
            <dd>{formatPrice(order.subtotal)}</dd>
          </div>
          <div className="flex justify-between">
            <dt>Shipping</dt>
            <dd>{formatPrice(order.shipping)}</dd>
          </div>
          {order.discount > 0 ? (
            <div className="flex justify-between text-green-700">
              <dt>Coupon {order.couponCode}</dt>
              <dd>-{formatPrice(order.discount)}</dd>
            </div>
          ) : null}
          <div className="flex justify-between font-semibold">
            <dt>Total</dt>
            <dd>{formatPrice(order.total)}</dd>
          </div>
        </dl>
      </section>
      <section className="mt-6 rounded-lg border p-4 text-sm">
        <h2 className="font-medium">Shipping to</h2>
        <p className="mt-2 text-slate-600">
          {order.shippingName}
          <br />
          {order.shippingLine1}
          <br />
          {order.shippingCity}, {order.shippingZip}
          <br />
          {order.shippingCountry}
        </p>
      </section>
      {order.status === "PENDING" ? (
        <button
          type="button"
          className="mt-8 w-full rounded-md bg-slate-900 py-2.5 text-sm text-white opacity-80"
          disabled
        >
          Pay with Stripe (SOP-08)
        </button>
      ) : null}
    </div>
  );
}
