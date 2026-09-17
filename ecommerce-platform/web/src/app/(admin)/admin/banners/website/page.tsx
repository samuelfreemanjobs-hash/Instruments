import Link from "next/link";
import { PageHeader } from "@/components/admin/PageHeader";
import { DataTable } from "@/components/admin/DataTable";
import { prisma } from "@/lib/db";

export const dynamic = "force-dynamic";

export default async function WebsiteBannersPage() {
  let banners: Awaited<ReturnType<typeof prisma.banner.findMany>> = [];
  try {
    banners = await prisma.banner.findMany({
      where: { type: "WEBSITE" },
      orderBy: { sortOrder: "asc" },
    });
  } catch {
    banners = [];
  }

  return (
    <>
      <PageHeader
        title="Website Banners"
        description="Homepage carousel — BannerType WEBSITE."
        action={
          <Link
            href="/admin/banners/website/new"
            className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white"
          >
            New banner
          </Link>
        }
      />
      <DataTable
        columns={[
          { key: "title", header: "Title" },
          { key: "active", header: "Active", render: (r) => (r.active ? "Yes" : "No") },
          { key: "sortOrder", header: "Sort" },
        ]}
        rows={banners}
      />
    </>
  );
}
