import { customAlphabet } from "nanoid";
import { listCommunityDeals } from "../community.js";
import { discoverLocalDeals, enrichWithLocalHints } from "../local-deals.js";
import { buildRecommendations, type ShopperProfile } from "../recommendations.js";
import { detectSeason } from "../season.js";
import type { ScoredDeal } from "../rank.js";
import { analyzeDealIntelligence } from "../intelligence/engine.js";
import { computeValueScore, explainBestValueWinner } from "../value-score.js";
import { returnPolicyFor } from "../return-policy.js";

const runId = customAlphabet("23456789ABCDEFGHJKLMNPQRSTUVWXYZ", 10);

export type AgentSpecialty =
  | "value_scout"
  | "deal_stalker"
  | "local_scout"
  | "community_curator"
  | "season_planner";

export interface AutonomousAgent {
  id: AgentSpecialty;
  name: string;
  mission: string;
  scheduleHint: string;
}

export const AUTONOMOUS_AGENTS: readonly AutonomousAgent[] = [
  {
    id: "value_scout",
    name: "Value Scout",
    mission: "Hunts best value (not cheapest junk) across authorized feeds.",
    scheduleHint: "Every 15m via n8n",
  },
  {
    id: "deal_stalker",
    name: "Deal Stalker",
    mission: "Watches steal scores + your targets; triggers push/email.",
    scheduleHint: "On price refresh + n8n webhooks",
  },
  {
    id: "local_scout",
    name: "Local Scout",
    mission: "Surfaces partner circulars and pickup near your ZIP.",
    scheduleHint: "Daily local file ingest",
  },
  {
    id: "community_curator",
    name: "Community Curator",
    mission: "Ranks household tips on trusted domains.",
    scheduleHint: "Hourly moderation queue",
  },
  {
    id: "season_planner",
    name: "Season Planner",
    mission: "Aligns picks with Black Friday → holiday → grocery modes.",
    scheduleHint: "Season boundary cron",
  },
] as const;

export interface AgentRunResult {
  agentId: AgentSpecialty;
  title: string;
  picks: { id: string; label: string; detail: string }[];
  narrative: string;
  runAt: string;
}

export function runAutonomousAgent(
  agentId: AgentSpecialty,
  pool: ScoredDeal[],
  profile: ShopperProfile,
): AgentRunResult {
  const runAt = new Date().toISOString();

  switch (agentId) {
    case "value_scout": {
      const ranked = [...pool]
        .map((d) => ({
          d,
          v: computeValueScore(d, pool, returnPolicyFor(d)),
        }))
        .sort((a, b) => b.v.total - a.v.total)
        .slice(0, 5);
      const top = ranked[0];
      const second = ranked[1];
      return {
        agentId,
        title: "Value Scout digest",
        picks: ranked.map(({ d, v }) => ({
          id: d.id,
          label: d.title,
          detail: `Value ${v.total}/100 · steal ${d.stealScore}`,
        })),
        narrative:
          top && second
            ? explainBestValueWinner(
                top.d,
                second.d,
                top.v,
                second.v,
                returnPolicyFor(top.d),
              )
            : "Ranked by value score (returns, trust, real savings) — not sticker price alone.",
        runAt,
      };
    }
    case "deal_stalker": {
      const hot = pool
        .filter((d) => d.stealScore >= 82 || d.priceDropped)
        .sort((a, b) => b.stealScore - a.stealScore)
        .slice(0, 6);
      return {
        agentId,
        title: "Deal Stalker alerts",
        picks: hot.map((d) => ({
          id: d.id,
          label: d.title,
          detail: `Steal ${d.stealScore}${d.priceDropped ? " · fresh drop" : ""} · $${d.price.toFixed(2)}`,
        })),
        narrative: "Would have pinged web push + email when notifications are enabled.",
        runAt,
      };
    }
    case "local_scout": {
      const local = discoverLocalDeals(profile.zip);
      const hints = enrichWithLocalHints(pool, profile.zip);
      return {
        agentId,
        title: `Local Scout — ${local.metro}`,
        picks: [
          ...local.offers.map((o) => ({
            id: o.id,
            label: o.title,
            detail: `${o.storeName} · ${o.distanceMiles} mi · ${o.source.replace("_", " ")}`,
          })),
          ...hints.map((h) => ({
            id: h.dealId,
            label: "Online + pickup",
            detail: h.hint,
          })),
        ].slice(0, 8),
        narrative: local.dataPolicy,
        runAt,
      };
    }
    case "community_curator": {
      const comm = listCommunityDeals(8);
      return {
        agentId,
        title: "Community Curator",
        picks: comm.map((c) => ({
          id: c.id,
          label: c.title,
          detail: `${c.retailer} · $${c.price.toFixed(2)} · ${c.upvotes} upvotes`,
        })),
        narrative: "Community tips must pass trusted-domain checks before display.",
        runAt,
      };
    }
    case "season_planner": {
      const season = detectSeason();
      const recs = buildRecommendations(
        pool.filter((d) => d.seasons.includes(season.season)),
        profile,
      );
      return {
        agentId,
        title: `Season Planner — ${season.label}`,
        picks: recs.recommendations.slice(0, 6).map((r) => ({
          id: r.dealId,
          label: r.title,
          detail: r.reason,
        })),
        narrative: season.focus,
        runAt,
      };
    }
  }
}

export interface AutopilotDigest {
  runId: string;
  generatedAt: string;
  agents: AgentRunResult[];
  topVerdicts: string[];
  automation: { n8n: string; vercel: string };
}

export function runAutopilotDigest(pool: ScoredDeal[], profile: ShopperProfile): AutopilotDigest {
  const agents = AUTONOMOUS_AGENTS.map((a) => runAutonomousAgent(a.id, pool, profile));
  const topVerdicts = pool
    .slice(0, 8)
    .map((d) => analyzeDealIntelligence(d, pool))
    .filter((i) => i.buyNowOrWait === "buy_now")
    .map((i) => i.bestValueVerdict ?? i.verdictHeadline)
    .slice(0, 4);

  return {
    runId: runId(),
    generatedAt: new Date().toISOString(),
    agents,
    topVerdicts,
    automation: {
      n8n: "Schedule POST /api/autopilot/run with N8N_WEBHOOK_SECRET",
      vercel: "Static PWA on Vercel; Node API on container or long-lived host",
    },
  };
}
