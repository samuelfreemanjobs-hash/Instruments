import { AdminListStub } from "@/components/admin/AdminListStub";
import { mockProducts } from "@/lib/mock/admin-mock-data";
import { formatPrice } from "@/lib/utils";

export default function ProductsPage() {
  return (
    <AdminListStub
      title="All Products"
      description="Catalog — wire to Product model (SOP-04)."
      createHref="/admin/products/new"
      createLabel="Create product"
      columns={[
        { key: "title", header: "Title" },
        { key: "price", header: "Price" },
        { key: "stock", header: "Stock" },
      ]}
      rows={mockProducts.map((p) => ({ ...p, price: formatPrice(p.price) }))}
    />
  );
}
