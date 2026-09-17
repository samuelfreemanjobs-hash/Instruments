import { AdminListStub } from "@/components/admin/AdminListStub";

export default function OffersPage() {
  return (
    <AdminListStub
      title="Home Screen Offers"
      description="Promotional tiles on the storefront home."
      createHref="/admin/offers/new"
      columns={[
        { key: "title", header: "Title" },
        { key: "active", header: "Active" },
      ]}
      rows={[{ title: "Bundle & Save", active: "Yes" }]}
    />
  );
}
