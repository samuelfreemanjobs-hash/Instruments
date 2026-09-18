import { NextResponse } from "next/server";

import { getLaunchCampaignDay, isTripwireLive } from "@/lib/launch/campaign-day";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

function auth(req: Request): boolean {
  const secret = process.env.AUTOMATION_SECRET ?? process.env.CRON_SECRET;
  if (!secret) return process.env.NODE_ENV !== "production";
  const url = new URL(req.url);
  const provided =
    req.headers.get("authorization")?.replace(/^Bearer\s+/i, "") ??
    url.searchParams.get("secret");
  return provided === secret;
}

/** Executive Assistant / Slack workflow: daily status JSON + slack text */
export async function GET(req: Request) {
  if (!auth(req)) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  const checks = {
    resend: Boolean(process.env.RESEND_API_KEY && process.env.RESEND_FROM_EMAIL),
    supabaseAdmin: Boolean(getSupabaseAdmin()),
    launchCampaignStart: Boolean(process.env.LAUNCH_CAMPAIGN_START),
    vocalStemsUrl: Boolean(process.env.LAUNCH_VOCAL_STEMS_URL),
    tripwireStripe: Boolean(process.env.STRIPE_TRIPWIRE_PRICE_ID),
    tripwireLive: isTripwireLive(),
    campaignDay: getLaunchCampaignDay(),
  };

  const blockers: string[] = [];
  if (!checks.supabaseAdmin) blockers.push("SUPABASE_SERVICE_ROLE_KEY + migrations");
  if (!checks.resend) blockers.push("RESEND_* for lifecycle email");
  if (!checks.vocalStemsUrl) blockers.push("LAUNCH_VOCAL_STEMS_URL");
  if (!checks.tripwireStripe) blockers.push("STRIPE_TRIPWIRE_PRICE_ID");

  const slackText = [
    "*Disklordz exec digest*",
    `Campaign day: ${checks.campaignDay} · Tripwire live: ${checks.tripwireLive ? "yes" : "no"}`,
    blockers.length
      ? `*Blockers:* ${blockers.join("; ")}`
      : "*Blockers:* none on config checklist",
    "Agents: PM schedule · FORGE cron · CROW QC · HELM launch · VARA copy",
    "Factory: `disklordz/factory/WORKFLOW.md`",
    "Ask Chief of Staff (SAGE) in #disklordz-exec",
  ].join("\n");

  return NextResponse.json({
    ok: true,
    generatedAt: new Date().toISOString(),
    checks,
    blockers,
    slackText,
    routing: {
      marketing: "#disklordz-marketing-dept",
      exec: "#disklordz-exec",
      factoryAgents: ["artist-midnight-circuit", "ar-kit-qc", "ar-director", "product-launch-manager", "workflow-automation-engineer", "marketing-director"],
    },
  });
}
