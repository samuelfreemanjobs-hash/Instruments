import { CategoriesClient } from "./categories-client";

export default function CategoriesPage() {
  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Categories</h2>
        <p className="text-sm text-bfm-muted">Browse by category or retailer — opens Universal Search with filters.</p>
      </header>
      <CategoriesClient />
    </section>
  );
}
