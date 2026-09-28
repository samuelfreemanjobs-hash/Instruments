import type { KitManifest } from "@/lib/manifest";

function supportsDirectoryPicker(): boolean {
  return typeof window !== "undefined" && "showDirectoryPicker" in window;
}

/** WO-SAAS-016: write kit WAVs into a user-chosen folder (Chrome / Edge). */
export async function exportKitToFolder(manifest: KitManifest): Promise<void> {
  if (!supportsDirectoryPicker()) {
    throw new Error("folder_picker_unsupported");
  }

  const root = await (
    window as unknown as {
      showDirectoryPicker: () => Promise<FileSystemDirectoryHandle>;
    }
  ).showDirectoryPicker();

  const kitDir = await root.getDirectoryHandle(
    `disklordz-${manifest.presetId}-${manifest.kitId.slice(0, 8)}`,
    { create: true },
  );

  const manifestFile = await kitDir.getFileHandle("manifest.json", { create: true });
  const writable = await manifestFile.createWritable();
  await writable.write(JSON.stringify(manifest, null, 2));
  await writable.close();

  for (const sample of manifest.samples) {
    const res = await fetch(sample.url);
    if (!res.ok) {
      throw new Error(`fetch_failed:${sample.name}`);
    }
    const buf = await res.arrayBuffer();
    const parts = sample.filename.split("/");
    let dir = kitDir;
    for (let i = 0; i < parts.length - 1; i++) {
      dir = await dir.getDirectoryHandle(parts[i], { create: true });
    }
    const file = await dir.getFileHandle(parts[parts.length - 1], { create: true });
    const out = await file.createWritable();
    await out.write(buf);
    await out.close();
  }
}

export { supportsDirectoryPicker };
