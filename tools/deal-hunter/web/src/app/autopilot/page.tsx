import { AutopilotClient } from "./autopilot-client";

export default function AutopilotPage() {
  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Autopilot agents</h2>
        <p className="text-sm text-bfm-muted">Autonomous scouts backed by the deal API digest.</p>
      </header>
      <AutopilotClient />
    </section>
  );
}
