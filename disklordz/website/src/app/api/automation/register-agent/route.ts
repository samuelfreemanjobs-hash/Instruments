import { NextResponse } from "next/server";

import { getSupabaseAdmin } from "@/lib/supabase/admin";

const AUTOMATION_SECRET = process.env.AUTOMATION_SECRET;

type Body = {
  agent_id?: string;
  source_path?: string;
  workflow_kind?: string;
  schedule_cron?: string;
  config?: Record<string, unknown>;
};

/** FORGE: register workflow when a new agent/plan lands in repo (CI or local). */
export async function POST(req: Request) {
  if (AUTOMATION_SECRET) {
    const auth = req.headers.get("authorization")?.replace(/^Bearer\s+/i, "");
    if (auth !== AUTOMATION_SECRET) {
      return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
    }
  }

  let body: Body;
  try {
    body = (await req.json()) as Body;
  } catch {
    return NextResponse.json({ error: "Invalid JSON" }, { status: 400 });
  }

  const agent_id = body.agent_id?.trim();
  const source_path = body.source_path?.trim();
  if (!agent_id || !source_path || agent_id.length > 120 || source_path.length > 500) {
    return NextResponse.json({ error: "agent_id and source_path required" }, { status: 400 });
  }

  const supabase = getSupabaseAdmin();
  if (supabase) {
    await supabase.from("workflow_registry").insert({
      agent_id,
      source_path,
      workflow_kind: body.workflow_kind ?? "agent_created",
      schedule_cron: body.schedule_cron ?? null,
      config: body.config ?? {},
    });
  }

  console.info("[automation/register-agent]", { agent_id, source_path });

  return NextResponse.json({
    ok: true,
    agent_id,
    next: [
      "Ensure subagent listed in DISKLORDZ_MARKETING_DIRECTOR or FORGE roster",
      "HELM: add launch checklist hook if GTM agent",
      "PM: schedule WO if engineering touch required",
    ],
  });
}
