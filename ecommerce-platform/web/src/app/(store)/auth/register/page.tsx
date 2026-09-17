import Link from "next/link";

export default function RegisterPage() {
  return (
    <div className="mx-auto max-w-md px-4 py-16">
      <h1 className="text-2xl font-semibold">Create account</h1>
      <form className="mt-8 space-y-4">
        <input placeholder="Name" className="w-full rounded-md border border-slate-300 px-3 py-2" />
        <input
          type="email"
          placeholder="Email"
          className="w-full rounded-md border border-slate-300 px-3 py-2"
        />
        <input
          type="password"
          placeholder="Password"
          className="w-full rounded-md border border-slate-300 px-3 py-2"
        />
        <button type="button" className="w-full rounded-md bg-slate-900 py-2 text-white">
          Register
        </button>
      </form>
      <p className="mt-4 text-center text-sm">
        <Link href="/auth/login" className="underline">
          Back to login
        </Link>
      </p>
    </div>
  );
}
