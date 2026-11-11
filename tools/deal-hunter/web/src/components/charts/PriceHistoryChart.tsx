"use client";

import { Line, LineChart, ResponsiveContainer, Tooltip, XAxis, YAxis } from "recharts";

export function PriceHistoryChart({ rows }: { rows: { price: number; recorded_at: string }[] }) {
  const data = [...rows]
    .reverse()
    .map((r) => ({
      t: new Date(r.recorded_at).toLocaleDateString(undefined, { month: "short", day: "numeric" }),
      price: r.price,
    }));

  if (data.length < 2) {
    return <p className="text-sm text-bfm-muted">Building price history — check back after more refreshes.</p>;
  }

  return (
    <ResponsiveContainer width="100%" height={200}>
      <LineChart data={data}>
        <XAxis dataKey="t" tick={{ fill: "#a6b0c0", fontSize: 10 }} />
        <YAxis tick={{ fill: "#a6b0c0", fontSize: 10 }} domain={["auto", "auto"]} />
        <Tooltip contentStyle={{ background: "#171d29", border: "1px solid #283244" }} />
        <Line type="monotone" dataKey="price" stroke="#00b8d9" strokeWidth={2} dot={false} />
      </LineChart>
    </ResponsiveContainer>
  );
}
