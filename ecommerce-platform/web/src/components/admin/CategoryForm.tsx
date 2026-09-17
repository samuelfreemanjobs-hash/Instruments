"use client";

import { useRouter } from "next/navigation";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { categorySchema, type CategoryInput } from "@/lib/validators/category";
import { createCategory, updateCategory } from "@/lib/actions/categories";
import { CloudinaryUploadField } from "@/components/admin/CloudinaryUploadField";
import { useState } from "react";

export function CategoryForm({
  defaultValues,
  categoryId,
}: {
  defaultValues?: Partial<CategoryInput>;
  categoryId?: string;
}) {
  const router = useRouter();
  const [serverError, setServerError] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    setValue,
    watch,
    formState: { errors, isSubmitting },
  } = useForm<CategoryInput>({
    resolver: zodResolver(categorySchema),
    defaultValues: {
      name: defaultValues?.name ?? "",
      slug: defaultValues?.slug ?? "",
      image: defaultValues?.image ?? "",
    },
  });

  const imageUrl = watch("image") ?? "";
  const imageUrls = imageUrl ? [imageUrl] : [];

  async function onSubmit(data: CategoryInput) {
    setServerError(null);
    const result = categoryId
      ? await updateCategory(categoryId, data)
      : await createCategory(data);
    if (!result.ok) {
      setServerError(result.error);
      return;
    }
    router.push("/admin/categories");
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
        Name
        <input
          {...register("name")}
          className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
        />
        {errors.name ? <span className="text-xs text-red-600">{errors.name.message}</span> : null}
      </label>
      <label className="block text-sm font-medium text-slate-700">
        Slug (optional)
        <input
          {...register("slug")}
          placeholder="auto-generated from name"
          className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
        />
        {errors.slug ? <span className="text-xs text-red-600">{errors.slug.message}</span> : null}
      </label>
      <CloudinaryUploadField
        label="Category image (optional)"
        folder="ecommerce/categories"
        urls={imageUrls}
        multiple={false}
        onChange={(urls) => setValue("image", urls[0] ?? "", { shouldValidate: true })}
      />
      <input type="hidden" {...register("image")} />
      <button
        type="submit"
        disabled={isSubmitting}
        className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white disabled:opacity-60"
      >
        {isSubmitting ? "Saving…" : categoryId ? "Update category" : "Create category"}
      </button>
    </form>
  );
}
