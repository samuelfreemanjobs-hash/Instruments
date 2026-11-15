"use client";

import { Bar, BarChart, CartesianGrid, ResponsiveContainer, Tooltip, XAxis, YAxis } from "recharts";

export function CategoryTrendChart({
  data,
}: {
  data: { category: string; avgRealSavingsPct: number; dealCount: number }[];
}) {
  if (!data.length) return null;
  return (
    <div className="rounded-xl border border-bfm-border bg-bfm-card p-4">
      <p className="text-sm font-medium">Real savings by category (% vs 30d avg)</p>
      <ResponsiveContainer width="100%" height={220}>
        <BarChart data={data} margin={{ top: 12, right: 8, left: -8, bottom: 0 }}>
          <CartesianGrid stroke="#283244" strokeDasharray="3 3" vertical={false} />
          <XAxis dataKey="category" tick={{ fill: "#a6b0c0", fontSize: 11 }} />
          <YAxis tick={{ fill: "#a6b0c0", fontSize: 11 }} unit="%" />
          <Tooltip
            contentStyle={{ background: "#171d29", border: "1px solid #283244", borderRadius: 8 }}
            formatter={(v: number) => [`${v}%`, "real savings"]}
          />
          <Bar dataKey="avgRealSavingsPct" fill="#2878ff" radius={[4, 4, 0, 0]} />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
}
