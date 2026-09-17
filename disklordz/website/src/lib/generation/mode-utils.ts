import type { GenerationSpec, LoopLength } from "@/lib/generation/generation-spec";

export function barsForLength(length: LoopLength): { min: number; max: number } {
  switch (length) {
    case "short":
      return { min: 2, max: 4 };
    case "medium":
      return { min: 4, max: 8 };
    case "long":
      return { min: 8, max: 16 };
    default:
      return { min: 4, max: 8 };
  }
}

export function barsForSpec(spec: GenerationSpec): number {
  const tier = barsForLength(spec.length);
  return Math.min(tier.max, Math.max(tier.min, spec.bars));
}

/** ILLUGEN-shaped credit costs (WO-SAAS-014). */
export function creditCostForSpec(spec: GenerationSpec): number {
  if (spec.mode === "one_shot") {
    return 1;
  }
  if (spec.mode === "sfx") {
    switch (spec.length) {
      case "short":
        return 1;
      case "medium":
        return 2;
      case "long":
        return 3;
      default:
        return 1;
    }
  }
  const bars = barsForSpec(spec);
  let cost = 2;
  if (bars > 4) cost = 4;
  if (bars > 8) cost = 7;
  if (spec.engine === "studio") cost += 1;
  return cost;
}

export const PRODUCT_PACK_CREDIT_COST = 10;
