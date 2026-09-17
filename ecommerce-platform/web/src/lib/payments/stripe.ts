import Stripe from "stripe";

let stripeSingleton: Stripe | null = null;

export function isStripeConfigured() {
  return Boolean(process.env.STRIPE_SECRET_KEY?.trim());
}

export function getStripe() {
  if (!isStripeConfigured()) {
    throw new Error("Stripe is not configured");
  }
  if (!stripeSingleton) {
    stripeSingleton = new Stripe(process.env.STRIPE_SECRET_KEY!, {
      apiVersion: "2026-08-26.dahlia",
    });
  }
  return stripeSingleton;
}

export function appBaseUrl() {
  return process.env.NEXT_PUBLIC_APP_URL?.replace(/\/$/, "") ?? "http://localhost:3000";
}
