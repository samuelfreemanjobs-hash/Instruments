"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { loginSchema, type LoginInput } from "@/lib/validators/auth";
import { loginAction } from "@/lib/actions/auth";

export function LoginForm({ nextPath }: { nextPath?: string }) {
  const [serverError, setServerError] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
  } = useForm<LoginInput>({
    resolver: zodResolver(loginSchema),
  });

  async function onSubmit(data: LoginInput) {
    setServerError(null);
    const result = await loginAction(data, nextPath);
    if (result && !result.ok) {
      setServerError(result.error);
    }
  }

  return (
    <form onSubmit={handleSubmit(onSubmit)} className="mt-8 space-y-4">
      {serverError ? (
        <p className="rounded-md bg-red-50 px-3 py-2 text-sm text-red-700">{serverError}</p>
      ) : null}
      <input
        type="email"
        placeholder="Email"
        {...register("email")}
        className="w-full rounded-md border border-slate-300 px-3 py-2"
      />
      {errors.email ? <p className="text-xs text-red-600">{errors.email.message}</p> : null}
      <input
        type="password"
        placeholder="Password"
        {...register("password")}
        className="w-full rounded-md border border-slate-300 px-3 py-2"
      />
      {errors.password ? <p className="text-xs text-red-600">{errors.password.message}</p> : null}
      <button
        type="submit"
        disabled={isSubmitting}
        className="w-full rounded-md bg-slate-900 py-2 text-white disabled:opacity-60"
      >
        {isSubmitting ? "Signing in…" : "Login"}
      </button>
    </form>
  );
}
