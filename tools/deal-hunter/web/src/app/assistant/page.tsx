import { AssistantClient } from "./assistant-client";

export default function AssistantPage() {
  return (
    <section className="space-y-3">
      <h2 className="text-lg font-semibold">AI shopping assistant</h2>
      <p className="text-sm text-bfm-muted">Chief orchestrator + evidence-backed recommendations. No autonomous checkout.</p>
      <AssistantClient />
    </section>
  );
}
