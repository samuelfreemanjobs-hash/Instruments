import { NextRequest, NextResponse } from "next/server";

import { parseGenerationSpec } from "@/lib/generation/generation-spec";
import { buildVariationBatch } from "@/lib/generation/variations";
import { saveKitForUser } from "@/lib/kits/persist";
import { getPreset, STYLE_PRESETS } from "@/lib/presets";
import { checkRateLimit } from "@/lib/rate-limit";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function POST(req: NextRequest) {
  const ip =
    req.headers.get("x-forwarded-for")?.split(",")[0]?.trim() ??
    req.headers.get("x-real-ip") ??
    "anonymous";

  const limited = checkRateLimit(ip);
  if (!limited.ok) {
    return NextResponse.json(
      {
        error: "daily_limit",
        message: `Free tier allows ${limited.limit} kits per day. Try again later.`,
        retryAfterSec: limited.retryAfterSec,
        limit: limited.limit,
      },
      { status: 429 },
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
    return NextResponse.json(
      { error: "prompt_required", message: "Describe your vibe (at least 3 characters)." },
      { status: 400 },
    );
  }

  if (!getPreset(presetId)) {
    return NextResponse.json({ error: "invalid_preset" }, { status: 400 });
  }

  const parsed = parseGenerationSpec(body.spec, presetId);
  if (!parsed.ok) {
    return NextResponse.json(
      { error: parsed.error, message: "Invalid generation spec." },
      { status: 400 },
    );
  }

  const baseUrl = req.nextUrl.origin;
  const { batchId, manifests } = await buildVariationBatch(
    prompt,
    presetId,
    baseUrl,
    parsed.spec,
  );

  let savedToAccount = false;
  if (isSupabaseConfigured()) {
    const supabase = await createClient();
    if (supabase) {
      const {
        data: { user },
      } = await supabase.auth.getUser();
      if (user) {
        for (const manifest of manifests) {
          const saved = await saveKitForUser(supabase, user.id, manifest);
          if (saved.ok) savedToAccount = true;
        }
      }
    }
  }

  const variations = manifests.map((manifest) => ({
    label: manifest.variationLabel ?? "A",
    manifest,
  }));

  return NextResponse.json({
    batchId,
    variationCount: variations.length,
    variations,
    /** @deprecated Use variations[0].manifest — kept for compatibility */
    manifest: manifests[0],
    savedToAccount,
    rateLimit: { remaining: limited.remaining, limit: limited.limit },
    presets: STYLE_PRESETS.map(({ id, label, description }) => ({
      id,
      label,
      description,
    })),
  });
}
