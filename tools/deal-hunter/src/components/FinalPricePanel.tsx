import type { Deal } from "../types";

interface Props {
  deal: Deal;
  showStudent?: boolean;
}

export function FinalPricePanel({ deal, showStudent }: Props) {
  const p = deal.pricing;
  if (!p) {
    return (
      <div className="price-row">
        <span className="price-now">${deal.price.toFixed(2)}</span>
        <span className="price-was">${deal.msrp.toFixed(2)}</span>
      </div>
    );
  }

  const pay = showStudent && p.finalPriceWithStudent != null ? p.finalPriceWithStudent : p.finalPrice;
  const lines = showStudent && p.linesWithStudent ? p.linesWithStudent : p.lines;

  return (
    <div className="final-price-panel">
      <div className="price-row">
        <span className="price-now" title="After eligible discounts">
          You pay ${pay.toFixed(2)}
        </span>
        <span className="price-was">Listed ${p.advertisedPrice.toFixed(2)}</span>
        <span className="price-was">MSRP ${deal.msrp.toFixed(2)}</span>
      </div>
      <ul className="price-breakdown">
        {lines
          .filter((l) => l.kind !== "total")
          .map((l) => (
            <li key={l.label}>
              {l.label}: {l.amount < 0 ? `-$${Math.abs(l.amount).toFixed(2)}` : `$${l.amount.toFixed(2)}`}
            </li>
          ))}
      </ul>
      {p.coupon && (
        <p className={`coupon-verify coupon-${p.coupon.verificationStatus}`}>
          Coupon {p.coupon.code}: {p.coupon.verificationStatus}
          {p.coupon.expiresInHours != null && p.coupon.expiresInHours > 0
            ? ` · expires in ~${p.coupon.expiresInHours}h`
            : ""}
          . {p.coupon.successNote}
        </p>
      )}
      {p.bundle && <p className="bundle-note">Bundle: {p.bundle.description}</p>}
      {p.studentEligible && (
        <p className="student-note">
          Student discount eligible
          {p.finalPriceWithStudent != null ? ` — you pay $${p.finalPriceWithStudent.toFixed(2)} with ID` : ""}
        </p>
      )}
    </div>
  );
}
