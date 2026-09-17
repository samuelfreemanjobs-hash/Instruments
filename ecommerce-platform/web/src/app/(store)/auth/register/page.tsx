import Link from "next/link";
import { RegisterForm } from "@/components/store/RegisterForm";

export default function RegisterPage() {
  return (
    <div className="mx-auto max-w-md px-4 py-16">
      <h1 className="text-2xl font-semibold">Create account</h1>
      <p className="mt-2 text-sm text-slate-600">New accounts are customers (storefront + profile).</p>
      <RegisterForm />
      <p className="mt-4 text-center text-sm">
        <Link href="/auth/login" className="underline">
          Back to login
        </Link>
      </p>
    </div>
  );
}
