import { PageHeader } from "@/components/admin/PageHeader";
import { mockKpis } from "@/lib/mock/admin-mock-data";
import { formatPrice } from "@/lib/utils";

const cards = [
  { label: "Revenue", value: formatPrice(mockKpis.revenue) },
  { label: "Orders", value: mockKpis.orders.toLocaleString() },
  { label: "Customers", value: mockKpis.customers.toLocaleString() },
  { label: "Products", value: mockKpis.products.toLocaleString() },
];

export default function AdminDashboardPage() {
  return (
    <>
      <PageHeader
        title="Dashboard"
        description="Sales overview — wire to live analytics in SOP-04 (Phase 4)."
      />
      <div className="grid gap-4 sm:grid-cols-2 xl:grid-cols-4">
        {cards.map((c) => (
          <div key={c.label} className="rounded-lg border border-slate-200 bg-white p-5 shadow-sm">
            <p className="text-sm text-slate-500">{c.label}</p>
            <p className="mt-2 text-2xl font-semibold text-slate-900">{c.value}</p>
          </div>
        ))}
      </div>
    </>
  );
}
