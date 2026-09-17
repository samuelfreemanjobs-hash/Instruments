import Link from "next/link";
import Image from "next/image";
import { getFeaturedProducts, getWebsiteBanners } from "@/lib/catalog";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

export default async function HomePage({
  searchParams,
}: {
  searchParams: Promise<{ error?: string }>;
}) {
  const { error } = await searchParams;
  const [products, banners] = await Promise.all([getFeaturedProducts(), getWebsiteBanners()]);

  return (
    <div>
      {error === "admin-required" ? (
        <div className="bg-amber-50 px-4 py-2 text-center text-sm text-amber-900">
          Admin access requires an administrator account.
        </div>
      ) : null}
      {banners.length > 0 ? (
        <section className="bg-slate-100 px-4 py-6">
          <div className="mx-auto flex max-w-6xl gap-4 overflow-x-auto">
            {banners.map((b) => (
              <Link
                key={b.id}
                href={b.link ?? "/shop"}
                className="relative min-w-[280px] flex-1 overflow-hidden rounded-lg bg-white shadow-sm"
              >
                <div className="relative h-40 w-full">
                  <Image src={b.image} alt={b.title} fill className="object-cover" sizes="400px" />
                </div>
                <p className="p-3 text-sm font-medium">{b.title}</p>
              </Link>
            ))}
          </div>
        </section>
      ) : (
        <section className="bg-gradient-to-br from-slate-900 to-slate-700 px-4 py-20 text-white">
          <div className="mx-auto max-w-6xl">
            <h1 className="text-4xl font-bold tracking-tight md:text-5xl">
              Modern commerce, AI-accelerated
            </h1>
            <p className="mt-4 max-w-xl text-slate-200">
              Featured products load from MongoDB. Add website banners in admin.
            </p>
            <Link
              href="/shop"
              className="mt-8 inline-block rounded-md bg-white px-5 py-2.5 text-sm font-medium text-slate-900"
            >
              Shop now
            </Link>
          </div>
        </section>
      )}
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
                <div className="relative mb-4 aspect-square overflow-hidden rounded-md bg-slate-100">
                  {p.images[0] ? (
                    <Image src={p.images[0]} alt={p.title} fill className="object-cover" sizes="300px" />
                  ) : null}
                </div>
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
