import { analyzeDealIntelligence, intelligenceForQuery } from "../intelligence/engine.js";
import { PARTICIPATING_RETAILERS } from "../participating-retailers.js";
import { detectSeason } from "../season.js";
import type { ScoredDeal } from "../rank.js";
import { evaluateTargetWatches, type TargetWatchInput } from "../target-watch.js";
import { universalSearch } from "../universal-search.js";

export type AssistantAction =
  | { type: "set_target_watch"; productQuery: string; targetPrice: number }
  | { type: "open_search"; query: string }
  | { type: "add_to_list"; dealId: string }
  | { type: "enable_notifications" };

export interface AssistantTurn {
  message: string;
  actions: AssistantAction[];
  deals: ScoredDeal[];
  intelligenceSummary?: string;
  targetWatchPreview?: ReturnType<typeof evaluateTargetWatches>[number];
  participatingRetailers: readonly string[];
  seasonLabel: string;
  generatedAt: string;
}

function parseTargetWatch(text: string): { product: string; target: number } | null {
  const priceMatch = text.match(/(?:target|at|under|below)\s*\$?\s*(\d+(?:\.\d{1,2})?)/i);
  const genericPrice = text.match(/\$\s*(\d+(?:\.\d{1,2})?)/);
  const target = Number((priceMatch ?? genericPrice)?.[1]);
  if (!Number.isFinite(target) || target <= 0) return null;

  const stripped = text
    .replace(/watch|notify|alert|target price|when|drops|to|for|me|please/gi, " ")
    .replace(/\$\s*\d+(?:\.\d{1,2})?/g, " ")
    .trim();
  const product = stripped.length > 2 ? stripped : "MPC sampler";
  return { product, target };
}

export function runShoppingAssistant(
  userMessage: string,
  pool: ScoredDeal[],
  existingWatches: TargetWatchInput[] = [],
): AssistantTurn {
  const generatedAt = new Date().toISOString();
  const season = detectSeason();
  const text = userMessage.trim();
  const lower = text.toLowerCase();
  const actions: AssistantAction[] = [];
  let message = "";
  let deals: ScoredDeal[] = [];
  let intelligenceSummary: string | undefined;
  let targetWatchPreview: ReturnType<typeof evaluateTargetWatches>[number] | undefined;

  if (/^help$|what can you|how do you/i.test(lower)) {
    message =
      "I’m your household shopping assistant. I can: find deals across participating retailers, explain real vs advertised savings, set target-price watches (e.g. MPC sampler → $299), plan rooms, and tell you buy now or wait. What are you hunting?";
    return {
      message,
      actions: [{ type: "enable_notifications" }],
      deals: [],
      participatingRetailers: PARTICIPATING_RETAILERS,
      seasonLabel: season.label,
      generatedAt,
    };
  }

  const watchIntent =
    /watch|target price|notify when|alert me when|price reaches/i.test(lower) || lower.includes("mpc");
  if (watchIntent) {
    const parsed = parseTargetWatch(text) ?? { product: "MPC sampler", target: 299 };
    const previewWatch: TargetWatchInput = {
      id: "preview",
      productQuery: parsed.product,
      targetPrice: parsed.target,
    };
    const preview = evaluateTargetWatches([previewWatch], pool)[0];
    targetWatchPreview = preview;
    actions.push({
      type: "set_target_watch",
      productQuery: parsed.product,
      targetPrice: parsed.target,
    });
    actions.push({ type: "enable_notifications" });

    if (preview.targetReached) {
      const hit = preview.participatingOffers.find((o) => o.atTarget);
      message = `Target hit! ${parsed.product} is at or below $${parsed.target.toFixed(2)} at ${hit?.retailer ?? "a participating store"} ($${hit?.price.toFixed(2) ?? preview.bestDeal?.price.toFixed(2)}). Open the official link and buy before it bounces.`;
      deals = preview.bestDeal ? [preview.bestDeal] : [];
    } else if (preview.matched) {
      const best = preview.bestDeal!;
      message = `Watching ${PARTICIPATING_RETAILERS.length} participating retailers for “${parsed.product}” at $${parsed.target.toFixed(2)} or less. Best live price: $${best.price.toFixed(2)} at ${best.retailer}. I’ll notify you on the next refresh when someone hits your target.`;
      deals = [best];
      intelligenceSummary = analyzeDealIntelligence(best, pool).verdictHeadline;
    } else {
      message = `I’ll watch for “${parsed.product}” at $${parsed.target.toFixed(2)}. No listing in this refresh — I’ll keep scanning Guitar Center, Sweetwater, Best Buy, Amazon, and the rest.`;
    }
    return {
      message,
      actions,
      deals,
      intelligenceSummary,
      targetWatchPreview,
      participatingRetailers: PARTICIPATING_RETAILERS,
      seasonLabel: season.label,
      generatedAt,
    };
  }

  if (/decor|room|living room|bedroom|office/i.test(lower)) {
    const hits = universalSearch(pool, { q: "home", includeIntelligence: true, trustedOnly: true }).slice(
      0,
      3,
    );
    message =
      "For decorating, pick one room and a budget — open the Decorate tab. Here are trusted home deals trending now:";
    deals = hits;
    return {
      message,
      actions: [],
      deals,
      participatingRetailers: PARTICIPATING_RETAILERS,
      seasonLabel: season.label,
      generatedAt,
    };
  }

  const q = intelligenceForQuery(text, pool);
  if (q.picks.length > 0) {
    const top = q.picks[0];
    const full = pool.find((d) => d.id === top.dealId);
    message = q.answer;
    intelligenceSummary = `Authenticity ${top.authenticityScore}/100 · Real savings vs avg ${top.realSavingsVsAvgPct}% (tag ${top.advertisedDiscountPct}% off MSRP). ${top.buyNowOrWait === "buy_now" ? "Buy now." : top.buyNowOrWait === "wait" ? "Wait." : "Good if you need it."}`;
    deals = full ? [full] : pool.filter((d) => q.picks.some((p) => p.dealId === d.id)).slice(0, 3);
    if (full) actions.push({ type: "add_to_list", dealId: full.id });
  } else {
    const hits = universalSearch(pool, { q: text, includeIntelligence: true, trustedOnly: true }).slice(0, 4);
    deals = hits;
    message =
      hits.length > 0
        ? `Found ${hits.length} live matches — ranked by deal quality, not ads. Fresh ${hits[0]?.fetchedAt ? new Date(hits[0].fetchedAt).toLocaleTimeString() : "now"}.`
        : "No matches this second. Try brand + model (Adidas Ultraboost, Rare Beauty) or a UPC in Search.";
    if (hits[0]) actions.push({ type: "open_search", query: text });
  }

  if (existingWatches.length > 0) {
    const status = evaluateTargetWatches(existingWatches, pool).filter((w) => w.targetReached);
    if (status.length > 0 && !watchIntent) {
      message += ` Also: ${status.length} target price watch(es) hit — check notifications.`;
    }
  }

  return {
    message,
    actions,
    deals,
    intelligenceSummary,
    participatingRetailers: PARTICIPATING_RETAILERS,
    seasonLabel: season.label,
    generatedAt,
  };
}
