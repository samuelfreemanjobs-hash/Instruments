import { z } from "zod";

export const bannerSchema = z.object({
  title: z.string().min(2),
  image: z.string().min(1, "Image is required"),
  link: z.string().url().optional().or(z.literal("")),
  sortOrder: z.number().int().min(0).optional(),
  active: z.boolean().optional(),
});

export type BannerInput = z.infer<typeof bannerSchema>;
