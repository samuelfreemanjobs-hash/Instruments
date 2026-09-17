"use client";

import { useRouter } from "next/navigation";
import { useState } from "react";

export function DeleteRowButton({
  label,
  onDelete,
}: {
  label: string;
  onDelete: () => Promise<{ ok: boolean; error?: string }>;
}) {
  const router = useRouter();
  const [error, setError] = useState<string | null>(null);
  const [pending, setPending] = useState(false);

  async function handleClick() {
    if (!confirm(`Delete ${label}?`)) return;
    setPending(true);
    setError(null);
    const result = await onDelete();
    setPending(false);
    if (!result.ok) {
      setError(result.error ?? "Delete failed");
      return;
    }
    router.refresh();
  }

  return (
    <span className="inline-flex flex-col items-end gap-1">
      <button
        type="button"
        onClick={handleClick}
        disabled={pending}
        className="text-xs text-red-600 hover:underline disabled:opacity-50"
      >
        {pending ? "…" : "Delete"}
      </button>
      {error ? <span className="text-xs text-red-600">{error}</span> : null}
    </span>
  );
}
