import { PageHeader } from "@/components/admin/PageHeader";

export default function TopBarPage() {
  return (
    <>
      <PageHeader title="Top Bar" description="Sitewide announcement bar (single active record)." />
      <form className="max-w-xl space-y-4 rounded-lg border border-slate-200 bg-white p-6 shadow-sm">
        <label className="block text-sm font-medium text-slate-700">
          Message
          <input
            className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
            defaultValue="Free shipping on orders over $50"
            name="message"
          />
        </label>
        <label className="block text-sm font-medium text-slate-700">
          Link (optional)
          <input className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2" name="link" />
        </label>
        <button type="button" className="rounded-md bg-slate-900 px-4 py-2 text-sm text-white">
          Save
        </button>
      </form>
    </>
  );
}
