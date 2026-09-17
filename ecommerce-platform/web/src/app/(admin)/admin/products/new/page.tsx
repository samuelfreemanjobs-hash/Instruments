import { PageHeader } from "@/components/admin/PageHeader";
import { ProductForm } from "@/components/admin/ProductForm";
import { prisma } from "@/lib/db";

export const dynamic = "force-dynamic";

export default async function NewProductPage() {
  let categories: { id: string; name: string }[] = [];
  try {
    categories = await prisma.category.findMany({
      orderBy: { name: "asc" },
      select: { id: true, name: true },
    });
  } catch {
    categories = [];
  }

  return (
    <>
      <PageHeader title="Create Product" description="Validated with Zod; saved via server action." />
      <ProductForm categories={categories} />
    </>
  );
}
