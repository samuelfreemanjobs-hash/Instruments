import { notFound } from "next/navigation";
import { PageHeader } from "@/components/admin/PageHeader";
import { ProductForm } from "@/components/admin/ProductForm";
import { prisma } from "@/lib/db";

export const dynamic = "force-dynamic";

export default async function EditProductPage({
  params,
}: {
  params: Promise<{ id: string }>;
}) {
  const { id } = await params;
  let product = null;
  let categories: { id: string; name: string }[] = [];
  try {
    [product, categories] = await Promise.all([
      prisma.product.findUnique({ where: { id } }),
      prisma.category.findMany({ orderBy: { name: "asc" }, select: { id: true, name: true } }),
    ]);
  } catch {
    product = null;
  }
  if (!product) notFound();

  return (
    <>
      <PageHeader title="Edit product" />
      <ProductForm
        productId={product.id}
        categories={categories}
        defaultValues={{
          title: product.title,
          slug: product.slug,
          description: product.description,
          price: product.price,
          stock: product.stock,
          categoryId: product.categoryId,
          featured: product.featured,
          images: product.images,
        }}
      />
    </>
  );
}
