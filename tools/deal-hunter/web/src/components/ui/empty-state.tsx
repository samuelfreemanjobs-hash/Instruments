import { cn } from "@/lib/utils";

export function EmptyState({
  title,
  description,
  className,
  action,
}: {
  title: string;
  description?: string;
  className?: string;
  action?: React.ReactNode;
}) {
  return (
    <div
      className={cn(
        "flex flex-col items-center justify-center rounded-xl border border-dashed border-bfm-border bg-bfm-bg-2/50 px-6 py-12 text-center",
        className,
      )}
    >
      <p className="text-base font-medium text-white">{title}</p>
      {description && <p className="mt-2 max-w-sm text-sm text-bfm-muted">{description}</p>}
      {action && <div className="mt-4">{action}</div>}
    </div>
  );
}
