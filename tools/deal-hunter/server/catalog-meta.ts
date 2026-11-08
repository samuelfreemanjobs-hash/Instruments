/** Shared product identity fields for universal search */
export interface ProductIdentity {
  brand: string;
  model?: string;
  upc?: string;
  specs: Record<string, string>;
  /** Hints for image similarity search */
  visualTags: string[];
}

export const IDENTITY_BY_ID: Record<string, ProductIdentity> = {
  d1: {
    brand: "Sony",
    model: "WH-1000XM5",
    upc: "027242918524",
    specs: { connectivity: "Bluetooth 5.2", noise_cancelling: "yes" },
    visualTags: ["black", "headphones", "electronics"],
  },
  d9: {
    brand: "Samsung",
    model: "990 PRO 2TB",
    upc: "887276804205",
    specs: { capacity: "2TB", interface: "NVMe PCIe 4.0" },
    visualTags: ["silver", "ssd", "electronics"],
  },
  ad1: {
    brand: "Adidas",
    model: "Ultraboost 5",
    upc: "4067885001234",
    specs: { size_run: "US 7-13", material: "Primeknit upper" },
    visualTags: ["white", "sneaker", "athletic", "running"],
  },
  ad2: {
    brand: "Adidas",
    model: "Samba OG",
    upc: "4062059287654",
    specs: { size_run: "US 5-12", material: "leather" },
    visualTags: ["white", "black", "sneaker", "casual"],
  },
  sp1: {
    brand: "Rare Beauty",
    model: "Soft Pinch Trio",
    upc: "810025683912",
    specs: { type: "liquid blush set", shade_count: "3" },
    visualTags: ["pink", "beauty", "makeup", "sephora"],
  },
  sp2: {
    brand: "Dyson",
    model: "Airwrap Complete",
    upc: "885609028901",
    specs: { attachments: "6", heat_control: "yes" },
    visualTags: ["copper", "beauty", "hair", "sephora"],
  },
  ul1: {
    brand: "Anastasia Beverly Hills",
    model: "Brow Wiz",
    upc: "689860112340",
    specs: { shade: "Soft Brown", format: "pencil" },
    visualTags: ["brown", "beauty", "brow", "ulta"],
  },
  ul2: {
    brand: "Lancome",
    model: "Genifique Serum 50ml",
    upc: "3605533251823",
    specs: { volume: "50ml", skin_type: "all" },
    visualTags: ["gold", "skincare", "serum", "ulta"],
  },
  "mpc-gc": {
    brand: "Akai",
    model: "MPC Live II",
    upc: "0694318042345",
    specs: { type: "sampler", pads: "16" },
    visualTags: ["black", "mpc", "music"],
  },
  "mpc-sw": {
    brand: "Akai",
    model: "MPC Live II",
    upc: "0694318042345",
    specs: { type: "sampler", condition: "open box" },
    visualTags: ["black", "mpc", "music"],
  },
  "mpc-bb": {
    brand: "Akai",
    model: "MPC Live II",
    upc: "0694318042345",
    specs: { type: "sampler" },
    visualTags: ["black", "mpc", "music"],
  },
};

export function identityFor(dealId: string, title: string, retailer: string): ProductIdentity {
  if (IDENTITY_BY_ID[dealId]) return IDENTITY_BY_ID[dealId];
  const brandGuess = title.split("—")[0]?.trim().split(" ")[0] ?? "Unknown";
  return {
    brand: brandGuess,
    specs: {},
    visualTags: [retailer.toLowerCase()],
  };
}

export function searchableBlob(
  title: string,
  retailer: string,
  category: string,
  tags: string[],
  identity: ProductIdentity,
): string {
  return [
    title,
    retailer,
    category,
    identity.brand,
    identity.model ?? "",
    identity.upc ?? "",
    ...tags,
    ...Object.entries(identity.specs).flatMap(([k, v]) => [k, v]),
    ...identity.visualTags,
  ]
    .join(" ")
    .toLowerCase();
}
