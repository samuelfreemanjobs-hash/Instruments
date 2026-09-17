import Link from "next/link";
import { PageHeader } from "@/components/admin/PageHeader";
import { DataTable } from "@/components/admin/DataTable";
import { DeleteRowButton } from "@/components/admin/DeleteRowButton";
import { deleteProduct } from "@/lib/actions/products";
import { prisma } from "@/lib/db";
import { formatPrice } from "@/lib/utils";

export const dynamic = "force-dynamic";

type ProductRow = Awaited<
  ReturnType<
    typeof prisma.product.findMany<{ include: { category: true } }>
  >
>[number];

export default async function ProductsPage() {
  let products: ProductRow[] = [];
  try {
    products = await prisma.product.findMany({
      orderBy: { createdAt: "desc" },
      include: { category: true },
    });
  } catch {
    products = [];
  }

  return (
    <>
      <PageHeader
        title="All Products"
        description="Catalog synced with storefront."
        action={
          <Link
            href="/admin/products/new"
            className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white hover:bg-slate-800"
          >
            Create product
          </Link>
        }
      />
      <DataTable
        columns={[
          { key: "title", header: "Title" },
          {
            key: "category",
            header: "Category",
            render: (row) => row.category?.name ?? "—",
          },
          {
            key: "price",
            header: "Price",
            render: (row) => formatPrice(row.price as number),
          },
          { key: "stock", header: "Stock" },
          {
            key: "actions",
            header: "",
            render: (row) => (
              <div className="flex justify-end gap-3">
                <Link
                  href={`/admin/products/${row.id as string}/edit`}
                  className="text-xs text-slate-600 hover:underline"
                >
                  Edit
                </Link>
                <DeleteRowButton
                  label={row.title as string}
                  onDelete={() => deleteProduct(row.id as string)}
                />
              </div>
            ),
          },
        ]}
        rows={products}
      />
    </>
  );
}
