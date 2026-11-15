import Link from "next/link";

export default function NotFound() {
  return (
    <div className="mx-auto max-w-xl px-4 py-20 text-center">
      <h1 className="font-display text-4xl text-ivory">Page not found</h1>
      <p className="mt-3 text-ivory/70">That route is not part of the store.</p>
      <Link href="/" className="mt-6 inline-flex min-h-11 items-center text-gold underline">
        Back to DiskLordz
      </Link>
    </div>
  );
}
