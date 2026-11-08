import { SearchClient } from "./search-client";

export default function SearchPage() {
  return (
    <section className="space-y-3">
      <h2 className="text-lg font-semibold">Universal product search</h2>
      <p className="text-sm text-bfm-muted">Authorized retailer catalog — real savings ranked per refresh.</p>
      <SearchClient />
    </section>
  );
}
