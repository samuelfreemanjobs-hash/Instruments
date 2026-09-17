import Link from "next/link";
import { formatPrice } from "@/lib/utils";

export default async function ProductPage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const title = slug.replace(/-/g, " ").replace(/\b\w/g, (c) => c.toUpperCase());

  return (
    <div className="mx-auto max-w-6xl px-4 py-12">
      <Link href="/shop" className="text-sm text-slate-600 hover:underline">
        ← Back to shop
      </Link>
      <div className="mt-8 grid gap-10 md:grid-cols-2">
        <div className="aspect-square rounded-lg bg-slate-100" />
        <div>
          <h1 className="text-3xl font-semibold">{title}</h1>
          <p className="mt-4 text-2xl">{formatPrice(79.99)}</p>
          <p className="mt-4 text-slate-600">
            Product detail wired to Prisma by slug in SOP-05. Reviews, gallery, and add-to-cart in
            SOP-07.
          </p>
          <button
            type="button"
            className="mt-8 rounded-md bg-slate-900 px-6 py-3 text-sm font-medium text-white"
          >
            Add to cart
          </button>
        </div>
      </div>
    </div>
  );
}
