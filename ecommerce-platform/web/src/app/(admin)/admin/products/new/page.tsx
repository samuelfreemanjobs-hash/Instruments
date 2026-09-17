import { PageHeader } from "@/components/admin/PageHeader";

export default function NewProductPage() {
  return (
    <>
      <PageHeader
        title="Create Product"
        description="Use React Hook Form + product Zod schema from templates/validators."
      />
      <form className="max-w-xl space-y-4 rounded-lg border border-slate-200 bg-white p-6 shadow-sm">
        <label className="block text-sm font-medium text-slate-700">
          Title
          <input
            className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
            placeholder="Product title"
            name="title"
          />
        </label>
        <label className="block text-sm font-medium text-slate-700">
          Price
          <input
            type="number"
            step="0.01"
            className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
            name="price"
          />
        </label>
        <button
          type="button"
          className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white"
        >
          Save (wire server action)
        </button>
      </form>
    </>
  );
}
