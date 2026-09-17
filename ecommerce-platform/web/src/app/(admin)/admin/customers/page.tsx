import { AdminListStub } from "@/components/admin/AdminListStub";

export default function CustomersPage() {
  return (
    <AdminListStub
      title="Customers"
      description="Customer accounts — connect to User model (SOP-04)."
      columns={[
        { key: "email", header: "Email" },
        { key: "name", header: "Name" },
        { key: "role", header: "Role" },
      ]}
      rows={[
        { email: "jane@example.com", name: "Jane Doe", role: "CUSTOMER" },
        { email: "admin@example.com", name: "Admin", role: "ADMIN" },
      ]}
    />
  );
}
