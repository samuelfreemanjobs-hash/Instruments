"use client";

import { useRouter } from "next/navigation";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { productSchema, type ProductInput } from "@/lib/validators/product";
import { createProduct, updateProduct } from "@/lib/actions/products";
import { CloudinaryUploadField } from "@/components/admin/CloudinaryUploadField";
import { useState } from "react";

type CategoryOption = { id: string; name: string };

export function ProductForm({
  categories,
  defaultValues,
  productId,
}: {
  categories: CategoryOption[];
  defaultValues?: Partial<ProductInput>;
  productId?: string;
}) {
  const router = useRouter();
  const [serverError, setServerError] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    setValue,
    watch,
    formState: { errors, isSubmitting },
  } = useForm<ProductInput>({
    resolver: zodResolver(productSchema),
    defaultValues: {
      title: defaultValues?.title ?? "",
      slug: defaultValues?.slug ?? "",
      description: defaultValues?.description ?? "",
      price: defaultValues?.price ?? 0,
      stock: defaultValues?.stock ?? 0,
      categoryId: defaultValues?.categoryId ?? categories[0]?.id ?? "",
      featured: defaultValues?.featured ?? false,
      images: defaultValues?.images ?? [],
    },
  });

  const images = watch("images") ?? [];

  async function onSubmit(data: ProductInput) {
    setServerError(null);
    const result = productId ? await updateProduct(productId, data) : await createProduct(data);
    if (!result.ok) {
      setServerError(result.error);
      return;
    }
    router.push("/admin/products");
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
      <label className="block text-sm font-medium text-slate-700">
        Slug (optional)
        <input {...register("slug")} className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2" />
      </label>
      <label className="block text-sm font-medium text-slate-700">
        Description
        <textarea
          {...register("description")}
          rows={4}
          className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
        />
        {errors.description ? (
          <span className="text-xs text-red-600">{errors.description.message}</span>
        ) : null}
      </label>
      <div className="grid grid-cols-2 gap-4">
        <label className="block text-sm font-medium text-slate-700">
          Price
          <input
            type="number"
            step="0.01"
            {...register("price", { valueAsNumber: true })}
            className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
          />
        </label>
        <label className="block text-sm font-medium text-slate-700">
          Stock
          <input
            type="number"
            {...register("stock", { valueAsNumber: true })}
            className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
          />
        </label>
      </div>
      <label className="block text-sm font-medium text-slate-700">
        Category
        <select
          {...register("categoryId")}
          className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2"
        >
          {categories.map((c) => (
            <option key={c.id} value={c.id}>
              {c.name}
            </option>
          ))}
        </select>
        {errors.categoryId ? (
          <span className="text-xs text-red-600">{errors.categoryId.message}</span>
        ) : null}
      </label>
      <label className="flex items-center gap-2 text-sm">
        <input type="checkbox" {...register("featured")} />
        Featured on home
      </label>
      <CloudinaryUploadField
        label="Product images (Cloudinary)"
        folder="ecommerce/products"
        urls={images}
        onChange={(urls) => setValue("images", urls, { shouldValidate: true })}
      />
      <label className="block text-sm font-medium text-slate-700">
        Or paste image URL
        <input
          className="mt-1 w-full rounded-md border border-slate-300 px-3 py-2 text-sm"
          placeholder="https://res.cloudinary.com/demo/..."
          onBlur={(e) => {
            const url = e.target.value.trim();
            if (url && !images.includes(url)) {
              setValue("images", [...images, url], { shouldValidate: true });
              e.target.value = "";
            }
          }}
        />
      </label>
      <p className="text-xs text-slate-500">
        Cloudinary upload needs env vars — see web/docs/CLOUDINARY.md
      </p>
      <button
        type="submit"
        disabled={isSubmitting || categories.length === 0}
        className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white disabled:opacity-60"
      >
        {isSubmitting ? "Saving…" : productId ? "Update product" : "Create product"}
      </button>
      {categories.length === 0 ? (
        <p className="text-sm text-amber-700">Create a category before adding products.</p>
      ) : null}
    </form>
  );
}
