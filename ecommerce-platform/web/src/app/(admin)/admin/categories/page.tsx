import { AdminListStub } from "@/components/admin/AdminListStub";

export default function CategoriesPage() {
  return (
    <AdminListStub
      title="Categories"
      description="Top-level catalog categories."
      createHref="/admin/categories/new"
      columns={[
        { key: "name", header: "Name" },
        { key: "slug", header: "Slug" },
      ]}
      rows={[{ name: "Electronics", slug: "electronics" }]}
    />
  );
}
