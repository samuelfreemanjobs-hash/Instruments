import type { DealIntelligence, IntelligenceVerdict } from "../types";
import { VERDICT_LABELS } from "../types";

const VERDICT_CLASS: Record<IntelligenceVerdict, string> = {
  buy_now: "verdict-buy",
  good_deal: "verdict-good",
  wait: "verdict-wait",
  skip: "verdict-skip",
};

export function IntelligenceBadge({ intel }: { intel?: DealIntelligence }) {
  if (!intel) return null;
  return (
    <div className={`intel-badge ${VERDICT_CLASS[intel.verdict]}`}>
      <span className="intel-verdict">{VERDICT_LABELS[intel.verdict]}</span>
      <span className="intel-score">{intel.intelligenceScore} IQ</span>
    </div>
  );
}

export function IntelligencePanel({ intel }: { intel: DealIntelligence }) {
  return (
    <aside className="intel-panel">
      <IntelligenceBadge intel={intel} />
      <p className="intel-headline">{intel.verdictHeadline}</p>
      <p className="intel-meta">
        Confidence {intel.confidence}% · 30d low ${intel.priceAnalysis.lowest30d.toFixed(2)}
      </p>
      <ul className="intel-factors">
        {intel.factors.slice(0, 4).map((f) => (
          <li key={f.label} className={`factor-${f.impact}`}>
            <strong>{f.label}:</strong> {f.detail}
          </li>
        ))}
      </ul>
      {intel.bestValueVerdict && <p className="intel-headline">{intel.bestValueVerdict}</p>}
      <p className="intel-coupon">{intel.couponStrategy}</p>
    </aside>
  );
}
