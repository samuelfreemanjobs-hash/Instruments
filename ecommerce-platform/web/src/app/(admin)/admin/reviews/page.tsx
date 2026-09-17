import { AdminListStub } from "@/components/admin/AdminListStub";

export default function ReviewsPage() {
  return (
    <AdminListStub
      title="Reviews"
      description="Moderate customer reviews before they appear on PDP."
      columns={[
        { key: "product", header: "Product" },
        { key: "rating", header: "Rating" },
        { key: "approved", header: "Approved" },
      ]}
      rows={[{ product: "Wireless Headphones", rating: "5", approved: "Pending" }]}
    />
  );
}
