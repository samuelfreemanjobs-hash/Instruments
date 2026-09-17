"use client";

import { useRouter } from "next/navigation";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { BannerType } from "@prisma/client";
import { bannerSchema, type BannerInput } from "@/lib/validators/banner";
import { createBanner } from "@/lib/actions/banners";
import { CloudinaryUploadField } from "@/components/admin/CloudinaryUploadField";
import { useState } from "react";

export function BannerForm({ type }: { type: BannerType }) {
  const router = useRouter();
  const [serverError, setServerError] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    setValue,
    watch,
    formState: { errors, isSubmitting },
  } = useForm<BannerInput>({
    resolver: zodResolver(bannerSchema),
    defaultValues: { active: true, sortOrder: 0, image: "" },
  });

  const image = watch("image") ?? "";
  const imageUrls = image ? [image] : [];

  async function onSubmit(data: BannerInput) {
    setServerError(null);
    const result = await createBanner(type, data);
    if (!result.ok) {
      setServerError(result.error);
      return;
    }
    router.push(type === "WEBSITE" ? "/admin/banners/website" : "/admin/banners/app");
    router.refresh();
  }

  return (
    <form
      onSubmit={handleSubmit(onSubmit)}
      className="max-w-xl space-y-4 rounded-lg border border-slate-200 bg-white p-6 shadow-sm"
    >
      {serverError ? (
        <p className="rounded-md bg-red-50 px-3 py-2 text-sm text-red-700">{serverError}</p>
      ) : null}
      <label className="block text-sm font-medium text-slate-700">
        Title
        <input {...register("title")} className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2" />
        {errors.title ? <span className="text-xs text-red-600">{errors.title.message}</span> : null}
      </label>
      <CloudinaryUploadField
        label="Banner image"
        folder={`ecommerce/banners/${type.toLowerCase()}`}
        urls={imageUrls}
        multiple={false}
        onChange={(urls) => setValue("image", urls[0] ?? "", { shouldValidate: true })}
      />
      <input type="hidden" {...register("image")} />
      {errors.image ? <span className="text-xs text-red-600">{errors.image.message}</span> : null}
      <label className="block text-sm font-medium text-slate-700">
        Link (optional)
        <input {...register("link")} className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2" />
      </label>
      <label className="flex items-center gap-2 text-sm">
        <input type="checkbox" {...register("active")} />
        Active
      </label>
      <button
        type="submit"
        disabled={isSubmitting}
        className="rounded-md bg-slate-900 px-4 py-2 text-sm text-white disabled:opacity-60"
      >
        Create banner
      </button>
    </form>
  );
}
