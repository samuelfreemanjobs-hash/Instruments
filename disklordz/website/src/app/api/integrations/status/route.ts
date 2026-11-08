import { NextResponse } from "next/server";

import { activeEngineBackend } from "@/lib/generation/engines/registry";
import { inngestConfigured } from "@/inngest/client";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

import { loadIntegrationManifest } from "@/lib/integrations/manifest";

export async function GET() {
  const manifest = loadIntegrationManifest();
  const admin = getSupabaseAdmin();
  let pgvectorReady = false;
  if (admin) {
    const { error } = await admin.from("prompt_knowledge_chunks").select("id").limit(1);
    pgvectorReady = !error;
  }

  return NextResponse.json({
    manifestVersion: manifest.version,
    repoCount: manifest.repos.length,
    integrated: manifest.repos.filter((r) => r.status === "integrated").length,
    partial: manifest.repos.filter((r) => r.status === "partial").length,
    external: manifest.repos.filter((r) => r.status === "external").length,
    runtime: {
      engineBackend: activeEngineBackend(),
      inngest: inngestConfigured(),
      openai: Boolean(process.env.OPENAI_API_KEY),
      pgvectorTable: pgvectorReady,
      supabaseMcp: Boolean(process.env.SUPABASE_PROJECT_REF),
      stripeMcp: Boolean(process.env.STRIPE_SECRET_KEY),
      audiocraftWorker: Boolean(process.env.DISKLORDZ_AUDIOCRAFT_ENGINE_URL),
      stableAudioWorker: Boolean(process.env.DISKLORDZ_STABLE_AUDIO_ENGINE_URL),
    },
    repos: manifest.repos,
  });
}
