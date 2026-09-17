import { AdminListStub } from "@/components/admin/AdminListStub";

export default function SubCategoriesPage() {
  return (
    <AdminListStub
      title="Sub-Categories"
      description="Nested under categories."
      createHref="/admin/subcategories/new"
      columns={[
        { key: "name", header: "Name" },
        { key: "category", header: "Category" },
      ]}
      rows={[{ name: "Headphones", category: "Electronics" }]}
    />
  );
}
