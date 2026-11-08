import { NextRequest, NextResponse } from "next/server";

import { parseGenerationSpec } from "@/lib/generation/generation-spec";
import { polishPromptWithAi } from "@/lib/rag/ai-suggest";
import { retrieveHybrid } from "@/lib/rag/hybrid-retrieve";
import {
  exemplarToSpecHints,
  mergePromptWithSnippets,
  pickRandomExemplar,
  suggestExemplar,
} from "@/lib/rag/retrieve";
import { getPreset, STYLE_PRESETS } from "@/lib/presets";

type Body = {
  mode?: "random" | "suggest";
  presetId?: string;
  query?: string;
  applySpec?: boolean;
  useAiPolish?: boolean;
};

export async function POST(req: NextRequest) {
  let body: Body;
  try {
    body = await req.json();
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  const mode = body.mode === "suggest" ? "suggest" : "random";
  const presetId = body.presetId ?? STYLE_PRESETS[0].id;
  if (!getPreset(presetId)) {
    return NextResponse.json({ error: "invalid_preset" }, { status: 400 });
  }

  const query = (body.query ?? "").trim();
  const exemplar =
    mode === "random" ? pickRandomExemplar(presetId) : suggestExemplar(query || "drums", presetId);

  const effectivePreset = exemplar.presetId ?? presetId;
  const { snippets, retrieval } = await retrieveHybrid(
    `${query} ${exemplar.prompt} ${exemplar.tags.join(" ")}`,
    3,
  );

  let prompt =
    mode === "suggest" && query.length >= 3
      ? mergePromptWithSnippets(query, snippets)
      : exemplar.prompt;

  if (body.useAiPolish !== false) {
    const polished = await polishPromptWithAi(prompt, snippets);
    if (polished) {
      prompt = polished;
    }
  }

  const specPartial = exemplarToSpecHints(exemplar, effectivePreset);
  const parsed = parseGenerationSpec(specPartial, effectivePreset);
  const spec = parsed.ok ? parsed.spec : undefined;

  return NextResponse.json({
    mode,
    prompt,
    presetId: effectivePreset,
    spec,
    exemplar: {
      prompt: exemplar.prompt,
      tags: exemplar.tags,
    },
    snippets: snippets.map((s) => ({ id: s.id, text: s.text })),
    retrieval,
  });
}
