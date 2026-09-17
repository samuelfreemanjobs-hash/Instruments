"use server";

import { prisma } from "@/lib/db";
import { revalidatePath } from "next/cache";
import { productSchema } from "@/lib/validators/product";

export async function createProduct(input: unknown) {
  const data = productSchema.parse(input);
  await prisma.product.create({ data });
  revalidatePath("/admin/products");
}

export async function updateProduct(id: string, input: unknown) {
  const data = productSchema.parse(input);
  await prisma.product.update({ where: { id }, data });
  revalidatePath("/admin/products");
}

export async function deleteProduct(id: string) {
  await prisma.product.delete({ where: { id } });
  revalidatePath("/admin/products");
}
