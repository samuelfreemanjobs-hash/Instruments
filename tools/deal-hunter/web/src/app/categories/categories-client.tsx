"use client";

import { useEffect, useState } from "react";
import Link from "next/link";

import { fetchCategories } from "@/lib/deal-api";
import { Skeleton } from "@/components/ui/skeleton";
import { EmptyState } from "@/components/ui/empty-state";

export function CategoriesClient() {
  const [data, setData] = useState<Awaited<ReturnType<typeof fetchCategories>> | null>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    void fetchCategories()
      .then(setData)
      .catch(() => setData(null))
      .finally(() => setLoading(false));
  }, []);

  if (loading) return <Skeleton className="h-40 w-full" />;
  if (!data) return <EmptyState title="Categories offline" description="Start the deal API." />;

  return (
    <div className="grid gap-6 sm:grid-cols-2">
      <section>
        <h3 className="mb-2 text-sm font-medium">Categories</h3>
        <ul className="flex flex-wrap gap-2">
          {data.categories.map((c) => (
            <li key={c}>
              <Link
                href={`/search?category=${encodeURIComponent(c)}`}
                className="rounded-full border border-bfm-border px-3 py-1 text-sm text-bfm-muted hover:border-bfm-accent hover:text-white"
              >
                {c}
              </Link>
            </li>
          ))}
        </ul>
      </section>
      <section>
        <h3 className="mb-2 text-sm font-medium">Retailers</h3>
        <ul className="flex flex-wrap gap-2">
          {data.retailers.map((r) => (
            <li key={r}>
              <Link
                href={`/search?retailer=${encodeURIComponent(r)}`}
                className="rounded-full border border-bfm-border px-3 py-1 text-sm text-bfm-muted hover:border-bfm-accent hover:text-white"
              >
                {r}
              </Link>
            </li>
          ))}
        </ul>
      </section>
    </div>
  );
}
