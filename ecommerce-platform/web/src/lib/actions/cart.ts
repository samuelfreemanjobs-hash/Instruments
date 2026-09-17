"use server";

import { revalidatePath } from "next/cache";
import { readCartLines, writeCartLines } from "@/lib/cart/cookies";

export type CartActionResult = { ok: true } | { ok: false; error: string };

export async function addToCartAction(productId: string, quantity = 1): Promise<CartActionResult> {
  if (!productId) return { ok: false, error: "Invalid product" };
  const qty = Math.max(1, Math.min(quantity, 99));
  const lines = await readCartLines();
  const idx = lines.findIndex((l) => l.productId === productId);
  if (idx >= 0) {
    lines[idx] = { productId, quantity: lines[idx].quantity + qty };
  } else {
    lines.push({ productId, quantity: qty });
  }
  await writeCartLines(lines);
  revalidatePath("/", "layout");
  return { ok: true };
}

export async function updateCartQuantityAction(
  productId: string,
  quantity: number,
): Promise<CartActionResult> {
  const lines = await readCartLines();
  const next = lines
    .map((l) => (l.productId === productId ? { ...l, quantity } : l))
    .filter((l) => l.quantity > 0);
  await writeCartLines(next);
  revalidatePath("/", "layout");
  return { ok: true };
}

export async function removeFromCartAction(productId: string): Promise<CartActionResult> {
  const lines = await readCartLines();
  await writeCartLines(lines.filter((l) => l.productId !== productId));
  revalidatePath("/", "layout");
  return { ok: true };
}

export async function clearCartAction() {
  await writeCartLines([]);
  revalidatePath("/", "layout");
}
