import { AdminListStub } from "@/components/admin/AdminListStub";

export default function AppBannersPage() {
  return (
    <AdminListStub
      title="App Banners"
      description="Future mobile app promos — BannerType APP."
      createHref="/admin/banners/app/new"
      columns={[
        { key: "title", header: "Title" },
        { key: "active", header: "Active" },
      ]}
      rows={[{ title: "App exclusive", active: "Yes" }]}
    />
  );
}
