import type { Deal } from "../types";
import { CONDITION_LABELS, DEAL_TYPE_LABELS } from "../types";
import { IntelligenceBadge } from "./IntelligenceBadge";
import { FinalPricePanel } from "./FinalPricePanel";
import { RealSavingsStrip } from "./RealSavingsStrip";

interface Props {
  deal: Deal;
  stealThreshold: number;
  onWatch: (deal: Deal) => void;
  onAddList: (deal: Deal) => void;
  isWatched: boolean;
  inList: boolean;
  compact?: boolean;
  showStudent?: boolean;
}

export function DealCard({
  deal,
  stealThreshold,
  onWatch,
  onAddList,
  isWatched,
  inList,
  compact,
  showStudent,
}: Props) {
  const isSteal = deal.stealScore >= stealThreshold;
  const canOpen = deal.trust?.safeUrl !== false;

  return (
    <article className={`deal-card${isSteal ? " steal" : ""}${compact ? " compact" : ""}`}>
      <img src={deal.image} alt="" loading="lazy" />
      <div className="deal-body">
        <div className="deal-meta">
          <span className="badge">{DEAL_TYPE_LABELS[deal.dealType]}</span>
          <span className="badge">{CONDITION_LABELS[deal.itemCondition]}</span>
          {isSteal && <span className="badge steal">Top pick · {deal.stealScore}</span>}
          {deal.priceDropped && <span className="badge drop">Price drop</span>}
          {deal.freeShipping && <span className="badge ship">Free shipping</span>}
          {deal.returnPolicy && (
            <span className="badge return" title={deal.returnPolicy.summary}>
              {deal.returnPolicy.days <= 30 ? "30-day returns" : deal.returnPolicy.days <= 60 ? "60-day returns" : `${deal.returnPolicy.days}-day returns`}
            </span>
          )}
          {deal.coupon?.verified && <span className="badge coupon">Coupon ready</span>}
        </div>
        <IntelligenceBadge intel={deal.intelligence} />
        <RealSavingsStrip deal={deal} />
        <h3 className="deal-title">{deal.title}</h3>
        {deal.identity?.brand && (
          <p className="deal-retailer">
            {deal.identity.brand}
            {deal.identity.model ? ` · ${deal.identity.model}` : ""}
            {deal.identity.upc ? ` · UPC ${deal.identity.upc}` : ""}
          </p>
        )}
        <p className="deal-retailer">
          {deal.retailer}
          {deal.pricing
            ? ` · Save $${deal.pricing.totalSavedVsMsrp.toFixed(2)} all-in`
            : ` · You save $${deal.savings.toFixed(2)}`}
        </p>
        <FinalPricePanel deal={deal} showStudent={showStudent} />
        {deal.pricing?.coupon?.verificationStatus === "verified" && (
          <div className="coupon-strip">
            <code>{deal.pricing.coupon.code}</code>
            <span>{deal.pricing.coupon.description}</span>
            <button
              type="button"
              className="btn-copy"
              onClick={() => void navigator.clipboard.writeText(deal.pricing!.coupon!.code)}
            >
              Copy
            </button>
          </div>
        )}
        <div className="card-actions">
          {canOpen ? (
            <a className="btn-primary" href={deal.url} target="_blank" rel="noreferrer noopener">
              Official store
            </a>
          ) : (
            <span className="btn-blocked">Link blocked — not a verified store</span>
          )}
          {!compact && (
            <>
              <button
                type="button"
                className="btn-ghost"
                onClick={() => onWatch(deal)}
                aria-pressed={isWatched}
              >
                {isWatched ? "Watching" : "Watch"}
              </button>
              <button
                type="button"
                className="btn-ghost"
                onClick={() => onAddList(deal)}
                disabled={inList}
              >
                {inList ? "On list" : "+ List"}
              </button>
            </>
          )}
        </div>
      </div>
    </article>
  );
}

