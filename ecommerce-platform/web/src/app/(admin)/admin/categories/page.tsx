import Link from "next/link";
import { PageHeader } from "@/components/admin/PageHeader";
import { DataTable } from "@/components/admin/DataTable";
import { DeleteRowButton } from "@/components/admin/DeleteRowButton";
import { deleteCategory } from "@/lib/actions/categories";
import { prisma } from "@/lib/db";

export const dynamic = "force-dynamic";

export default async function CategoriesPage() {
  let categories: Awaited<ReturnType<typeof prisma.category.findMany>> = [];
  try {
    categories = await prisma.category.findMany({ orderBy: { name: "asc" } });
  } catch {
    categories = [];
  }

  return (
    <>
      <PageHeader
        title="Categories"
        description="Top-level catalog categories (live MongoDB)."
        action={
          <Link
            href="/admin/categories/new"
            className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white hover:bg-slate-800"
          >
            New category
          </Link>
        }
      />
      <DataTable
        columns={[
          { key: "name", header: "Name" },
          { key: "slug", header: "Slug" },
          {
            key: "actions",
            header: "",
            render: (row) => (
              <div className="flex justify-end gap-3">
                <Link
                  href={`/admin/categories/${row.id as string}/edit`}
                  className="text-xs text-slate-600 hover:underline"
                >
                  Edit
                </Link>
                <DeleteRowButton
                  label={row.name as string}
                  onDelete={() => deleteCategory(row.id as string)}
                />
              </div>
            ),
          },
        ]}
        rows={categories}
        emptyMessage="No categories yet. Create one to add products."
      />
    </>
  );
}
