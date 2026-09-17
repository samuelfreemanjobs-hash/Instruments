"use client";

import { useState } from "react";
import Image from "next/image";

type Props = {
  label?: string;
  folder?: string;
  urls: string[];
  onChange: (urls: string[]) => void;
  multiple?: boolean;
};

export function CloudinaryUploadField({
  label = "Images",
  folder = "ecommerce/products",
  urls,
  onChange,
  multiple = true,
}: Props) {
  const [error, setError] = useState<string | null>(null);
  const [uploading, setUploading] = useState(false);

  async function handleFiles(fileList: FileList | null) {
    if (!fileList?.length) return;
    setError(null);
    setUploading(true);

    const files = multiple ? Array.from(fileList) : [fileList[0]];
    const next = [...urls];

    try {
      for (const file of files) {
        const signRes = await fetch("/api/uploads/cloudinary-sign", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ folder }),
        });
        const sign = (await signRes.json()) as {
          error?: string;
          cloudName?: string;
          apiKey?: string;
          timestamp?: number;
          signature?: string;
          folder?: string;
        };
        if (!signRes.ok) {
          throw new Error(sign.error ?? "Upload sign failed");
        }

        const data = new FormData();
        data.append("file", file);
        data.append("api_key", sign.apiKey!);
        data.append("timestamp", String(sign.timestamp));
        data.append("signature", sign.signature!);
        data.append("folder", sign.folder!);

        const uploadRes = await fetch(
          `https://api.cloudinary.com/v1_1/${sign.cloudName}/image/upload`,
          { method: "POST", body: data },
        );
        const uploaded = (await uploadRes.json()) as { secure_url?: string; error?: { message?: string } };
        if (!uploadRes.ok || !uploaded.secure_url) {
          throw new Error(uploaded.error?.message ?? "Upload failed");
        }
        next.push(uploaded.secure_url);
      }
      onChange(multiple ? next : [next[next.length - 1]!]);
    } catch (e) {
      setError(e instanceof Error ? e.message : "Upload failed");
    } finally {
      setUploading(false);
    }
  }

  function removeUrl(url: string) {
    onChange(urls.filter((u) => u !== url));
  }

  return (
    <div className="space-y-2">
      <span className="block text-sm font-medium text-slate-700">{label}</span>
      {urls.length > 0 ? (
        <ul className="flex flex-wrap gap-2">
          {urls.map((url) => (
            <li key={url} className="relative h-20 w-20 overflow-hidden rounded border">
              <Image src={url} alt="" fill className="object-cover" sizes="80px" unoptimized />
              <button
                type="button"
                onClick={() => removeUrl(url)}
                className="absolute right-0 top-0 bg-black/60 px-1 text-xs text-white"
              >
                ×
              </button>
            </li>
          ))}
        </ul>
      ) : null}
      <input
        type="file"
        accept="image/*"
        multiple={multiple}
        disabled={uploading}
        onChange={(e) => void handleFiles(e.target.files)}
        className="block w-full text-sm"
      />
      {uploading ? <p className="text-xs text-slate-500">Uploading…</p> : null}
      {error ? <p className="text-xs text-red-600">{error}</p> : null}
      {!multiple && urls[0] ? (
        <input type="hidden" name="imageUrl" value={urls[0]} readOnly />
      ) : null}
    </div>
  );
}
