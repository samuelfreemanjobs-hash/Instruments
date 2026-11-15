import { useEffect, useState } from "react";
import {
  fetchCommunityDeals,
  fetchLocalDeals,
  fetchRecommendations,
  submitCommunityDeal,
  upvoteCommunityDeal,
} from "../lib/api";
import { loadShopperProfile, saveShopperProfile } from "../lib/storage";
import type { CommunityDeal, LocalDealOffer, ShoppingRecommendation } from "../types";
import { DealCard } from "../components/DealCard";

type Props = {
  deals: import("../types").Deal[];
  stealThreshold: number;
  onWatch: (d: import("../types").Deal) => void;
  onAddList: (d: import("../types").Deal) => void;
  watchIds: Set<string>;
  listIds: Set<string>;
  showStudent?: boolean;
};

export function DiscoverPage(props: Props) {
  const [community, setCommunity] = useState<CommunityDeal[]>([]);
  const [local, setLocal] = useState<{ metro: string; offers: LocalDealOffer[] } | null>(null);
  const [recs, setRecs] = useState<ShoppingRecommendation[]>([]);
  const [profile, setProfile] = useState(() => loadShopperProfile());
  const [tip, setTip] = useState({ title: "", retailer: "", price: "", url: "", note: "" });

  const refresh = () => {
    void fetchCommunityDeals().then((r) => setCommunity(r.deals));
    void fetchLocalDeals(profile.zip).then(setLocal);
    void fetchRecommendations(profile).then((r) => setRecs(r.recommendations));
  };

  useEffect(() => {
    refresh();
  }, [profile.zip]);

  const submitTip = async (e: React.FormEvent) => {
    e.preventDefault();
    const ok = await submitCommunityDeal({
      title: tip.title,
      retailer: tip.retailer,
      price: Number(tip.price),
      url: tip.url,
      category: "general",
      note: tip.note,
      zip: profile.zip,
    });
    if (ok) {
      setTip({ title: "", retailer: "", price: "", url: "", note: "" });
      refresh();
    }
  };

  const recDeals = props.deals.filter((d) => recs.some((r) => r.dealId === d.id));

  return (
    <div className="discover-page">
      <header className="panel-head">
        <h2>Discover</h2>
        <p>Community tips, local partner circulars, and personalized picks — authorized sources only.</p>
      </header>

      <section className="panel">
        <h3>Your profile</h3>
        <div className="profile-row">
          <label>
            ZIP (local)
            <input
              value={profile.zip ?? ""}
              onChange={(e) => {
                const next = { ...profile, zip: e.target.value };
                setProfile(next);
                saveShopperProfile(next);
              }}
              placeholder="78701"
            />
          </label>
          <label>
            Max budget $
            <input
              type="number"
              value={profile.maxBudget ?? 400}
              onChange={(e) => {
                const next = { ...profile, maxBudget: Number(e.target.value) };
                setProfile(next);
                saveShopperProfile(next);
              }}
            />
          </label>
        </div>
      </section>

      <section className="panel">
        <h3>Recommendations for you</h3>
        <ul className="rec-list">
          {recs.slice(0, 6).map((r) => (
            <li key={r.dealId}>
              <strong>{r.title}</strong>
              <span className="rec-meta">
                ${r.price.toFixed(2)} · {r.verdict} · score {r.score}
              </span>
              <p>{r.reason}</p>
            </li>
          ))}
        </ul>
        <div className="card-grid compact-grid">
          {recDeals.map((deal) => (
            <DealCard
              key={deal.id}
              deal={deal}
              stealThreshold={props.stealThreshold}
              onWatch={props.onWatch}
              onAddList={props.onAddList}
              isWatched={props.watchIds.has(deal.id)}
              inList={props.listIds.has(deal.id)}
              showStudent={props.showStudent}
              compact
            />
          ))}
        </div>
      </section>

      <section className="panel">
        <h3>Local near {local?.metro ?? "you"}</h3>
        <ul className="local-list">
          {(local?.offers ?? []).map((o) => (
            <li key={o.id}>
              <a href={o.url} target="_blank" rel="noreferrer noopener">
                {o.title}
              </a>
              <span>
                {o.storeName} · {o.distanceMiles} mi · {o.source.replace("_", " ")}
              </span>
            </li>
          ))}
        </ul>
      </section>

      <section className="panel">
        <h3>Community deals</h3>
        <ul className="community-list">
          {community.map((c) => (
            <li key={c.id}>
              <div>
                <strong>{c.title}</strong> — {c.retailer} ${c.price.toFixed(2)}
                {c.note && <p>{c.note}</p>}
              </div>
              <button type="button" className="btn-ghost" onClick={() => void upvoteCommunityDeal(c.id).then(refresh)}>
                ▲ {c.upvotes}
              </button>
            </li>
          ))}
        </ul>
        <form className="tip-form" onSubmit={(e) => void submitTip(e)}>
          <input
            placeholder="Title"
            value={tip.title}
            onChange={(e) => setTip({ ...tip, title: e.target.value })}
            required
          />
          <input
            placeholder="Retailer"
            value={tip.retailer}
            onChange={(e) => setTip({ ...tip, retailer: e.target.value })}
            required
          />
          <input
            placeholder="Price"
            type="number"
            value={tip.price}
            onChange={(e) => setTip({ ...tip, price: e.target.value })}
            required
          />
          <input
            placeholder="Trusted store URL"
            value={tip.url}
            onChange={(e) => setTip({ ...tip, url: e.target.value })}
            required
          />
          <input placeholder="Note" value={tip.note} onChange={(e) => setTip({ ...tip, note: e.target.value })} />
          <button type="submit" className="btn-primary">
            Share tip
          </button>
        </form>
      </section>
    </div>
  );
}
