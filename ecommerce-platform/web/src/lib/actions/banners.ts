"use server";

import { BannerType } from "@prisma/client";
import { revalidatePath } from "next/cache";
import { prisma } from "@/lib/db";
import { bannerSchema } from "@/lib/validators/banner";

export type ActionResult = { ok: true } | { ok: false; error: string };

export async function createBanner(type: BannerType, input: unknown): Promise<ActionResult> {
  const parsed = bannerSchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }
  try {
    await prisma.banner.create({
      data: {
        type,
        title: parsed.data.title,
        image: parsed.data.image,
        link: parsed.data.link || undefined,
        sortOrder: parsed.data.sortOrder ?? 0,
        active: parsed.data.active ?? true,
      },
    });
    revalidatePath(type === "WEBSITE" ? "/admin/banners/website" : "/admin/banners/app");
    revalidatePath("/");
    return { ok: true };
  } catch {
    return { ok: false, error: "Could not create banner" };
  }
}
