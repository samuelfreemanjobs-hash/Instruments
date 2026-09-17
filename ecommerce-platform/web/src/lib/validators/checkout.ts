import { z } from "zod";

export const checkoutSchema = z.object({
  shippingName: z.string().min(2),
  shippingLine1: z.string().min(5),
  shippingCity: z.string().min(2),
  shippingZip: z.string().min(3),
  shippingCountry: z.string().min(2),
  couponCode: z.string().optional(),
});

export type CheckoutInput = z.infer<typeof checkoutSchema>;
