import { useEffect, useState } from "react";
import { fetchAutopilotAgents, fetchAutopilotDigest } from "../lib/api";
import { loadShopperProfile } from "../lib/storage";
import type { AutonomousAgentMeta, AutopilotDigest } from "../types";

export function AutopilotPage() {
  const [agents, setAgents] = useState<AutonomousAgentMeta[]>([]);
  const [digest, setDigest] = useState<AutopilotDigest | null>(null);
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    void fetchAutopilotAgents().then((r) => setAgents(r.agents));
  }, []);

  const run = () => {
    setLoading(true);
    const profile = loadShopperProfile();
    void fetchAutopilotDigest(profile)
      .then(setDigest)
      .finally(() => setLoading(false));
  };

  useEffect(() => {
    run();
    const id = setInterval(run, 180_000);
    return () => clearInterval(id);
  }, []);

  return (
    <div className="autopilot-page">
      <header className="panel-head">
        <h2>Autopilot</h2>
        <p>
          Specialized agents hunt for you — no endless scrolling. n8n schedules refresh; email & push fire on steals.
        </p>
        <button type="button" className="btn-primary" disabled={loading} onClick={run}>
          {loading ? "Running…" : "Run all agents now"}
        </button>
      </header>

      <ul className="agent-roster">
        {agents.map((a) => (
          <li key={a.id}>
            <h3>{a.name}</h3>
            <p>{a.mission}</p>
            <span className="agent-schedule">{a.scheduleHint}</span>
          </li>
        ))}
      </ul>

      {digest && (
        <>
          <p className="fresh-line">
            Run {digest.runId} · {new Date(digest.generatedAt).toLocaleTimeString()}
          </p>
          {digest.topVerdicts.length > 0 && (
            <section className="panel">
              <h3>Top value verdicts</h3>
              <ul>
                {digest.topVerdicts.map((v) => (
                  <li key={v.slice(0, 40)}>{v}</li>
                ))}
              </ul>
            </section>
          )}
          {digest.agents.map((run) => (
            <section key={run.agentId} className="panel agent-run">
              <h3>{run.title}</h3>
              <p className="agent-narrative">{run.narrative}</p>
              <ol>
                {run.picks.map((p) => (
                  <li key={p.id}>
                    <strong>{p.label}</strong>
                    <span>{p.detail}</span>
                  </li>
                ))}
              </ol>
            </section>
          ))}
        </>
      )}
    </div>
  );
}
