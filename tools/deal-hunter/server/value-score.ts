import type { ReturnPolicy } from "./return-policy.js";
import type { ScoredDeal } from "./rank.js";

export interface ValueScoreBreakdown {
  total: number;
  priceValue: number;
  stealComponent: number;
  trustComponent: number;
  returnComponent: number;
  shippingComponent: number;
  conditionPenalty: number;
  couponComponent: number;
}

export function computeValueScore(
  deal: ScoredDeal,
  peers: ScoredDeal[],
  returnPolicy: ReturnPolicy,
): ValueScoreBreakdown {
  const pay = deal.pricing?.finalPrice ?? deal.price;
  const prices = peers.map((p) => p.pricing?.finalPrice ?? p.price);
  const minP = Math.min(...prices, pay);
  const maxP = Math.max(...prices, pay);
  const spread = maxP - minP || 1;
  const priceValue = Math.round(((maxP - pay) / spread) * 35);

  const authenticityEstimate =
    (deal.trust?.safeUrl ? 12 : 0) +
    (deal.stealScore >= 75 ? 8 : deal.stealScore < 55 ? -6 : 0);
  const stealComponent = Math.round(deal.stealScore * 0.22);
  const trustComponent = deal.trust?.safeUrl ? 12 : 0;
  const returnComponent = Math.min(18, Math.round((returnPolicy.days / 60) * 18));
  const shippingComponent = deal.freeShipping ? 8 : 0;
  const conditionPenalty =
    deal.itemCondition === "used" ? 6 : deal.itemCondition === "open_box" ? 3 : 0;
  const couponComponent =
    deal.pricing?.coupon?.verificationStatus === "verified"
      ? 8
      : deal.coupon?.verified
        ? 4
        : 0;

  const total = Math.min(
    100,
    Math.max(
      0,
      priceValue +
        stealComponent +
        trustComponent +
        returnComponent +
        shippingComponent +
        couponComponent -
        conditionPenalty +
        authenticityEstimate,
    ),
  );

  return {
    total,
    priceValue,
    stealComponent,
    trustComponent,
    returnComponent,
    shippingComponent,
    conditionPenalty,
    couponComponent,
  };
}

export function explainBestValueWinner(
  winner: ScoredDeal,
  runnerUp: ScoredDeal | undefined,
  winnerScore: ValueScoreBreakdown,
  runnerScore: ValueScoreBreakdown | undefined,
  winnerReturn: ReturnPolicy,
): string {
  const wPay = winner.pricing?.finalPrice ?? winner.price;
  const parts = [
    `${winner.retailer} wins on overall value (score ${winnerScore.total}/100), not just sticker price.`,
    `You pay ~$${wPay.toFixed(2)} after eligible discounts; steal score ${winner.stealScore}; ${winnerReturn.days}-day return policy.`,
  ];
  if (runnerUp) {
    const rPay = runnerUp.pricing?.finalPrice ?? runnerUp.price;
    if (rPay < wPay) {
      parts.push(
        `${runnerUp.retailer} is $${(wPay - rPay).toFixed(2)} cheaper but scores ${runnerScore?.total ?? "?"} on value — weaker trust/returns/coupon stack or condition.`,
      );
    } else {
      parts.push(`${runnerUp.retailer} is the runner-up at $${rPay.toFixed(2)}.`);
    }
  }
  return parts.join(" ");
}
