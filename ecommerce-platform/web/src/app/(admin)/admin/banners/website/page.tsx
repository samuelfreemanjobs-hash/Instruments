import { AdminListStub } from "@/components/admin/AdminListStub";

export default function WebsiteBannersPage() {
  return (
    <AdminListStub
      title="Website Banners"
      description="Homepage carousel — BannerType WEBSITE."
      createHref="/admin/banners/website/new"
      columns={[
        { key: "title", header: "Title" },
        { key: "active", header: "Active" },
      ]}
      rows={[{ title: "Summer Sale", active: "Yes" }]}
    />
  );
}
