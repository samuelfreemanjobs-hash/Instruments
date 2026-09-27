#!/usr/bin/env node
/**
 * Classify intake text → agent lane (WO-SAAS-022). Rules-only; no LLM.
 * Usage: echo "stripe webhook" | node pm-router-classify.mjs
 */
import { readFileSync } from "fs";

const text = (readFileSync(0, "utf8") + process.argv.slice(2).join(" ")).toLowerCase();

const rules = [
  {
    lane: "vst_preview",
    wo: "PLUGIN-001",
    re: /\b(vst preview|preview wav|offline render|listen in vscode|plugin preview)\b/,
  },
  { lane: "factory_dsp", wo: "018", re: /\b(kick|snare|hat|dsp|factory|808|phonk|audio|wav|synth)\b/ },
  { lane: "saas_ops", wo: "019", re: /\b(health|go-live|vercel|deploy|uptime|503|generate api)\b/ },
  { lane: "billing", wo: "020", re: /\b(stripe|webhook|checkout|subscription|credits|billing|pro plan)\b/ },
  { lane: "growth", wo: "021", re: /\b(marketing|landing|copy|seo|social|tutorial|mpc)\b/ },
  { lane: "preset_curator", wo: "023", re: /\b(preset|lane|dl00|artist|tag|bpm hint)\b/ },
  { lane: "customer_success", wo: "024", re: /\b(support|ticket|user report|can't download|login|help)\b/ },
  { lane: "analytics", wo: "025", re: /\b(funnel|analytics|conversion|metrics|drop-off)\b/ },
  { lane: "async_jobs", wo: "026", re: /\b(async|job queue|poll|stuck generating|spinner)\b/ },
  { lane: "audio_qa", wo: "027", re: /\b(golden|regression|qa audio|lane quality)\b/ },
  { lane: "competitive_intel", wo: "028", re: /\b(splice|competitor|pricing|market intel|loudly)\b/ },
  { lane: "plugin_juce", wo: "HISE", re: /\b(juce|vst|plugin|cmake|pluginval|wave909)\b/ },
  { lane: "hise_antigravity", wo: "HISE", re: /\b(hise|antigravity|sketch lane)\b/ },
];

const hits = rules.filter((r) => r.re.test(text));
const primary = hits[0] ?? { lane: "saas_general", wo: "019", re: null };

const out = {
  primaryLane: primary.lane,
  suggestedWo: primary.wo,
  allLanes: hits.map((h) => h.lane),
  dispatch: {
    factory_dsp: "disklordz-factory-daily",
    saas_ops: "disklordz-saas-ops-daily",
    billing: "disklordz-billing-weekly",
    growth: "disklordz-growth-weekly",
    preset_curator: "disklordz-preset-curator-weekly",
    vst_preview: "instruments-vst-preview",
    plugin_juce: "build-plugin",
    hise_antigravity: "airtable-antigravity-handoff",
    customer_success: "disklordz-support-weekly",
    analytics: "disklordz-analytics-weekly",
    async_jobs: "disklordz-async-jobs-audit",
    audio_qa: "disklordz-audio-qa-weekly",
    competitive_intel: "disklordz-competitive-intel-monthly",
  }[primary.lane],
};

console.log(JSON.stringify(out, null, 2));
