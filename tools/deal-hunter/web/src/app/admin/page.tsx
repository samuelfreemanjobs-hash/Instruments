import { AdminDashboardClient } from "./admin-client";

export default function AdminPage() {
  return (
    <section className="space-y-4">
      <h2 className="text-lg font-semibold">Operations console</h2>
      <p className="text-sm text-bfm-muted">PRD Part 3 — sync catalog, inspect adapter health, trigger jobs.</p>
      <AdminDashboardClient />
    </section>
  );
}
