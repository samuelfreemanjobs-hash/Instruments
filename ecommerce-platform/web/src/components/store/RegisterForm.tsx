"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { registerSchema, type RegisterInput } from "@/lib/validators/auth";
import { registerAction } from "@/lib/actions/auth";

export function RegisterForm() {
  const [serverError, setServerError] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
  } = useForm<RegisterInput>({
    resolver: zodResolver(registerSchema),
  });

  async function onSubmit(data: RegisterInput) {
    setServerError(null);
    const result = await registerAction(data);
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
        placeholder="Name"
        {...register("name")}
        className="w-full rounded-md border border-slate-300 px-3 py-2"
      />
      {errors.name ? <p className="text-xs text-red-600">{errors.name.message}</p> : null}
      <input
        type="email"
        placeholder="Email"
        {...register("email")}
        className="w-full rounded-md border border-slate-300 px-3 py-2"
      />
      {errors.email ? <p className="text-xs text-red-600">{errors.email.message}</p> : null}
      <input
        type="password"
        placeholder="Password (min 8 characters)"
        {...register("password")}
        className="w-full rounded-md border border-slate-300 px-3 py-2"
      />
      {errors.password ? <p className="text-xs text-red-600">{errors.password.message}</p> : null}
      <button
        type="submit"
        disabled={isSubmitting}
        className="w-full rounded-md bg-slate-900 py-2 text-white disabled:opacity-60"
      >
        {isSubmitting ? "Creating…" : "Register"}
      </button>
    </form>
  );
}
