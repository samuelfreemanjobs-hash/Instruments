import { useEffect, useState } from "react";
import { DealCard } from "../components/DealCard";
import { chatAssistant, evaluateTargetWatches } from "../lib/api";
import {
  loadAssistantLog,
  loadTargetWatches,
  saveAssistantLog,
  saveTargetWatches,
  type TargetPriceWatch,
} from "../lib/storage";
import type { Deal } from "../types";

const STARTERS = [
  "Watch MPC sampler until price reaches $299",
  "Should I buy the Dyson now or wait?",
  "Best Sephora deal with a real coupon",
  "Find Adidas Ultraboost under $150",
];

interface Props {
  stealThreshold: number;
  onWatch: (d: Deal) => void;
  onAddList: (d: Deal) => void;
  watchIds: Set<string>;
  listIds: Set<string>;
  showStudent?: boolean;
  onNotifyPermission: () => void;
}

export function PersonalAssistant(props: Props) {
  const [input, setInput] = useState("");
  const [log, setLog] = useState(() => loadAssistantLog());
  const [deals, setDeals] = useState<Deal[]>([]);
  const [intelLine, setIntelLine] = useState<string | null>(null);
  const [targetWatches, setTargetWatches] = useState<TargetPriceWatch[]>(() => loadTargetWatches());
  const [watchStatus, setWatchStatus] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const syncWatches = () => {
    const payload = targetWatches.map((w) => ({
      id: w.id,
      productQuery: w.productQuery,
      targetPrice: w.targetPrice,
    }));
    if (payload.length === 0) return;
    void evaluateTargetWatches(payload).then((res) => {
      const hit = res.results.find((r) => r.targetReached);
      if (hit) {
        setWatchStatus(
          `Target reached: ${hit.productQuery} ≤ $${hit.targetPrice} (${hit.participatingOffers.find((o) => o.atTarget)?.retailer ?? "store"})`,
        );
      }
    });
  };

  useEffect(() => {
    syncWatches();
    const id = setInterval(syncWatches, 60_000);
    return () => clearInterval(id);
  }, [targetWatches]);

  const send = async (text: string) => {
    const msg = text.trim();
    if (!msg) return;
    setLoading(true);
    const userEntry = { role: "user" as const, text: msg, at: new Date().toISOString() };
    const nextLog = [...log, userEntry];
    setLog(nextLog);
    setInput("");
    try {
      const res = await chatAssistant(
        msg,
        targetWatches.map((w) => ({
          id: w.id,
          productQuery: w.productQuery,
          targetPrice: w.targetPrice,
        })),
      );
      const assistantEntry = {
        role: "assistant" as const,
        text: res.message,
        at: res.generatedAt,
      };
      const full = [...nextLog, assistantEntry];
      setLog(full);
      saveAssistantLog(full);
      setDeals(res.deals as Deal[]);
      setIntelLine(res.intelligenceSummary ?? null);

      for (const action of res.actions) {
        if (action.type === "set_target_watch") {
          const watch: TargetPriceWatch = {
            id: `tw-${Date.now()}`,
            productQuery: action.productQuery,
            targetPrice: action.targetPrice,
            createdAt: new Date().toISOString(),
          };
          const updated = [...targetWatches.filter((w) => w.productQuery !== watch.productQuery), watch];
          setTargetWatches(updated);
          saveTargetWatches(updated);
          props.onNotifyPermission();
        }
        if (action.type === "add_to_list" && res.deals[0]) {
          props.onAddList(res.deals[0] as Deal);
        }
      }
    } finally {
      setLoading(false);
    }
  };

  return (
    <section className="assistant-page">
      <header className="assistant-hero">
        <p className="tagline">Your personal shopping assistant</p>
        <h2>Ask once — I search, compare, and watch prices for you</h2>
        <p className="panel-hint">
          Like Honey + CamelCamelCamel + a savvy friend: real savings vs averages, target-price alerts across
          participating retailers, buy/wait calls.
        </p>
      </header>

      <div className="starter-chips">
        {STARTERS.map((s) => (
          <button key={s} type="button" className="chip on" onClick={() => void send(s)}>
            {s}
          </button>
        ))}
      </div>

      <div className="chat-log" aria-live="polite">
        {log.length === 0 && (
          <p className="empty">Try: “Watch MPC sampler until $299” — we scan Guitar Center, Sweetwater, Best Buy, and more.</p>
        )}
        {log.map((entry, i) => (
          <div key={`${entry.at}-${i}`} className={`chat-bubble ${entry.role}`}>
            {entry.text}
          </div>
        ))}
      </div>

      {intelLine && <p className="intel-answer">{intelLine}</p>}
      {watchStatus && <p className="target-hit">{watchStatus}</p>}

      {targetWatches.length > 0 && (
        <div className="target-watch-list">
          <h3>Target price watches</h3>
          <ul>
            {targetWatches.map((w) => (
              <li key={w.id}>
                {w.productQuery} → ${w.targetPrice.toFixed(2)}
                <button
                  type="button"
                  className="btn-ghost"
                  onClick={() => {
                    const next = targetWatches.filter((x) => x.id !== w.id);
                    setTargetWatches(next);
                    saveTargetWatches(next);
                  }}
                >
                  Remove
                </button>
              </li>
            ))}
          </ul>
        </div>
      )}

      <form
        className="assistant-compose"
        onSubmit={(e) => {
          e.preventDefault();
          void send(input);
        }}
      >
        <input
          value={input}
          onChange={(e) => setInput(e.target.value)}
          placeholder="What should I hunt for you?"
          disabled={loading}
        />
        <button type="submit" className="btn-primary" disabled={loading}>
          {loading ? "Thinking…" : "Ask"}
        </button>
      </form>

      {deals.length > 0 && (
        <div className="deal-grid tight">
          {deals.map((deal) => (
            <DealCard
              key={deal.id}
              deal={deal}
              stealThreshold={props.stealThreshold}
              onWatch={props.onWatch}
              onAddList={props.onAddList}
              isWatched={props.watchIds.has(deal.id)}
              inList={props.listIds.has(deal.id)}
              compact
              showStudent={props.showStudent}
            />
          ))}
        </div>
      )}
    </section>
  );
}
