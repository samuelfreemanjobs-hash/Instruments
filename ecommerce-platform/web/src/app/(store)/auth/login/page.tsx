import Link from "next/link";

export default function LoginPage() {
  return (
    <div className="mx-auto max-w-md px-4 py-16">
      <h1 className="text-2xl font-semibold">Sign in</h1>
      <p className="mt-2 text-sm text-slate-600">
        Custom auth — implement with templates/lib/auth-session.template.ts (SOP-06).
      </p>
      <form className="mt-8 space-y-4">
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
          Login
        </button>
      </form>
      <p className="mt-4 text-center text-sm">
        No account?{" "}
        <Link href="/auth/register" className="underline">
          Register
        </Link>
      </p>
    </div>
  );
}
