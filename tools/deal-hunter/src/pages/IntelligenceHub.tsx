import { useEffect, useState } from "react";
import { IntelligencePanel } from "../components/IntelligenceBadge";
import { askIntelligence, fetchIntelligenceBriefing } from "../lib/api";
import type { IntelligenceBriefing } from "../types";
import { CategoryTrendChart } from "../components/CategoryTrendChart";
import { VERDICT_LABELS } from "../types";

export function IntelligenceHub() {
  const [briefing, setBriefing] = useState<IntelligenceBriefing | null>(null);
  const [question, setQuestion] = useState("");
  const [answer, setAnswer] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const refresh = () => {
    void fetchIntelligenceBriefing().then(setBriefing);
  };

  useEffect(() => {
    refresh();
    const id = setInterval(refresh, 120_000);
    return () => clearInterval(id);
  }, []);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!question.trim()) return;
    setLoading(true);
    try {
      const res = await askIntelligence(question);
      setAnswer(res.answer);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="intel-hub">
      <header className="intel-hub-header">
        <h2>AI deal intelligence</h2>
        <p>
          CamelCamelCamel-style price history, Honey-style coupons, and compare logic — in one engine.
          Verdicts use <strong>live</strong> prices from this refresh, not stale ads.
        </p>
        {briefing && (
          <p className="fresh-line">
            Data through {new Date(briefing.dataFreshThrough).toLocaleTimeString()} ·{" "}
            {briefing.priceDropsToday} drops today · real avg {briefing.avgRealSavingsPct ?? briefing.avgSavingsPct ?? 0}%
            vs 30d (advertised {briefing.avgAdvertisedDiscountPct ?? "—"}%)
          </p>
        )}
      </header>

      <form className="intel-ask" onSubmit={(e) => void submit(e)}>
        <input
          value={question}
          onChange={(e) => setQuestion(e.target.value)}
          placeholder="Should I buy the Dyson now or wait? Best Adidas deal?"
        />
        <button type="submit" className="btn-primary" disabled={loading}>
          Analyze
        </button>
      </form>
      {answer && <p className="intel-answer">{answer}</p>}

      {briefing && (
        <>
          <p className="briefing-head">{briefing.headline}</p>
          <ul className="pulse-list">
            {briefing.marketPulse.map((line) => (
              <li key={line}>{line}</li>
            ))}
          </ul>

          {briefing.categoryTrends && briefing.categoryTrends.length > 0 && (
            <CategoryTrendChart data={briefing.categoryTrends} />
          )}

          <section>
            <h3>Buy now</h3>
            <div className="intel-cards">
              {briefing.buyNow.map((intel) => (
                <div key={intel.dealId} className="intel-card-wrap">
                  <span className="mini-title">{intel.title}</span>
                  <IntelligencePanel intel={intel} />
                </div>
              ))}
            </div>
          </section>

          <section>
            <h3>Wait for a better price</h3>
            <ul className="wait-list">
              {briefing.waitList.map((w) => (
                <li key={w.dealId}>
                  <strong>{VERDICT_LABELS[w.verdict]}</strong> — {w.title} (${w.priceAnalysis.current.toFixed(2)})
                </li>
              ))}
            </ul>
          </section>
        </>
      )}
    </div>
  );
}
