"use server";

import { revalidatePath } from "next/cache";
import { prisma } from "@/lib/db";
import { toSlug } from "@/lib/slug";
import { categorySchema } from "@/lib/validators/category";

export type ActionResult = { ok: true } | { ok: false; error: string };

export async function createCategory(input: unknown): Promise<ActionResult> {
  const parsed = categorySchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }
  const slug = parsed.data.slug?.trim() || toSlug(parsed.data.name);
  try {
    await prisma.category.create({
      data: {
        name: parsed.data.name,
        slug,
        image: parsed.data.image || undefined,
      },
    });
    revalidatePath("/admin/categories");
    revalidatePath("/shop");
    return { ok: true };
  } catch {
    return { ok: false, error: "Could not create category (slug may already exist)" };
  }
}

export async function updateCategory(id: string, input: unknown): Promise<ActionResult> {
  const parsed = categorySchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }
  const slug = parsed.data.slug?.trim() || toSlug(parsed.data.name);
  try {
    await prisma.category.update({
      where: { id },
      data: {
        name: parsed.data.name,
        slug,
        image: parsed.data.image || undefined,
      },
    });
    revalidatePath("/admin/categories");
    revalidatePath("/shop");
    return { ok: true };
  } catch {
    return { ok: false, error: "Update failed" };
  }
}

export async function deleteCategory(id: string): Promise<ActionResult> {
  const count = await prisma.product.count({ where: { categoryId: id } });
  if (count > 0) {
    return { ok: false, error: `Remove ${count} product(s) from this category first` };
  }
  try {
    await prisma.category.delete({ where: { id } });
    revalidatePath("/admin/categories");
    return { ok: true };
  } catch {
    return { ok: false, error: "Delete failed" };
  }
}
