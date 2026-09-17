import { AdminListStub } from "@/components/admin/AdminListStub";

export default function CouponsPage() {
  return (
    <AdminListStub
      title="Coupons"
      description="Discount codes — complete in SOP-04 with Zod + Prisma."
      createHref="/admin/coupons/new"
      createLabel="New coupon"
      columns={[
        { key: "code", header: "Code" },
        { key: "discount", header: "Discount" },
        { key: "active", header: "Active" },
      ]}
      rows={[{ code: "WELCOME10", discount: "10%", active: "Yes" }]}
    />
  );
}
