import { cn } from "@/lib/utils";

export function Input({ className, ...props }: React.InputHTMLAttributes<HTMLInputElement>) {
  return (
    <input
      className={cn(
        "flex min-h-10 w-full rounded-lg border border-bfm-border bg-bfm-bg-2 px-3 py-2 text-sm text-white placeholder:text-bfm-muted",
        className,
      )}
      {...props}
    />
  );
}
