import { AdminListStub } from "@/components/admin/AdminListStub";
import { mockOrders } from "@/lib/mock/admin-mock-data";

export default function OrdersPage() {
  return (
    <AdminListStub
      title="Orders"
      description="Order fulfillment — update status in SOP-04."
      columns={[
        { key: "id", header: "Order" },
        { key: "customer", header: "Customer" },
        { key: "total", header: "Total" },
        { key: "status", header: "Status" },
      ]}
      rows={mockOrders.map((o) => ({ ...o, total: `$${o.total}` }))}
    />
  );
}
