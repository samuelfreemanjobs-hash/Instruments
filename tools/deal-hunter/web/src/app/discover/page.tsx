import { DiscoverClient } from "./discover-client";

export default function DiscoverPage() {
  return (
    <section className="space-y-3">
      <h2 className="text-lg font-semibold">Discover deals</h2>
      <p className="text-sm text-bfm-muted">Community, local partner circulars, and personalized picks.</p>
      <DiscoverClient />
    </section>
  );
}
