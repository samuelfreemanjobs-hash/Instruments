import { NextRequest, NextResponse } from "next/server";

import { GROK_PRESETS, type GrokPresetId } from "@/lib/grok/presets";
import { triggerGrokBot } from "@/lib/grok/inference-hook";

/**
 * Server-only bridge: phone PWA → Grok Bot / Cursor cloud runtime.
 * API keys and hook URLs live in Vercel env — never shipped to the client.
 */

type Body = {
  prompt?: string;
  repoUrl?: string;
  preset?: GrokPresetId;
  clientRef?: string;
  target?: "grok" | "cursor" | "auto";
};

export async function POST(req: NextRequest) {
  let body: Body;
  try {
    body = await req.json();
  } catch {
    return NextResponse.json(
      { success: false, error: "Invalid JSON body" },
      { status: 400 },
    );
  }

  const presetId = body.preset;
  const repoUrl = body.repoUrl?.trim();

  let prompt = (body.prompt ?? "").trim();
  if (presetId) {
    if (!GROK_PRESETS[presetId]) {
      return NextResponse.json(
        { success: false, error: "Unknown preset" },
        { status: 400 },
      );
    }
    const repo =
      repoUrl ||
      process.env.GROK_DEFAULT_REPO_URL ||
      "https://github.com/samuelfreemanjobs-hash/Instruments";
    prompt = GROK_PRESETS[presetId].buildMessage(repo);
  }

  if (!prompt) {
    return NextResponse.json(
      { success: false, error: "Prompt is required" },
      { status: 400 },
    );
  }
  if (prompt.length > 8000) {
    return NextResponse.json(
      { success: false, error: "Prompt is too long" },
      { status: 400 },
    );
  }

  const result = await triggerGrokBot({
    prompt,
    repoUrl,
    preset: presetId,
    clientRef: body.clientRef,
    target: body.target ?? "auto",
  });

  if (!result.ok) {
    const status = result.error === "target_platform_rejected" ? 502 : 500;
    return NextResponse.json(
      {
        success: false,
        error:
          result.error === "target_platform_rejected"
            ? "Target platform rejected agent execution loop"
            : "Internal system fault triggering background worker",
      },
      { status },
    );
  }

  return NextResponse.json(
    {
      success: true,
      dryRun: result.dryRun,
      status: result.platformStatus,
      requestId: result.requestId,
      detail: result.detail ?? null,
    },
    { status: result.dryRun ? 202 : 200 },
  );
}
