import { cookies } from "next/headers";
import type { CartLine } from "@/lib/cart/types";

export const CART_COOKIE = "shop_cart";
export const FAVORITES_COOKIE = "shop_favorites";

export async function readCartLines(): Promise<CartLine[]> {
  const jar = await cookies();
  const raw = jar.get(CART_COOKIE)?.value;
  if (!raw) return [];
  try {
    const parsed = JSON.parse(raw) as CartLine[];
    if (!Array.isArray(parsed)) return [];
    return parsed.filter(
      (l) => l && typeof l.productId === "string" && typeof l.quantity === "number" && l.quantity > 0,
    );
  } catch {
    return [];
  }
}

export async function writeCartLines(lines: CartLine[]) {
  const jar = await cookies();
  jar.set(CART_COOKIE, JSON.stringify(lines), {
    httpOnly: true,
    sameSite: "lax",
    path: "/",
    maxAge: 60 * 60 * 24 * 30,
  });
}

export async function readFavoriteIds(): Promise<string[]> {
  const jar = await cookies();
  const raw = jar.get(FAVORITES_COOKIE)?.value;
  if (!raw) return [];
  try {
    const parsed = JSON.parse(raw) as string[];
    return Array.isArray(parsed) ? parsed.filter((id) => typeof id === "string") : [];
  } catch {
    return [];
  }
}

export async function writeFavoriteIds(ids: string[]) {
  const jar = await cookies();
  jar.set(FAVORITES_COOKIE, JSON.stringify(ids), {
    httpOnly: true,
    sameSite: "lax",
    path: "/",
    maxAge: 60 * 60 * 24 * 365,
  });
}
