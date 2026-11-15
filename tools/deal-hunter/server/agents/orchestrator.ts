/**
 * PRD Part 7 — Chief Shopping Intelligence Agent coordinates specialists.
 * No agent may complete purchases or access payment credentials.
 */

import { runShoppingAssistant } from "./shopping-assistant.js";
import { runPricingAgent } from "./pricing-agent.js";
import { runAutonomousAgent, type AgentSpecialty } from "./autonomous-fleet.js";
import { buildRecommendations, type ShopperProfile } from "../recommendations.js";
import { computeDealIntelligenceScore } from "../intelligence/deal-score.js";
import { analyzeDealIntelligence } from "../intelligence/engine.js";
import type { ScoredDeal } from "../rank.js";
import { db } from "../db.js";

export type OrchestratorIntent =
  | "general"
  | "compare"
  | "budget"
  | "verify_deal"
  | "find_cheapest";

export interface OrchestratorRequest {
  message: string;
  profile?: ShopperProfile;
  dealIds?: string[];
  budgetMax?: number;
}

export interface OrchestratorEvidence {
  kind: "deal_score" | "assistant" | "pricing" | "agent" | "recommendation";
  summary: string;
  payload: unknown;
}

export interface OrchestratorResponse {
  intent: OrchestratorIntent;
  answer: string;
  evidence: OrchestratorEvidence[];
  confidence: number;
  generatedAt: string;
  agentsInvoked: string[];
}

function detectIntent(message: string): OrchestratorIntent {
  const m = message.toLowerCase();
  if (/compare|vs\b|versus|which (one|tv|laptop)/i.test(m)) return "compare";
  if (/budget|\$\d+.*list|under \$/i.test(m)) return "budget";
  if (/legit|real deal|scam|fake|verify/i.test(m)) return "verify_deal";
  if (/cheapest|lowest price|best price/i.test(m)) return "find_cheapest";
  return "general";
}

function logRun(agentType: string, input: unknown, output: unknown) {
  const id = `run-${Date.now()}-${Math.random().toString(36).slice(2, 8)}`;
  try {
    db.prepare(
      `INSERT INTO agent_run_log (id, agent_id, summary, payload_json, created_at) VALUES (?, ?, ?, ?, ?)`,
    ).run(id, agentType, "orchestrator", JSON.stringify({ input, output }), new Date().toISOString());
  } catch {
    /* sqlite optional in serverless */
  }
  return id;
}

export function runShoppingOrchestrator(
  req: OrchestratorRequest,
  pool: ScoredDeal[],
): OrchestratorResponse {
  const intent = detectIntent(req.message);
  const profile = req.profile ?? { maxBudget: req.budgetMax ?? 500 };
  const evidence: OrchestratorEvidence[] = [];
  const agentsInvoked: string[] = [];
  let answer = "";
  let confidence = 70;

  const chiefNote =
    "Chief Shopping Intelligence: recommendations use verified catalog data only; checkout stays on retailer sites — no autonomous purchases.";

  if (intent === "compare" || intent === "find_cheapest") {
    agentsInvoked.push("product_comparison", "price_analyst", "deal_verification");
    const pricing = runPricingAgent(req.message, pool, req.dealIds);
    evidence.push({ kind: "pricing", summary: pricing.valueVerdict || pricing.summary, payload: pricing });
    answer = `${pricing.valueVerdict || pricing.summary}\n\n${chiefNote}`;
    confidence = 78;
  } else if (intent === "verify_deal") {
    agentsInvoked.push("deal_verification", "price_analyst");
    const target = req.dealIds?.[0] ? pool.find((d) => d.id === req.dealIds![0]) : pool[0];
    if (target) {
      const intel = analyzeDealIntelligence(target, pool);
      const rows = db
        .prepare(`SELECT price FROM price_snapshots WHERE deal_id = ? LIMIT 5`)
        .all(target.id) as { price: number }[];
      const score = computeDealIntelligenceScore(target, pool, {
        avg30d: intel.priceAnalysis.average30d,
        low30d: intel.priceAnalysis.lowest30d,
        advertisedDiscountPct: intel.advertisedDiscountPct,
        realSavingsVsAvgPct: intel.realSavingsVsAvgPct,
        hasHistory: rows.length >= 3,
      });
      evidence.push({ kind: "deal_score", summary: score.explanation, payload: score });
      answer = `${score.classificationLabel} (${score.score}/100, confidence ${score.confidence}%). ${intel.verdictHeadline}`;
      confidence = score.confidence;
    } else {
      answer = "No product selected to verify.";
      confidence = 40;
    }
  } else if (intent === "budget") {
    agentsInvoked.push("budget_optimization", "deal_hunter");
    const recs = buildRecommendations(pool, { ...profile, maxBudget: req.budgetMax ?? profile.maxBudget });
    evidence.push({
      kind: "recommendation",
      summary: `${recs.recommendations.length} picks within budget`,
      payload: recs,
    });
    const top = recs.recommendations.slice(0, 5).map((r) => `• ${r.title} — $${r.price.toFixed(2)} (${r.reason})`);
    answer = `Budget plan (estimates, not purchases):\n${top.join("\n")}\n\n${chiefNote}`;
    confidence = 72;
  } else {
    agentsInvoked.push("chief", "deal_hunter");
    const assistant = runShoppingAssistant(req.message, pool, []);
    evidence.push({ kind: "assistant", summary: assistant.message.slice(0, 200), payload: assistant });
    const specialty: AgentSpecialty = "value_scout";
    const scout = runAutonomousAgent(specialty, pool, profile);
    agentsInvoked.push("value_scout");
    evidence.push({ kind: "agent", summary: scout.narrative, payload: scout });
    answer = `${assistant.message}\n\n${scout.narrative}`;
    confidence = 75;
  }

  const response: OrchestratorResponse = {
    intent,
    answer,
    evidence,
    confidence,
    generatedAt: new Date().toISOString(),
    agentsInvoked,
  };
  logRun("orchestrator", req, response);
  return response;
}
