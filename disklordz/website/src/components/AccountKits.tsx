type KitRow = {
  kit_id: string;
  prompt: string;
  preset_id: string;
  artist_lane: string;
  created_at: string;
};

export function AccountKits({ kits }: { kits: KitRow[] }) {
  if (kits.length === 0) {
    return (
      <p className="rounded-xl border border-zinc-800 bg-zinc-900/40 px-4 py-6 text-sm text-zinc-400">
        No saved kits yet. Generate one while signed in — it auto-saves.
      </p>
    );
  }

  return (
    <ul className="space-y-2">
      {kits.map((kit) => (
        <li
          key={kit.kit_id}
          className="rounded-xl border border-zinc-800 bg-zinc-950 px-4 py-3"
        >
          <p className="font-medium text-zinc-100">{kit.prompt}</p>
          <p className="mt-1 text-xs text-zinc-500">
            {kit.preset_id} · {kit.artist_lane} ·{" "}
            {new Date(kit.created_at).toLocaleString()}
          </p>
          <p className="mt-1 font-mono text-xs text-zinc-600">{kit.kit_id}</p>
        </li>
      ))}
    </ul>
  );
}
