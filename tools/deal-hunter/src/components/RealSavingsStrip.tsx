import type { Deal } from "../types";

/** Surfaces real savings vs retailer advertised discount — core product promise */
export function RealSavingsStrip({ deal }: { deal: Deal }) {
  const intel = deal.intelligence;
  const advertised = intel?.advertisedDiscountPct ?? deal.discountPct;
  const real = intel?.realSavingsVsAvgPct ?? deal.effectiveDiscountPct ?? deal.discountPct;
  const gap = intel?.discountHonestyGap ?? Math.max(0, advertised - real);
  const misleading = gap >= 8;

  return (
    <div className={`real-savings-strip${misleading ? " warn" : ""}`} title="Compared to 30-day tracked average">
      <div className="rss-col">
        <span className="rss-label">Advertised</span>
        <span className="rss-val muted">{advertised}% off tag</span>
      </div>
      <span className="rss-arrow" aria-hidden>
        →
      </span>
      <div className="rss-col highlight">
        <span className="rss-label">Real savings</span>
        <span className="rss-val">{real}% vs 30d avg</span>
      </div>
      {misleading && <span className="rss-flag">Gap {gap}% — trust final price</span>}
    </div>
  );
}
