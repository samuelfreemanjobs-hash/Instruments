import { z } from "zod";

export const productSchema = z.object({
  title: z.string().min(2, "Title is required"),
  slug: z
    .string()
    .min(2)
    .regex(/^[a-z0-9-]+$/, "Lowercase slug with hyphens")
    .optional(),
  description: z.string().min(10),
  price: z.number().positive(),
  comparePrice: z.number().positive().optional(),
  stock: z.number().int().min(0),
  categoryId: z.string().min(1),
  subCategoryId: z.string().optional(),
  featured: z.boolean().optional(),
  images: z.array(z.string()).optional(),
});

export type ProductInput = z.infer<typeof productSchema>;
