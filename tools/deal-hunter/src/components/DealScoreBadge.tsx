import type { DealIntelligence } from "../types";

export function DealScoreBadge({ intel }: { intel?: DealIntelligence }) {
  const score = intel?.dealIntelligenceScore;
  if (!score) return null;

  return (
    <div className={`deal-score-badge ${score.classification}`} title={score.explanation}>
      <span className="dsb-score">{score.score}</span>
      <span className="dsb-label">{score.classificationLabel}</span>
      <span className="dsb-conf">{score.confidence}% conf</span>
    </div>
  );
}
