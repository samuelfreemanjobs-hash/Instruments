import Link from "next/link";
import { notFound } from "next/navigation";
import { getProductBySlug } from "@/lib/catalog";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

export default async function ProductPage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const product = await getProductBySlug(slug);
  if (!product) notFound();

  return (
    <div className="mx-auto max-w-6xl px-4 py-12">
      <Link href="/shop" className="text-sm text-slate-600 hover:underline">
        ← Back to shop
      </Link>
      <div className="mt-8 grid gap-10 md:grid-cols-2">
        <div className="aspect-square rounded-lg bg-slate-100" />
        <div>
          <p className="text-sm text-slate-500">{product.category.name}</p>
          <h1 className="text-3xl font-semibold">{product.title}</h1>
          <p className="mt-4 text-2xl">{formatPrice(product.price)}</p>
          <p className="mt-4 text-slate-600">{product.description}</p>
          <p className="mt-2 text-sm text-slate-500">{product.stock} in stock</p>
          <button
            type="button"
            className="mt-8 rounded-md bg-slate-900 px-6 py-3 text-sm font-medium text-white"
          >
            Add to cart (SOP-07)
          </button>
        </div>
      </div>
    </div>
  );
}
