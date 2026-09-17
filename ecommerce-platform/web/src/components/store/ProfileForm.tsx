"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { updateProfileAction } from "@/lib/actions/profile";

const schema = z.object({
  name: z.string().min(2, "Name is required"),
});

type FormValues = z.infer<typeof schema>;

export function ProfileForm({ defaultName, email }: { defaultName: string; email: string }) {
  const [message, setMessage] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
  } = useForm<FormValues>({
    resolver: zodResolver(schema),
    defaultValues: { name: defaultName },
  });

  async function onSubmit(data: FormValues) {
    setMessage(null);
    const result = await updateProfileAction(data);
    if (result.ok) setMessage("Profile updated");
    else setMessage(result.error);
  }

  return (
    <form onSubmit={handleSubmit(onSubmit)} className="mt-8 space-y-4">
      <label className="block text-sm font-medium text-slate-700">
        Email
        <input
          value={email}
          readOnly
          className="mt-1 w-full rounded-md border border-slate-200 bg-slate-50 px-3 py-2 text-slate-600"
        />
      </label>
      <label className="block text-sm font-medium text-slate-700">
        Name
        <input {...register("name")} className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2" />
        {errors.name ? <span className="text-xs text-red-600">{errors.name.message}</span> : null}
      </label>
      {message ? <p className="text-sm text-slate-600">{message}</p> : null}
      <button
        type="submit"
        disabled={isSubmitting}
        className="rounded-md bg-slate-900 px-4 py-2 text-sm text-white disabled:opacity-60"
      >
        Save
      </button>
    </form>
  );
}
