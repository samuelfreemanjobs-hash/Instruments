/**
 * AI Vibe Forge — maps natural-language trap prompts to drum DSP params via Gemini.
 * Prefer POST /api/ai/vibe (serve.py proxy); optional direct API key (may hit CORS).
 */

const DEFAULT_MODEL = "gemini-2.0-flash";

const PARAM_SCHEMA_HINT = `Return ONLY valid JSON object with numeric fields applicable to trap drums:
rootHz, pitchMod, pitchDecay, drive, fCut, fQ, fEnvAmt, aA, aD, aS, aR, fA, fD, fS, fR,
transAttackDb, transSustainDb, bits, lofiSr, snap, snapBite, reverbMix, distType (tape|tube|foldback|spinz|fl_clip|triode),
filterMode (lp12|lp24|svf). Omit unknown keys.`;

export async function forgeVibeFromPrompt(prompt, drumId, { apiKey, model } = {}) {
  const body = {
    prompt: String(prompt).slice(0, 500),
    drumId,
    model: model || localStorage.getItem("trapforge_gemini_model") || DEFAULT_MODEL,
  };

  const proxyRes = await fetch("/api/ai/vibe", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  }).catch(() => null);

  if (proxyRes?.ok) {
    const data = await proxyRes.json();
    return data.parameters || data;
  }

  const key = apiKey || localStorage.getItem("trapforge_gemini_key");
  if (!key) {
    throw new Error("Add Gemini API key or run python serve.py with GEMINI_API_KEY for AI Vibe Forge.");
  }

  const modelId = body.model;
  const url = `https://generativelanguage.googleapis.com/v1beta/models/${modelId}:generateContent?key=${encodeURIComponent(key)}`;
  const res = await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      contents: [
        {
          role: "user",
          parts: [
            {
              text: `You are a trap drum sound designer. Drum type: ${drumId}. User vibe: "${body.prompt}". ${PARAM_SCHEMA_HINT}`,
            },
          ],
        },
      ],
      generationConfig: { temperature: 0.35, responseMimeType: "application/json" },
    }),
  });
  if (!res.ok) throw new Error(`Gemini error ${res.status}`);
  const json = await res.json();
  const text = json.candidates?.[0]?.content?.parts?.[0]?.text || "{}";
  return JSON.parse(text);
}

export function applyForgedParams(target, forged) {
  if (!forged || typeof forged !== "object") return;
  Object.entries(forged).forEach(([k, v]) => {
    if (v === null || v === undefined) return;
    if (typeof target[k] === "number" && typeof v === "number") target[k] = v;
    else if (typeof target[k] === "string" && typeof v === "string") target[k] = v;
    else if (target[k] !== undefined && (typeof v === "number" || typeof v === "string")) target[k] = v;
  });
}
