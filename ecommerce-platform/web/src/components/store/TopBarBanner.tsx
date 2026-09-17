import Link from "next/link";
import { getActiveTopBar } from "@/lib/catalog";

export async function TopBarBanner() {
  const topBar = await getActiveTopBar();
  if (!topBar) return null;

  const inner = (
    <span>
      {topBar.message}
      {topBar.link ? " →" : null}
    </span>
  );

  return (
    <div className="bg-slate-900 px-4 py-2 text-center text-sm text-white">
      {topBar.link ? (
        <Link href={topBar.link} className="hover:underline">
          {inner}
        </Link>
      ) : (
        inner
      )}
    </div>
  );
}
