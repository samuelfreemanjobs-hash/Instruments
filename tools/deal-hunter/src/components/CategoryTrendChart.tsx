import {
  Bar,
  BarChart,
  CartesianGrid,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from "recharts";
import type { CategoryTrendPoint } from "../types";

export function CategoryTrendChart({ data }: { data: CategoryTrendPoint[] }) {
  if (!data.length) return null;

  return (
    <div className="chart-panel">
      <h3 className="chart-title">Real savings by category</h3>
      <p className="chart-sub">% below 30-day average — not shelf stickers</p>
      <ResponsiveContainer width="100%" height={220}>
        <BarChart data={data} margin={{ top: 8, right: 8, left: -12, bottom: 0 }}>
          <CartesianGrid stroke="var(--border)" strokeDasharray="3 3" vertical={false} />
          <XAxis
            dataKey="category"
            tick={{ fill: "var(--silver)", fontSize: 11 }}
            axisLine={{ stroke: "var(--border)" }}
          />
          <YAxis
            tick={{ fill: "var(--silver)", fontSize: 11 }}
            axisLine={{ stroke: "var(--border)" }}
            unit="%"
          />
          <Tooltip
            contentStyle={{
              background: "var(--surface)",
              border: "1px solid var(--border)",
              borderRadius: 8,
              color: "var(--text)",
            }}
            formatter={(v: number) => [`${v}% real savings`, "vs 30d avg"]}
          />
          <Bar dataKey="avgRealSavingsPct" fill="var(--electric)" radius={[6, 6, 0, 0]} />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
}
