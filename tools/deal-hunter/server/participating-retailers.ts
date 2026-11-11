/** Retailers the app monitors for target-price hits */
export const PARTICIPATING_RETAILERS = [
  "Amazon",
  "Best Buy",
  "Walmart",
  "Target",
  "Costco",
  "Guitar Center",
  "Sweetwater",
  "Adidas",
  "Sephora",
  "Ulta",
  "Newegg",
  "eBay",
  "REI",
  "Chewy",
  "Home Depot",
] as const;

export type ParticipatingRetailer = (typeof PARTICIPATING_RETAILERS)[number];

export function isParticipating(retailer: string): boolean {
  return PARTICIPATING_RETAILERS.some(
    (r) => r.toLowerCase() === retailer.toLowerCase(),
  );
}
