import Link from "next/link";
import { getAllProducts } from "@/lib/catalog";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

export default async function ShopPage() {
  const products = await getAllProducts();

  return (
    <div className="mx-auto max-w-6xl px-4 py-12">
      <h1 className="text-3xl font-semibold">Shop</h1>
      <p className="mt-2 text-slate-600">{products.length} products in catalog</p>
      <div className="mt-10 grid gap-6 sm:grid-cols-2 lg:grid-cols-3">
        {products.map((p) => (
          <Link
            key={p.id}
            href={`/product/${p.slug}`}
            className="rounded-lg border border-slate-200 p-4"
          >
            <div className="mb-3 aspect-square rounded-md bg-slate-100" />
            <h2 className="font-medium">{p.title}</h2>
            <p className="text-slate-600">{formatPrice(p.price)}</p>
          </Link>
        ))}
      </div>
    </div>
  );
}
