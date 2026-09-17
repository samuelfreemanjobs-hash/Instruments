import { prisma } from "@/lib/db";
import { readCartLines, readFavoriteIds } from "@/lib/cart/cookies";
import type { CartSnapshot, CartProductLine } from "@/lib/cart/types";

export async function getCartSnapshot(): Promise<CartSnapshot> {
  const lines = await readCartLines();
  if (lines.length === 0) {
    return { lines: [], subtotal: 0, itemCount: 0 };
  }

  try {
    const ids = lines.map((l) => l.productId);
    const products = await prisma.product.findMany({
      where: { id: { in: ids } },
      select: { id: true, title: true, slug: true, price: true, stock: true },
    });
    const byId = new Map(products.map((p) => [p.id, p]));

    const enriched: CartProductLine[] = [];
    let subtotal = 0;
    let itemCount = 0;

    for (const line of lines) {
      const p = byId.get(line.productId);
      if (!p) continue;
      const quantity = Math.min(line.quantity, Math.max(p.stock, 0));
      if (quantity <= 0) continue;
      const lineTotal = p.price * quantity;
      enriched.push({
        productId: p.id,
        quantity,
        title: p.title,
        slug: p.slug,
        price: p.price,
        stock: p.stock,
        lineTotal,
      });
      subtotal += lineTotal;
      itemCount += quantity;
    }

    return { lines: enriched, subtotal, itemCount };
  } catch {
    return { lines: [], subtotal: 0, itemCount: 0 };
  }
}

export type FavoriteProduct = {
  id: string;
  title: string;
  slug: string;
  price: number;
};

export async function getFavoriteProducts(): Promise<FavoriteProduct[]> {
  const ids = await readFavoriteIds();
  if (ids.length === 0) return [];
  try {
    return await prisma.product.findMany({
      where: { id: { in: ids } },
      select: { id: true, title: true, slug: true, price: true },
    });
  } catch {
    return [];
  }
}

export const FREE_SHIPPING_THRESHOLD = 50;
export const SHIPPING_FLAT = 5.99;

export function computeShipping(subtotal: number) {
  return subtotal >= FREE_SHIPPING_THRESHOLD ? 0 : SHIPPING_FLAT;
}
