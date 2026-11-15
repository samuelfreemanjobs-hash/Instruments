import type { ScoredDeal } from "../rank.js";

export type RoomType =
  | "living"
  | "bedroom"
  | "kitchen"
  | "office"
  | "outdoor";

export interface DecorAgentRequest {
  room: RoomType;
  style: string;
  budgetMax?: number;
  colors?: string;
}

export interface DecorAgentPlan {
  headline: string;
  palette: string[];
  tips: string[];
  picks: ScoredDeal[];
  listSuggestion: string;
  analyzedAt: string;
}

const ROOM_CATEGORIES: Record<RoomType, string[]> = {
  living: ["Home", "TV", "Audio"],
  bedroom: ["Bedroom", "Home"],
  kitchen: ["Kitchen"],
  office: ["PC", "Home"],
  outdoor: ["Outdoor", "Outdoors", "Apparel"],
};

const STYLE_PALETTES: Record<string, string[]> = {
  modern: ["charcoal", "white", "brass accent"],
  cozy: ["warm beige", "sage", "soft gold"],
  minimal: ["off-white", "black", "natural wood"],
  industrial: ["concrete gray", "matte black", "rusted copper"],
};

export function runDecorAgent(
  body: DecorAgentRequest,
  pool: ScoredDeal[],
): DecorAgentPlan {
  const analyzedAt = new Date().toISOString();
  const cats = ROOM_CATEGORIES[body.room];
  const styleKey = body.style.trim().toLowerCase();
  const palette =
    STYLE_PALETTES[styleKey] ??
    (body.colors
      ? body.colors.split(/[,/]+/).map((c) => c.trim())
      : ["neutral base", "one accent color"]);

  let picks = pool.filter((d) => cats.includes(d.category));
  if (body.budgetMax != null) {
    picks = picks.filter((d) => d.price <= body.budgetMax!);
  }
  picks = picks.sort((a, b) => b.stealScore - a.stealScore).slice(0, 5);

  const roomLabel: Record<RoomType, string> = {
    living: "living room",
    bedroom: "bedroom",
    kitchen: "kitchen",
    office: "home office",
    outdoor: "outdoor space",
  };

  const tips = [
    `Start with one anchor piece for the ${roomLabel[body.room]} — then build around it so decisions stay small.`,
    `Keep ${palette[0]} as the base; use your accent in pillows, art, or one appliance only.`,
    "Add picks to **Our list** so your partner sees the same plan (one share code).",
  ];

  return {
    headline: `${body.style || "Cozy"} ${roomLabel[body.room]} — under $${body.budgetMax ?? "your budget"}`,
    palette,
    tips: tips.map((t) => t.replace(/\*\*/g, "")),
    picks,
    listSuggestion:
      picks.length > 0
        ? `Tap + List on: ${picks.slice(0, 2).map((p) => p.title.split("—")[0].trim()).join(" & ")}.`
        : "Loosen budget or check Deals search for Home / Kitchen categories.",
    analyzedAt,
  };
}
