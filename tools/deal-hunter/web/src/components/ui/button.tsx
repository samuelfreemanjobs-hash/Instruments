import { cn } from "@/lib/utils";

type Props = React.ButtonHTMLAttributes<HTMLButtonElement> & {
  variant?: "default" | "outline" | "ghost";
};

export function Button({ className, variant = "default", ...props }: Props) {
  return (
    <button
      className={cn(
        "inline-flex min-h-10 items-center justify-center rounded-lg px-4 py-2 text-sm font-medium transition-colors disabled:opacity-50",
        variant === "default" && "bg-bfm-accent text-white hover:bg-bfm-accent/90",
        variant === "outline" && "border border-bfm-border bg-transparent text-bfm-text hover:border-bfm-accent",
        variant === "ghost" && "text-bfm-muted hover:text-white",
        className,
      )}
      {...props}
    />
  );
}
