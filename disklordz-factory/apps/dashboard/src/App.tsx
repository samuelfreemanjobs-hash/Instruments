import { useCallback, useEffect, useState } from "react";
import {
  api,
  type ApprovalItem,
  type FactoryDashboard,
  type NightShiftRun,
  type NightShiftStep,
} from "./api";

function ProgressBar({ label, value }: { label: string; value: number }) {
  return (
    <div>
      <div className="bar-label">
        <span>{label}</span>
        <span>{value}%</span>
      </div>
      <div className="bar">
        <span style={{ width: `${value}%` }} />
      </div>
    </div>
  );
}

function money(n: number) {
  return n.toLocaleString(undefined, { style: "currency", currency: "USD" });
}

export default function App() {
  const [dash, setDash] = useState<FactoryDashboard | null>(null);
  const [queue, setQueue] = useState<ApprovalItem[]>([]);
  const [lastRun, setLastRun] = useState<NightShiftRun | null>(null);
  const [schedule, setSchedule] = useState<NightShiftStep[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const refresh = useCallback(async () => {
    setError(null);
    const [d, q, run, sched] = await Promise.all([
      api.dashboard(),
      api.approvalQueue(),
      api.latestNightShift(),
      api.nightShiftSchedule(),
    ]);
    setDash(d);
    setQueue(q);
    setLastRun(run);
    setSchedule(sched);
  }, []);

  useEffect(() => {
    refresh().catch((e: Error) => setError(e.message));
  }, [refresh]);

  async function pingSlackAgents() {
    setLoading(true);
    setError(null);
    try {
      const res = await api.slackCheckin();
      if (res && typeof res === "object" && "skipped" in res && res.skipped) {
        setError("Slack webhook not configured (SLACK_FACTORY_WEBHOOK_URL)");
      }
    } catch (e) {
      setError(e instanceof Error ? e.message : "Slack check-in failed");
    } finally {
      setLoading(false);
    }
  }

  async function runNightShift() {
    setLoading(true);
    setError(null);
    try {
      const run = await api.runNightShift();
      setLastRun(run);
      await refresh();
    } catch (e) {
      setError(e instanceof Error ? e.message : "Night shift failed");
    } finally {
      setLoading(false);
    }
  }

  async function decide(assetId: string, action: "approve" | "reject") {
    setError(null);
    try {
      if (action === "approve") {
        await api.approve(assetId);
      } else {
        await api.reject(assetId);
      }
      await refresh();
    } catch (e) {
      setError(e instanceof Error ? e.message : "Action failed");
    }
  }

  return (
    <div className="app">
      <header className="header">
        <div>
          <h1>DISKLORDZ FACTORY</h1>
          <p>Autonomous batch manufacturing · human approval at 07:00</p>
        </div>
        <div className="actions">
          <button type="button" onClick={() => refresh()} disabled={loading}>
            Refresh
          </button>
          <button type="button" onClick={pingSlackAgents} disabled={loading}>
            Ping Slack agents
          </button>
          <button type="button" className="primary" onClick={runNightShift} disabled={loading}>
            {loading ? "Running night shift…" : "Run night shift"}
          </button>
        </div>
      </header>

      {error && <p className="error">{error}</p>}

      <div className="grid">
        <section className="panel">
          <h2>Active projects · Today</h2>
          <div className="stat-row">
            <span>Tracks / batches</span>
            <strong>{dash?.active_projects ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>EPs (est.)</span>
            <strong>{dash?.active_eps ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Products</span>
            <strong>{dash?.active_products ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Generated today</span>
            <strong>{dash?.today_generated ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Approved today</span>
            <strong>{dash?.today_approved ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Assets extracted</span>
            <strong>{dash?.today_extracted ?? "—"}</strong>
          </div>
        </section>

        <section className="panel">
          <h2>Catalog · Revenue</h2>
          <div className="stat-row">
            <span>Audio assets</span>
            <strong>{dash?.catalog_audio_assets ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Tracks</span>
            <strong>{dash?.catalog_tracks ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Products</span>
            <strong>{dash?.catalog_products ?? "—"}</strong>
          </div>
          <div className="stat-row">
            <span>Revenue (placeholder)</span>
            <strong>{money(dash?.revenue_total_usd ?? 0)}</strong>
          </div>
        </section>

        <section className="panel">
          <h2>Factory queue</h2>
          <ProgressBar label="Production" value={dash?.queue_production ?? 0} />
          <ProgressBar label="QA" value={dash?.queue_qa ?? 0} />
          <ProgressBar label="Publishing prep" value={dash?.queue_publishing ?? 0} />
          {lastRun && (
            <div className="night-log">
              Last run: {lastRun.run_id} · {lastRun.approval_items} items awaiting you ·{" "}
              {lastRun.status}
            </div>
          )}
        </section>
      </div>

      <section className="panel" style={{ marginBottom: "1rem" }}>
        <h2>Night shift schedule</h2>
        <div className="stat-row">
          <span>Steps</span>
          <strong>{schedule.length}</strong>
        </div>
        <div className="night-log" style={{ maxHeight: "160px" }}>
          {schedule.map((s) => (
            <div key={`${s.schedule_time}-${s.label}`}>
              [{s.schedule_time}] {s.label}
            </div>
          ))}
        </div>
      </section>

      <section className="panel">
        <h2>Approval queue ({queue.length} pending)</h2>
        {queue.length === 0 ? (
          <p style={{ color: "var(--muted)", fontSize: "0.8rem" }}>
            Queue empty — run night shift to populate morning approvals.
          </p>
        ) : (
          <ul className="approval-list">
            {queue.map((item) => (
              <li key={item.asset_id}>
                <div className="approval-title">
                  {item.asset_id} — {item.title}
                </div>
                <div className="qa-tags">
                  <span className={item.audio_qa === "pass" ? "pass" : ""}>Audio {item.audio_qa}</span>
                  <span className={item.visual_qa === "pass" ? "pass" : ""}>Visual {item.visual_qa}</span>
                  <span className={item.product_qa === "pass" ? "pass" : ""}>Product {item.product_qa}</span>
                  <span className={item.rights_qa === "pass" ? "pass" : ""}>Rights {item.rights_qa}</span>
                </div>
                <div className="row-actions">
                  <button type="button" className="primary" onClick={() => decide(item.asset_id, "approve")}>
                    Approve
                  </button>
                  <button type="button" onClick={() => decide(item.asset_id, "reject")}>
                    Reject
                  </button>
                </div>
              </li>
            ))}
          </ul>
        )}
      </section>
    </div>
  );
}
