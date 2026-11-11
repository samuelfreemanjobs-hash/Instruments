import { useEffect, useState } from "react";
import { DealCard } from "../components/DealCard";
import { fetchDashboard } from "../lib/api";
import type { DashboardPayload, Deal } from "../types";

interface Props {
  stealThreshold: number;
  onOpenSearch: () => void;
  onWatch: (d: Deal) => void;
  onAddList: (d: Deal) => void;
  watchIds: Set<string>;
  listIds: Set<string>;
}

function Row({
  title,
  subtitle,
  deals,
  ...card
}: {
  title: string;
  subtitle: string;
  deals: Deal[];
} & Omit<Props, "onOpenSearch">) {
  if (deals.length === 0) return null;
  return (
    <section className="dash-row">
      <header>
        <h2>{title}</h2>
        <p>{subtitle}</p>
      </header>
      <div className="deal-grid tight">
        {deals.map((deal) => (
          <DealCard
            key={deal.id}
            deal={deal}
            stealThreshold={card.stealThreshold}
            onWatch={card.onWatch}
            onAddList={card.onAddList}
            isWatched={card.watchIds.has(deal.id)}
            inList={card.listIds.has(deal.id)}
            compact
          />
        ))}
      </div>
    </section>
  );
}

export function HqDashboard(props: Props) {
  const [data, setData] = useState<DashboardPayload | null>(null);
  const [loading, setLoading] = useState(true);

  const load = () => {
    setLoading(true);
    void fetchDashboard()
      .then(setData)
      .finally(() => setLoading(false));
  };

  useEffect(() => {
    load();
    const id = setInterval(load, 90_000);
    return () => clearInterval(id);
  }, []);

  return (
    <div className="hq-dashboard">
      <HqHero fetchedAt={data?.fetchedAt} loading={loading} onRefresh={load} onSearch={props.onOpenSearch} />
      {loading && !data ? (
        <p className="empty">Refreshing live deals…</p>
      ) : data ? (
        <>
          <Row title="Hidden gems" subtitle="Lesser-known bargains that score high on value." deals={data.hiddenGems} {...props} />
          <Row title="Price alerts" subtitle="Drops and top-tier scores right now." deals={data.priceAlerts} {...props} />
          <Row title="Trending now" subtitle="Popular products gaining attention." deals={data.trendingNow} {...props} />
          <Row title="Ending soon" subtitle="Act before these promos expire." deals={data.endingSoon} {...props} />
        </>
      ) : null}
    </div>
  );
}

function HqHero({
  fetchedAt,
  loading,
  onRefresh,
  onSearch,
}: {
  fetchedAt?: string;
  loading: boolean;
  onRefresh: () => void;
  onSearch: () => void;
}) {
  const t = fetchedAt ? new Date(fetchedAt).toLocaleTimeString() : "—";
  return (
    <div className="hq-hero">
      <div>
        <p className="tagline">Shop smarter. Pay less.</p>
        <h1 className="hq-title">Black Friday HQ</h1>
        <p className="hq-sub">Deals ranked first — updated {t} · prices re-checked every 90s</p>
      </div>
      <div className="hq-actions">
        <button type="button" className="btn-primary" onClick={onSearch}>
          Universal search
        </button>
        <button type="button" className="btn-ghost" onClick={onRefresh} disabled={loading}>
          Refresh now
        </button>
      </div>
    </div>
  );
}
