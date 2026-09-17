import { z } from "zod";

export const couponSchema = z
  .object({
    code: z.string().min(3).toUpperCase(),
    discountPct: z.coerce.number().min(0).max(100).optional(),
    discountAmt: z.coerce.number().min(0).optional(),
    minCart: z.coerce.number().min(0).optional(),
    maxUses: z.coerce.number().int().positive().optional(),
    expiresAt: z.coerce.date().optional(),
    active: z.boolean().default(true),
  })
  .refine((d) => d.discountPct != null || d.discountAmt != null, {
    message: "Provide discount percentage or fixed amount",
  });

export type CouponInput = z.infer<typeof couponSchema>;
