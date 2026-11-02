import { NextRequest, NextResponse } from "next/server";

import { inngest, inngestConfigured } from "@/inngest/client";
import { parseGenerationSpec } from "@/lib/generation/generation-spec";
import { newBatchId } from "@/lib/credits";
import { getPreset, STYLE_PRESETS } from "@/lib/presets";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function POST(req: NextRequest) {
  if (!inngestConfigured()) {
    return NextResponse.json(
      {
        error: "inngest_not_configured",
        message: "Set INNGEST_EVENT_KEY (or INNGEST_DEV=1 for local dev).",
      },
      { status: 503 },
    );
  }

  let body: { prompt?: string; presetId?: string; spec?: Record<string, unknown> };
  try {
    body = await req.json();
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  const prompt = (body.prompt ?? "").trim();
  const presetId = body.presetId ?? STYLE_PRESETS[0].id;
  if (!prompt || prompt.length < 3) {
    return NextResponse.json({ error: "prompt_required" }, { status: 400 });
  }
  if (!getPreset(presetId)) {
    return NextResponse.json({ error: "invalid_preset" }, { status: 400 });
  }

  const parsed = parseGenerationSpec(body.spec, presetId);
  if (!parsed.ok) {
    return NextResponse.json({ error: parsed.error }, { status: 400 });
  }

  let userId: string | null = null;
  if (isSupabaseConfigured()) {
    const supabase = await createClient();
    if (supabase) {
      const {
        data: { user },
      } = await supabase.auth.getUser();
      userId = user?.id ?? null;
    }
  }

  const batchId = newBatchId();
  const baseUrl = req.nextUrl.origin;

  await inngest.send({
    name: "disklordz/generate.requested",
    data: {
      prompt,
      presetId,
      spec: parsed.spec,
      baseUrl,
      userId,
      batchId,
    },
  });

  return NextResponse.json({
    batchId,
    status: "queued",
    pollHint: "Use sync POST /api/generate until job status API ships (WO-SAAS-009).",
  });
}
