import Link from "next/link";
import Image from "next/image";
import { notFound } from "next/navigation";
import { getProductBySlug } from "@/lib/catalog";
import { formatPrice } from "@/lib/utils";
import { AddToCartButton } from "@/components/store/AddToCartButton";
import { FavoriteButton } from "@/components/store/FavoriteButton";

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
        <div className="relative aspect-square overflow-hidden rounded-lg bg-slate-100">
          {product.images[0] ? (
            <Image
              src={product.images[0]}
              alt={product.title}
              fill
              className="object-cover"
              sizes="(max-width: 768px) 100vw, 50vw"
              priority
            />
          ) : null}
        </div>
        <div>
          <p className="text-sm text-slate-500">{product.category.name}</p>
          <h1 className="text-3xl font-semibold">{product.title}</h1>
          <p className="mt-4 text-2xl">{formatPrice(product.price)}</p>
          <p className="mt-4 text-slate-600">{product.description}</p>
          <p className="mt-2 text-sm text-slate-500">{product.stock} in stock</p>
          <div className="mt-8 flex flex-wrap items-center gap-3">
            <AddToCartButton productId={product.id} disabled={product.stock < 1} />
            <FavoriteButton productId={product.id} />
          </div>
        </div>
      </div>
    </div>
  );
}
