import type { CatalogDeal } from "./catalog.js";
import type { DealCoupon } from "./deal-enrich.js";

export type CouponVerificationStatus = "verified" | "expired" | "unverified";

export interface BundleDiscount {
  description: string;
  amount: number;
}

export interface PricingLine {
  label: string;
  amount: number;
  kind: "sale" | "coupon" | "student" | "bundle" | "total";
}

export interface PricingBreakdown {
  advertisedPrice: number;
  finalPrice: number;
  finalPriceWithStudent?: number;
  totalSavedVsMsrp: number;
  totalSavedVsAdvertised: number;
  lines: PricingLine[];
  linesWithStudent?: PricingLine[];
  coupon?: DealCoupon & {
    verificationStatus: CouponVerificationStatus;
    expiresInHours?: number;
    successNote: string;
  };
  studentEligible: boolean;
  studentDiscountApplied: boolean;
  bundle?: BundleDiscount;
}

const STUDENT_ELIGIBLE = new Set([
  "d6",
  "d12",
  "ad1",
  "ad2",
  "sp1",
  "c5",
  "mpc-gc",
  "Best Buy",
  "Adidas",
  "Apple",
]);

const STUDENT_PCT_BY_RETAILER: Record<string, number> = {
  Adidas: 15,
  "Best Buy": 10,
  Target: 0,
};

const BUNDLES: Partial<Record<string, BundleDiscount>> = {
  sp1: { description: "Rare Beauty trio bundle", amount: 6 },
  c1: { description: "LEGO 2-set bundle", amount: 12 },
  g2: { description: "Tide + Dawn pack", amount: 4.5 },
};

const COUPON_VALUE: Partial<Record<string, { pct?: number; fixed?: number }>> = {
  CYBERCLEAN25: { pct: 8 },
  SAVE10: { pct: 10 },
  FLASHSSD: { fixed: 15 },
  HOME15: { pct: 15 },
  PANTRY20: { pct: 20 },
  CLEAN5: { fixed: 5 },
  SHIPFREE: { fixed: 0 },
  EBAY15: { pct: 15 },
  SEPHORA20: { pct: 20 },
  ULTA21: { pct: 21 },
  ADIDAS15: { pct: 15 },
  BBDEAL50: { fixed: 50 },
};

function couponStatus(coupon: DealCoupon): {
  status: CouponVerificationStatus;
  expiresInHours?: number;
} {
  if (!coupon.verified) return { status: "unverified" };
  if (coupon.expiresAt) {
    const ms = new Date(coupon.expiresAt).getTime() - Date.now();
    if (ms <= 0) return { status: "expired" };
    return { status: "verified", expiresInHours: Math.round(ms / 3600000) };
  }
  return { status: "verified" };
}

export function computePricingBreakdown(
  deal: CatalogDeal & { coupon?: DealCoupon },
  opts?: { applyStudent?: boolean },
): PricingBreakdown {
  const applyStudent = opts?.applyStudent ?? false;
  let running = deal.price;
  const lines: PricingLine[] = [
    { label: "Advertised sale price", amount: deal.price, kind: "sale" },
  ];

  const bundle = BUNDLES[deal.id];
  if (bundle) {
    running = Math.max(0, Math.round((running - bundle.amount) * 100) / 100);
    lines.push({ label: `Bundle: ${bundle.description}`, amount: -bundle.amount, kind: "bundle" });
  }

  let couponMeta: PricingBreakdown["coupon"];
  if (deal.coupon) {
    const { status, expiresInHours } = couponStatus(deal.coupon);
    const val = COUPON_VALUE[deal.coupon.code];
    let couponSavings = 0;
    if (status === "verified" && val) {
      if (val.pct) couponSavings = Math.round(running * val.pct) / 100;
      if (val.fixed) couponSavings = val.fixed;
      running = Math.max(0, Math.round((running - couponSavings) * 100) / 100);
      lines.push({
        label: `Coupon ${deal.coupon.code}`,
        amount: -couponSavings,
        kind: "coupon",
      });
    }
    couponMeta = {
      ...deal.coupon,
      verificationStatus: status,
      expiresInHours,
      successNote:
        status === "verified"
          ? "Verified in retailer promo feed for this refresh."
          : status === "expired"
            ? "Expired — not included in final price."
            : "Unverified — confirm at checkout.",
    };
  }

  const studentEligible =
    STUDENT_ELIGIBLE.has(deal.id) || STUDENT_ELIGIBLE.has(deal.retailer);
  const studentPct = STUDENT_PCT_BY_RETAILER[deal.retailer] ?? (studentEligible ? 10 : 0);
  let studentDiscountApplied = false;

  if (applyStudent && studentEligible && studentPct > 0) {
    const studentAmt = Math.round(running * studentPct) / 100;
    if (studentAmt > 0) {
      running = Math.max(0, Math.round((running - studentAmt) * 100) / 100);
      lines.push({
        label: `Student discount (~${studentPct}%)`,
        amount: -studentAmt,
        kind: "student",
      });
      studentDiscountApplied = true;
    }
  }

  lines.push({ label: "Estimated you pay", amount: running, kind: "total" });

  let finalPriceWithStudent: number | undefined;
  let linesWithStudent: PricingLine[] | undefined;
  if (studentEligible && studentPct > 0 && !studentDiscountApplied) {
    const withStudent = computePricingBreakdown(deal, { applyStudent: true });
    finalPriceWithStudent = withStudent.finalPrice;
    linesWithStudent = withStudent.lines;
  }

  return {
    advertisedPrice: deal.price,
    finalPrice: running,
    finalPriceWithStudent,
    totalSavedVsMsrp: Math.round((deal.msrp - running) * 100) / 100,
    totalSavedVsAdvertised: Math.round((deal.price - running) * 100) / 100,
    lines,
    linesWithStudent,
    coupon: couponMeta,
    studentEligible,
    studentDiscountApplied,
    bundle,
  };
}
