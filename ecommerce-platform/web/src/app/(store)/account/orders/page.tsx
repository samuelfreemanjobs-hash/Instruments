import Link from "next/link";
import { redirect } from "next/navigation";
import { getSession } from "@/lib/auth/session";
import { prisma } from "@/lib/db";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

export default async function OrdersPage() {
  const session = await getSession();
  if (!session) redirect("/auth/login?next=/account/orders");

  let orders: Awaited<ReturnType<typeof prisma.order.findMany>> = [];
  try {
    orders = await prisma.order.findMany({
      where: { userId: session.userId },
      orderBy: { createdAt: "desc" },
      include: { items: true },
    });
  } catch {
    orders = [];
  }

  return (
    <div className="mx-auto max-w-3xl px-4 py-16">
      <h1 className="text-2xl font-semibold">Your orders</h1>
      {orders.length === 0 ? (
        <p className="mt-4 text-slate-600">No orders yet.</p>
      ) : (
        <ul className="mt-8 space-y-4">
          {orders.map((o) => (
            <li key={o.id} className="rounded-lg border border-slate-200 p-4">
              <div className="flex flex-wrap items-center justify-between gap-2">
                <div>
                  <p className="font-medium">Order {o.id.slice(-8)}</p>
                  <p className="text-sm text-slate-600">
                    {o.status} · {new Date(o.createdAt).toLocaleDateString()}
                  </p>
                </div>
                <p className="font-medium">{formatPrice(o.total)}</p>
              </div>
              <Link href={`/account/orders/${o.id}`} className="mt-2 inline-block text-sm underline">
                View details
              </Link>
            </li>
          ))}
        </ul>
      )}
    </div>
  );
}
