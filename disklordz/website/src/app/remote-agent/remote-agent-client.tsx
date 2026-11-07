"use client";

import { useCallback, useEffect, useRef, useState } from "react";

import { GROK_PRESETS, type GrokPresetId } from "@/lib/grok/presets";

type Msg = { role: "user" | "assistant" | "system"; text: string };

async function notifyPhone(title: string, body: string) {
  if (typeof window === "undefined" || !("Notification" in window)) return;
  if (Notification.permission === "granted") {
    new Notification(title, { body: body.slice(0, 240) });
  }
}

export function RemoteAgentClient() {
  const [messages, setMessages] = useState<Msg[]>([
    {
      role: "system",
      text: "Triggers Grok Bot. Enable notifications for completion pings.",
    },
  ]);
  const [prompt, setPrompt] = useState("");
  const [busy, setBusy] = useState(false);
  const pollRef = useRef<ReturnType<typeof setInterval> | null>(null);

  const push = useCallback((role: Msg["role"], text: string) => {
    setMessages((m) => [...m, { role, text }]);
  }, []);

  const startPolling = useCallback(
    (clientRef: string) => {
      if (pollRef.current) clearInterval(pollRef.current);
      const since = new Date().toISOString();
      pollRef.current = setInterval(() => {
        void (async () => {
          try {
            const res = await fetch(
              `/api/agent/events?clientRef=${encodeURIComponent(clientRef)}&since=${encodeURIComponent(since)}`,
            );
            const data = await res.json();
            for (const ev of data.events ?? []) {
              const line = `[Grok ${ev.status}] ${ev.summary}`;
              push("assistant", line);
              void notifyPhone("Grok task update", ev.summary);
            }
          } catch {
            /* ignore poll errors */
          }
        })();
      }, 12_000);
    },
    [push],
  );

  useEffect(() => {
    return () => {
      if (pollRef.current) clearInterval(pollRef.current);
    };
  }, []);

  const fire = useCallback(
    async (body: { prompt?: string; preset?: GrokPresetId }) => {
      if (typeof window !== "undefined" && "Notification" in window) {
        if (Notification.permission === "default") {
          await Notification.requestPermission();
        }
      }
      setBusy(true);
      const clientRef = `mobile-${Date.now()}`;
      try {
        const res = await fetch("/api/agent", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ ...body, clientRef }),
        });
        const data = await res.json();
        if (body.prompt) push("user", body.prompt);
        else if (body.preset) push("user", `[${GROK_PRESETS[body.preset].label}]`);
        if (data.success) {
          const tag = data.dryRun ? "[dry-run] " : "";
          const rid = data.requestId ?? clientRef;
          push(
            "assistant",
            `${tag}Task sent (${rid}). Status: ${data.status ?? "queued"}. Watching webhook…`,
          );
          if (!data.dryRun) startPolling(rid);
        } else {
          push("assistant", `Error: ${data.error ?? "failed"}`);
        }
      } catch {
        push("assistant", "Network error");
      } finally {
        setBusy(false);
      }
    },
    [push, startPolling],
  );

  return (
    <div className="flex min-h-dvh flex-col bg-zinc-950 text-zinc-100">
      <header className="border-b border-zinc-800 px-4 py-3">
        <h1 className="text-lg font-semibold">Remote Grok</h1>
        <p className="text-sm text-zinc-400">Webhook controller — Claude Code stays local</p>
      </header>

      <div className="flex flex-1 flex-col gap-2 overflow-y-auto p-4">
        {messages.map((m, i) => (
          <div
            key={i}
            className={`max-w-[95%] rounded-xl px-3 py-2 text-sm leading-relaxed ${
              m.role === "user"
                ? "ml-auto bg-zinc-800"
                : m.role === "system"
                  ? "border border-dashed border-zinc-700 text-zinc-500"
                  : "border border-zinc-800 bg-zinc-900"
            }`}
          >
            {m.text}
          </div>
        ))}
      </div>

      <motionPad busy={busy} onPreset={(id) => void fire({ preset: id })} />

      <form
        className="flex gap-2 border-t border-zinc-800 bg-zinc-950 p-3"
        onSubmit={(e) => {
          e.preventDefault();
          const p = prompt.trim();
          if (!p || busy) return;
          setPrompt("");
          void fire({ prompt: p });
        }}
      >
        <textarea
          className="min-h-[3rem] flex-1 resize-none rounded-lg border border-zinc-700 bg-zinc-900 px-3 py-2 text-sm"
          placeholder="Custom task for Grok Bot…"
          value={prompt}
          onChange={(e) => setPrompt(e.target.value)}
          rows={2}
        />
        <button
          type="submit"
          disabled={busy}
          className="rounded-lg bg-blue-600 px-4 font-medium disabled:opacity-50"
        >
          Send
        </button>
      </form>
    </div>
  );
}

function motionPad({
  busy,
  onPreset,
}: {
  busy: boolean;
  onPreset: (id: GrokPresetId) => void;
}) {
  const ids = Object.keys(GROK_PRESETS) as GrokPresetId[];
  return (
    <div className="grid grid-cols-1 gap-2 px-3 pb-2 sm:grid-cols-3">
      {ids.map((id) => (
        <button
          key={id}
          type="button"
          disabled={busy}
          onClick={() => onPreset(id)}
          className="rounded-xl border border-zinc-700 bg-zinc-900 px-3 py-3 text-left text-sm disabled:opacity-50"
        >
          <div className="font-medium">{GROK_PRESETS[id].label}</div>
          <div className="text-zinc-500">{GROK_PRESETS[id].description}</div>
        </button>
      ))}
    </div>
  );
}
