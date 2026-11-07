import "server-only";

const MAX_PROMPT = 8_000;
const MAX_REPO_URL = 512;

export type GrokTriggerInput = {
  prompt: string;
  repoUrl?: string;
  preset?: string;
  clientRef?: string;
  /** Prefer Grok inference hook or Cursor Cloud Agent API URL from env */
  target?: "grok" | "cursor" | "auto";
};

export type GrokTriggerResult =
  | {
      ok: true;
      dryRun: boolean;
      status: number;
      requestId: string;
      platformStatus: string;
      detail?: string;
    }
  | { ok: false; error: string };

function agentsContextBlock(): string {
  const excerpt = process.env.GROK_AGENTS_MD_EXCERPT?.trim();
  if (excerpt) return `\n\nFollow repo AGENTS.md conventions:\n${excerpt.slice(0, 4000)}`;
  return (
    "\n\nRead AGENTS.md and ARCHITECTURE.md in the repo before editing. " +
    "For Instruments/JUCE work run vst-testing-ops CI before claiming done."
  );
}

/** Message shape Grok Bot / Cursor cloud runtime expects for orchestration tasks. */
export function buildOrchestrationMessage(repoUrl: string, prompt: string): string {
  const context = repoUrl?.trim() || "Main Workspace";
  const task = prompt.trim().slice(0, MAX_PROMPT);
  return (
    `Project Context: ${context}. Task: "${task}". ` +
    "Process through your execution sandbox, verify output builds correctly, " +
    "and submit structural status updates back to this channel." +
    agentsContextBlock()
  );
}

export function buildGrokMessage(input: GrokTriggerInput, repoUrl: string): string {
  if (input.preset) {
    return input.prompt.trim().slice(0, MAX_PROMPT) + agentsContextBlock();
  }
  return buildOrchestrationMessage(repoUrl, input.prompt);
}

function resolveAgentHookUrl(target: GrokTriggerInput["target"]): string {
  const grok = process.env.GROK_INFERENCE_HOOK_URL?.trim() ?? "";
  const cursor = process.env.CURSOR_AGENT_API_URL?.trim() ?? "";

  if (target === "cursor") return cursor;
  if (target === "grok") return grok;
  return grok || cursor;
}

export async function triggerGrokBot(input: GrokTriggerInput): Promise<GrokTriggerResult> {
  const hookUrl = resolveAgentHookUrl(input.target ?? "auto");
  const sessionId = process.env.GROK_SESSION_ID?.trim() ?? "";
  const repoUrl = (input.repoUrl ?? process.env.GROK_DEFAULT_REPO_URL ?? "").trim();

  if (repoUrl.length > MAX_REPO_URL) {
    return { ok: false, error: "repo_url_too_long" };
  }
  if (!input.prompt.trim()) {
    return { ok: false, error: "empty_prompt" };
  }

  const requestId =
    input.clientRef?.slice(0, 128) ?? `pwa-${Date.now().toString(36)}`;

  const message = buildGrokMessage(
    input,
    repoUrl || process.env.GROK_DEFAULT_REPO_URL || "Main Workspace",
  );

  const callbackUrl =
    process.env.GROK_CALLBACK_PUBLIC_URL?.trim() ||
    (process.env.VERCEL_URL
      ? `https://${process.env.VERCEL_URL}/api/agent/webhook`
      : "");

  const payload = {
    session_id: sessionId,
    message,
    protocol: "acp/0.1",
    message_type: "task",
    client_ref: requestId,
    callback_url: callbackUrl || undefined,
    context: {
      repo_url: repoUrl || null,
      preset: input.preset ?? null,
      source: "nextjs-remote-pwa",
    },
  };

  if (!hookUrl) {
    return {
      ok: true,
      dryRun: true,
      status: 202,
      requestId,
      platformStatus: "DryRun",
      detail:
        "Set GROK_INFERENCE_HOOK_URL and/or CURSOR_AGENT_API_URL in Vercel env (never expose keys to the phone).",
    };
  }

  const headers: Record<string, string> = {
    "Content-Type": "application/json",
    "X-ACP-Protocol": "acp/0.1",
  };

  const cursorKey = process.env.CURSOR_API_KEY?.trim();
  const grokToken = process.env.GROK_BOT_TOKEN?.trim();
  if (cursorKey) headers.Authorization = `Bearer ${cursorKey}`;
  else if (grokToken) headers.Authorization = `Bearer ${grokToken}`;

  const signing = process.env.GROK_SIGNING_SECRET?.trim();
  if (signing) headers["X-Grok-Signing-Secret"] = signing;

  try {
    const res = await fetch(hookUrl, {
      method: "POST",
      headers,
      body: JSON.stringify(payload),
      signal: AbortSignal.timeout(45_000),
    });

    const errorText = await res.text();
    if (!res.ok) {
      console.error("Agent platform error:", res.status, errorText.slice(0, 2000));
      return { ok: false, error: "target_platform_rejected" };
    }

    let platformStatus = "Active/Running";
    try {
      const data = JSON.parse(errorText) as Record<string, unknown>;
      platformStatus = String(data.status ?? data.state ?? platformStatus);
    } catch {
      if (errorText.trim()) platformStatus = errorText.trim().slice(0, 240);
    }

    return {
      ok: true,
      dryRun: false,
      status: res.status,
      requestId,
      platformStatus,
    };
  } catch (err) {
    console.error("Internal agent hook error:", err);
    return { ok: false, error: "agent_request_failed" };
  }
}
