interface Props {
  title: string;
  prdRef: string;
  bullets: string[];
}

export function StubPage({ title, prdRef, bullets }: Props) {
  return (
    <section className="panel stub-page">
      <h2>{title}</h2>
      <p className="stub-ref">{prdRef}</p>
      <ul>
        {bullets.map((b) => (
          <li key={b}>{b}</li>
        ))}
      </ul>
      <p className="panel-hint">Phase 2: Supabase auth + Next.js routes. Demo data active elsewhere in the app.</p>
    </section>
  );
}
