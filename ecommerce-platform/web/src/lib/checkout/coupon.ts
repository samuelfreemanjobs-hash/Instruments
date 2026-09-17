import { prisma } from "@/lib/db";

export type CouponResult =
  | { ok: true; discount: number; code: string }
  | { ok: false; error: string };

export async function validateCoupon(code: string, subtotal: number): Promise<CouponResult> {
  const normalized = code.trim().toUpperCase();
  if (!normalized) return { ok: false, error: "Enter a coupon code" };

  try {
    const coupon = await prisma.coupon.findUnique({ where: { code: normalized } });
    if (!coupon || !coupon.active) {
      return { ok: false, error: "Invalid or inactive coupon" };
    }
    if (coupon.expiresAt && coupon.expiresAt < new Date()) {
      return { ok: false, error: "Coupon has expired" };
    }
    if (coupon.maxUses != null && coupon.usedCount >= coupon.maxUses) {
      return { ok: false, error: "Coupon usage limit reached" };
    }
    if (coupon.minCart != null && subtotal < coupon.minCart) {
      return { ok: false, error: `Minimum cart $${coupon.minCart.toFixed(2)} required` };
    }

    let discount = 0;
    if (coupon.discountPct != null) {
      discount = (subtotal * coupon.discountPct) / 100;
    } else if (coupon.discountAmt != null) {
      discount = coupon.discountAmt;
    }
    discount = Math.min(discount, subtotal);

    return { ok: true, discount, code: normalized };
  } catch {
    return { ok: false, error: "Could not validate coupon" };
  }
}
