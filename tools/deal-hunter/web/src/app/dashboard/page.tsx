import { DashboardClient } from "./dashboard-client";

export default function DashboardPage() {
  return (
    <div className="space-y-6">
      <section>
        <h2 className="text-2xl font-semibold">Today&apos;s best deals</h2>
        <p className="mt-1 text-bfm-muted">Every deal gets a score from 0 to 100. Higher is better.</p>
      </section>
      <DashboardClient />
    </div>
  );
}
