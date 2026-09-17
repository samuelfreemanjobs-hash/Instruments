import { PageHeader } from "@/components/admin/PageHeader";

export default function AnalyticsPage() {
  return (
    <>
      <PageHeader
        title="Analytics"
        description="Charts from Order aggregates — replace placeholders in SOP-04."
      />
      <div className="grid gap-4 lg:grid-cols-2">
        <div className="flex h-64 items-center justify-center rounded-lg border border-dashed border-slate-300 bg-white text-slate-500">
          Revenue chart (template: analytics-page.template.tsx)
        </div>
        <div className="flex h-64 items-center justify-center rounded-lg border border-dashed border-slate-300 bg-white text-slate-500">
          Top products chart
        </div>
      </div>
    </>
  );
}
