import { useState } from "react";
import { runPricingAgent } from "../lib/api";
import type { Deal } from "../types";

export function PricingAgentPage() {
  const [question, setQuestion] = useState("");
  const [verdict, setVerdict] = useState<Awaited<ReturnType<typeof runPricingAgent>> | null>(null);
  const [loading, setLoading] = useState(false);

  const go = async () => {
    setLoading(true);
    try {
      setVerdict(await runPricingAgent(question));
    } finally {
      setLoading(false);
    }
  };

  return (
    <section className="panel agent-page">
      <h2>Pricing AI assistant</h2>
      <p className="panel-hint">Compares live listings, coupons, and shipping — use with Intelligence for buy/wait calls.</p>
      <div className="agent-form">
        <input
          value={question}
          onChange={(e) => setQuestion(e.target.value)}
          placeholder="Best Ultraboost deal? Sephora vs Ulta serum?"
        />
        <button type="button" className="btn-primary" onClick={() => void go()} disabled={loading}>
          Find best deal
        </button>
      </div>
      {verdict && <PricingVerdict verdict={verdict} />}
    </section>
  );
}

function PricingVerdict({ verdict }: { verdict: Awaited<ReturnType<typeof runPricingAgent>> }) {
  const best = verdict.bestPick as Deal | null;
  return (
    <div className="pricing-verdict">
      <p>{verdict.summary}</p>
      {verdict.valueVerdict && <p className="intel-answer">{verdict.valueVerdict}</p>}
      <p>{verdict.savingsBreakdown}</p>
      {verdict.bestValueScore > 0 && (
        <p className="fresh-line">Value score {verdict.bestValueScore}/100 (price, coupons, returns, trust — not cost alone).</p>
      )}
      <p>{verdict.couponTip}</p>
      <p className="fresh-line">Confidence: {verdict.confidence} · {verdict.analyzedAt}</p>
      {best && (
        <p>
          <a href={best.url} target="_blank" rel="noreferrer">
            Open {best.retailer}
          </a>
        </p>
      )}
    </div>
  );
}
