import { PageHeader } from "@/components/admin/PageHeader";
import { BannerForm } from "@/components/admin/BannerForm";

export default function NewAppBannerPage() {
  return (
    <>
      <PageHeader title="New app banner" description="For future mobile app promos." />
      <BannerForm type="APP" />
    </>
  );
}
