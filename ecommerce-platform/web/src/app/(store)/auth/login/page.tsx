import Link from "next/link";
import { LoginForm } from "@/components/store/LoginForm";

export default async function LoginPage({
  searchParams,
}: {
  searchParams: Promise<{ next?: string }>;
}) {
  const { next } = await searchParams;

  return (
    <div className="mx-auto max-w-md px-4 py-16">
      <h1 className="text-2xl font-semibold">Sign in</h1>
      <p className="mt-2 text-sm text-slate-600">
        Seed users: admin@example.com / admin123 (admin), customer@example.com / customer123
      </p>
      <LoginForm nextPath={next} />
      <p className="mt-4 text-center text-sm">
        No account?{" "}
        <Link href="/auth/register" className="underline">
          Register
        </Link>
      </p>
    </div>
  );
}
