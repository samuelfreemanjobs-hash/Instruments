import { CalendarClient } from "./calendar-client";

export default function CalendarPage() {
  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Deal calendar</h2>
        <p className="text-sm text-bfm-muted">Black Friday, Cyber Monday, and year-round shopping windows.</p>
      </header>
      <CalendarClient />
    </section>
  );
}
