import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { defaultGenerationSpec } from "@/lib/generation/generation-spec";
import { KNOWLEDGE_SNIPPETS, type KnowledgeSnippet } from "@/lib/rag/knowledge-snippets";
import { PROMPT_EXEMPLARS, type PromptExemplar } from "@/lib/rag/prompt-exemplars";

function tokenize(text: string): string[] {
  return text.toLowerCase().match(/[a-z0-9]+/g) ?? [];
}

function scoreTokens(queryTokens: string[], text: string): number {
  const t = text.toLowerCase();
  return queryTokens.reduce((sum, term) => sum + (t.includes(term) ? 1 : 0), 0);
}

export function retrieveSnippets(query: string, limit = 3): KnowledgeSnippet[] {
  const tokens = tokenize(query);
  if (!tokens.length) {
    return [];
  }
  return KNOWLEDGE_SNIPPETS
    .map((s) => ({ s, score: scoreTokens(tokens, `${s.text} ${s.tags.join(" ")}`) }))
    .filter((row) => row.score > 0)
    .sort((a, b) => b.score - a.score)
    .slice(0, limit)
    .map((row) => row.s);
}

function matchesPreset(exemplar: PromptExemplar, presetId?: string): boolean {
  if (!presetId) {
    return true;
  }
  return !exemplar.presetId || exemplar.presetId === presetId;
}

export function pickRandomExemplar(presetId?: string): PromptExemplar {
  const pool = PROMPT_EXEMPLARS.filter((e) => matchesPreset(e, presetId));
  const list = pool.length ? pool : PROMPT_EXEMPLARS;
  return list[Math.floor(Math.random() * list.length)];
}

export function suggestExemplar(query: string, presetId?: string): PromptExemplar {
  const tokens = tokenize(query);
  const pool = PROMPT_EXEMPLARS.filter((e) => matchesPreset(e, presetId));
  const scored = pool
    .map((e) => ({
      e,
      score: scoreTokens(tokens, `${e.prompt} ${e.tags.join(" ")}`),
    }))
    .sort((a, b) => b.score - a.score);

  if (scored[0]?.score > 0) {
    return scored[0].e;
  }
  return pickRandomExemplar(presetId);
}

export function exemplarToSpecHints(
  exemplar: PromptExemplar,
  presetId: string,
): Partial<GenerationSpec> {
  const base = defaultGenerationSpec(exemplar.presetId ?? presetId);
  return {
    engine: exemplar.engine ?? base.engine,
    key: exemplar.key ?? base.key,
    bpm: exemplar.bpm ?? base.bpm,
    mode: base.mode,
    bars: base.bars,
    length: base.length,
    stereo: base.stereo,
    wildness: exemplar.engine === "creative" ? 0.55 : base.wildness,
  };
}

export function mergePromptWithSnippets(prompt: string, snippets: KnowledgeSnippet[]): string {
  if (!snippets.length) {
    return prompt;
  }
  const hint = snippets[0].text.split(".")[0];
  if (prompt.toLowerCase().includes(hint.slice(0, 12).toLowerCase())) {
    return prompt;
  }
  return `${prompt.trim()} — ${hint}`;
}
