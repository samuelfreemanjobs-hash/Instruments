"use server";

import { revalidatePath } from "next/cache";
import { prisma } from "@/lib/db";
import { toSlug } from "@/lib/slug";
import { productSchema } from "@/lib/validators/product";

export type ActionResult = { ok: true } | { ok: false; error: string };

export async function createProduct(input: unknown): Promise<ActionResult> {
  const parsed = productSchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }
  const slug = parsed.data.slug?.trim() || toSlug(parsed.data.title);
  try {
    await prisma.product.create({
      data: {
        title: parsed.data.title,
        description: parsed.data.description,
        price: parsed.data.price,
        comparePrice: parsed.data.comparePrice,
        stock: parsed.data.stock,
        categoryId: parsed.data.categoryId,
        subCategoryId: parsed.data.subCategoryId || undefined,
        featured: parsed.data.featured ?? false,
        slug,
        images: parsed.data.images ?? [],
      },
    });
    revalidatePath("/admin/products");
    revalidatePath("/");
    revalidatePath("/shop");
    return { ok: true };
  } catch {
    return { ok: false, error: "Could not create product" };
  }
}

export async function updateProduct(id: string, input: unknown): Promise<ActionResult> {
  const parsed = productSchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }
  const slug = parsed.data.slug?.trim() || toSlug(parsed.data.title);
  try {
    await prisma.product.update({
      where: { id },
      data: {
        title: parsed.data.title,
        description: parsed.data.description,
        price: parsed.data.price,
        comparePrice: parsed.data.comparePrice,
        stock: parsed.data.stock,
        categoryId: parsed.data.categoryId,
        subCategoryId: parsed.data.subCategoryId || undefined,
        featured: parsed.data.featured ?? false,
        slug,
        images: parsed.data.images ?? [],
      },
    });
    revalidatePath("/admin/products");
    revalidatePath("/");
    revalidatePath("/shop");
    revalidatePath(`/product/${slug}`);
    return { ok: true };
  } catch {
    return { ok: false, error: "Update failed" };
  }
}

export async function deleteProduct(id: string): Promise<ActionResult> {
  try {
    await prisma.product.delete({ where: { id } });
    revalidatePath("/admin/products");
    revalidatePath("/");
    revalidatePath("/shop");
    return { ok: true };
  } catch {
    return { ok: false, error: "Delete failed" };
  }
}
