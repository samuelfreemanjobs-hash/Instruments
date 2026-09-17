import Link from "next/link";
import { getFeaturedProducts } from "@/lib/catalog";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

export default async function HomePage({
  searchParams,
}: {
  searchParams: Promise<{ error?: string }>;
}) {
  const { error } = await searchParams;
  const products = await getFeaturedProducts();

  return (
    <div>
      {error === "admin-required" ? (
        <div className="bg-amber-50 px-4 py-2 text-center text-sm text-amber-900">
          Admin access requires an administrator account.
        </div>
      ) : null}
      <section className="bg-gradient-to-br from-slate-900 to-slate-700 px-4 py-20 text-white">
        <div className="mx-auto max-w-6xl">
          <h1 className="text-4xl font-bold tracking-tight md:text-5xl">
            Modern commerce, AI-accelerated
          </h1>
          <p className="mt-4 max-w-xl text-slate-200">
            Featured products load from MongoDB. Manage catalog in the admin dashboard.
          </p>
          <Link
            href="/shop"
            className="mt-8 inline-block rounded-md bg-white px-5 py-2.5 text-sm font-medium text-slate-900"
          >
            Shop now
          </Link>
        </div>
      </section>
      <section className="mx-auto max-w-6xl px-4 py-16">
        <h2 className="text-2xl font-semibold">Featured products</h2>
        {products.length === 0 ? (
          <p className="mt-4 text-slate-600">
            No products yet. Run <code className="text-sm">npm run dev:db</code> to seed data, or
            add products in admin.
          </p>
        ) : (
          <div className="mt-8 grid gap-6 sm:grid-cols-2 lg:grid-cols-3">
            {products.map((p) => (
              <Link
                key={p.id}
                href={`/product/${p.slug}`}
                className="rounded-lg border border-slate-200 p-4 shadow-sm transition hover:border-slate-300"
              >
                <div className="mb-4 aspect-square rounded-md bg-slate-100" />
                <h3 className="font-medium">{p.title}</h3>
                <p className="mt-1 text-slate-600">{formatPrice(p.price)}</p>
              </Link>
            ))}
          </div>
        )}
      </section>
    </div>
  );
}
