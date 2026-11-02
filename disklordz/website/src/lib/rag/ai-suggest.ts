import { generateText } from "ai";
import { openai } from "@ai-sdk/openai";

import type { KnowledgeSnippet } from "@/lib/rag/knowledge-snippets";

/** Optional Vercel AI SDK polish when OPENAI_API_KEY is set. */
export async function polishPromptWithAi(
  prompt: string,
  snippets: KnowledgeSnippet[],
): Promise<string | null> {
  if (!process.env.OPENAI_API_KEY) {
    return null;
  }
  const context = snippets.map((s) => s.text).join("\n");
  const { text } = await generateText({
    model: openai(process.env.OPENAI_CHAT_MODEL ?? "gpt-4o-mini"),
    system:
      "You refine drum-sample prompts for Disklordz lanes. Keep under 220 chars. No markdown.",
    prompt: `Context:\n${context}\n\nUser prompt:\n${prompt}\n\nRefined prompt:`,
    maxOutputTokens: 120,
  });
  const trimmed = text.trim();
  return trimmed.length >= 3 ? trimmed : null;
}
