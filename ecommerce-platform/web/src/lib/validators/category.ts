import { z } from "zod";

export const categorySchema = z.object({
  name: z.string().min(2, "Name is required"),
  slug: z
    .string()
    .min(2)
    .regex(/^[a-z0-9-]+$/, "Lowercase slug with hyphens only")
    .optional(),
  image: z.string().url().optional().or(z.literal("")),
});

export type CategoryInput = z.infer<typeof categorySchema>;
