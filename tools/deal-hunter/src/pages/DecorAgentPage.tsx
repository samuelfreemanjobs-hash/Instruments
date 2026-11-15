import { useState } from "react";
import { DealCard } from "../components/DealCard";
import { runDecorAgent } from "../lib/api";
import type { Deal } from "../types";

interface Props {
  stealThreshold: number;
  onAddList: (d: Deal) => void;
  listIds: Set<string>;
}

export function DecorAgentPage(props: Props) {
  const [room, setRoom] = useState("living");
  const [style, setStyle] = useState("cozy");
  const [budget, setBudget] = useState("500");
  const [plan, setPlan] = useState<Awaited<ReturnType<typeof runDecorAgent>> | null>(null);
  const [loading, setLoading] = useState(false);

  const go = async () => {
    setLoading(true);
    try {
      const res = await runDecorAgent({
        room,
        style,
        budgetMax: budget ? Number(budget) : undefined,
      });
      setPlan(res);
    } finally {
      setLoading(false);
    }
  };

  return (
    <section className="panel agent-page">
      <h2>Interior decorating assistant</h2>
      <p className="panel-hint">One room at a time — picks trusted deals that fit your budget and style.</p>
      <div className="agent-form">
        <label>
          Room
          <select value={room} onChange={(e) => setRoom(e.target.value)}>
            <option value="living">Living room</option>
            <option value="bedroom">Bedroom</option>
            <option value="kitchen">Kitchen</option>
            <option value="office">Office</option>
            <option value="outdoor">Outdoor</option>
          </select>
        </label>
        <label>
          Style
          <input value={style} onChange={(e) => setStyle(e.target.value)} placeholder="modern, cozy, minimal…" />
        </label>
        <label>
          Max budget ($)
          <input type="number" value={budget} onChange={(e) => setBudget(e.target.value)} />
        </label>
        <button type="button" className="btn-primary" onClick={() => void go()} disabled={loading}>
          Plan room
        </button>
      </div>
      {plan && (
        <>
          <h3>{plan.headline}</h3>
          <p>Palette: {plan.palette.join(", ")}</p>
          <ul>{plan.tips.map((t: string) => <li key={t}>{t}</li>)}</ul>
          <p>{plan.listSuggestion}</p>
          <div className="deal-grid">
            {(plan.picks as Deal[]).map((deal) => (
              <DealCard
                key={deal.id}
                deal={deal}
                stealThreshold={props.stealThreshold}
                onWatch={() => undefined}
                onAddList={props.onAddList}
                isWatched={false}
                inList={props.listIds.has(deal.id)}
              />
            ))}
          </div>
        </>
      )}
    </section>
  );
}
