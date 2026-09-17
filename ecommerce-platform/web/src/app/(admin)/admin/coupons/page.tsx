import { PageHeader } from "@/components/admin/PageHeader";
import { DataTable } from "@/components/admin/DataTable";
import { prisma } from "@/lib/db";

export const dynamic = "force-dynamic";

export default async function CouponsPage() {
  let coupons: Awaited<ReturnType<typeof prisma.coupon.findMany>> = [];
  try {
    coupons = await prisma.coupon.findMany({ orderBy: { code: "asc" } });
  } catch {
    coupons = [];
  }

  return (
    <>
      <PageHeader
        title="Coupons"
        description="Discount codes stored in MongoDB (seed includes WELCOME10)."
      />
      <DataTable
        columns={[
          { key: "code", header: "Code" },
          {
            key: "discount",
            header: "Discount",
            render: (row) =>
              row.discountPct != null
                ? `${row.discountPct}%`
                : row.discountAmt != null
                  ? `$${row.discountAmt}`
                  : "—",
          },
          { key: "active", header: "Active", render: (row) => (row.active ? "Yes" : "No") },
        ]}
        rows={coupons}
      />
    </>
  );
}
