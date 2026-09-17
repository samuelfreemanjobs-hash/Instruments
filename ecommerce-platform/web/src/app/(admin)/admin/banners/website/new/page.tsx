import { PageHeader } from "@/components/admin/PageHeader";
import { BannerForm } from "@/components/admin/BannerForm";

export default function NewWebsiteBannerPage() {
  return (
    <>
      <PageHeader title="New website banner" description="Upload via Cloudinary signed upload." />
      <BannerForm type="WEBSITE" />
    </>
  );
}
