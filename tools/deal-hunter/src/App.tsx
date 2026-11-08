import { useEffect, useMemo, useState } from "react";
import { SeasonBanner } from "./components/SeasonBanner";
import { useDealAlerts } from "./hooks/useAlerts";
import {
  createSharedList,
  fetchSeason,
  loadSharedList,
  saveSharedList,
  searchDeals,
} from "./lib/api";
import {
  loadAlertSettings,
  loadListDone,
  loadLocalList,
  loadUiPreferences,
  loadWatchlist,
  saveAlertSettings,
  saveListDone,
  saveLocalList,
  saveUiPreferences,
  saveWatchlist,
  type LocalListState,
} from "./lib/storage";
import { DecorAgentPage } from "./pages/DecorAgentPage";
import { HqDashboard } from "./pages/HqDashboard";
import { IntelligenceHub } from "./pages/IntelligenceHub";
import { PersonalAssistant } from "./pages/PersonalAssistant";
import { PricingAgentPage } from "./pages/PricingAgentPage";
import { UniversalSearchPage } from "./pages/UniversalSearchPage";
import { DiscoverPage } from "./pages/DiscoverPage";
import { AutopilotPage } from "./pages/AutopilotPage";
import type { Deal, SeasonInfo } from "./types";
import "./App.css";

type Tab =
  | "assistant"
  | "hq"
  | "intelligence"
  | "discover"
  | "autopilot"
  | "search"
  | "list"
  | "alerts"
  | "decor"
  | "pricing";

const TABS: { id: Tab; label: string }[] = [
  { id: "assistant", label: "Assistant" },
  { id: "hq", label: "HQ" },
  { id: "intelligence", label: "Intel" },
  { id: "discover", label: "Discover" },
  { id: "autopilot", label: "Autopilot" },
  { id: "search", label: "Search" },
  { id: "list", label: "List" },
  { id: "alerts", label: "Alerts" },
  { id: "pricing", label: "Pricing" },
];

export default function App() {
  const [tab, setTab] = useState<Tab>("assistant");
  const [deals, setDeals] = useState<Deal[]>([]);
  const [toast, setToast] = useState<string | null>(null);
  const [localList, setLocalList] = useState<LocalListState>(() => loadLocalList());
  const [watchlist, setWatchlist] = useState(() => loadWatchlist());
  const [alerts, setAlerts] = useState(() => loadAlertSettings());
  const [ui, setUi] = useState(() => loadUiPreferences());
  const [listDone, setListDone] = useState(() => loadListDone());
  const [seasonInfo, setSeasonInfo] = useState<SeasonInfo | null>(null);
  const [joinCode, setJoinCode] = useState("");

  const { requestPermission } = useDealAlerts(deals);

  useEffect(() => {
    void fetchSeason().then(setSeasonInfo);
    void searchDeals({ includeIntelligence: true }).then(setDeals);
    const id = setInterval(() => void searchDeals({ includeIntelligence: true }).then(setDeals), 90_000);
    return () => clearInterval(id);
  }, []);

  const showToast = (msg: string) => {
    setToast(msg);
    setTimeout(() => setToast(null), 2800);
  };

  const watchIds = useMemo(() => new Set(watchlist.map((w) => w.dealId)), [watchlist]);
  const listIds = useMemo(() => new Set(localList.items.map((i) => i.dealId)), [localList]);

  const watchDeal = (deal: Deal) => {
    const exists = watchlist.some((w) => w.dealId === deal.id);
    const next = exists
      ? watchlist.filter((x) => x.dealId !== deal.id)
      : [
          ...watchlist,
          {
            dealId: deal.id,
            alertSteal: true,
            lastKnownPrice: deal.price,
            title: deal.title,
          },
        ];
    setWatchlist(next);
    saveWatchlist(next);
    showToast(exists ? "Unwatched" : "Watching price");
  };

  const addToList = (deal: Deal) => {
    if (localList.items.some((i) => i.dealId === deal.id)) return;
    const next = {
      ...localList,
      items: [...localList.items, { dealId: deal.id, addedAt: new Date().toISOString() }],
    };
    setLocalList(next);
    saveLocalList(next);
    showToast("Added to list");
  };

  const cardProps = {
    stealThreshold: alerts.minStealScore,
    onWatch: watchDeal,
    onAddList: addToList,
    watchIds,
    listIds,
    showStudent: ui.applyStudentDiscount,
  };

  const publishList = async () => {
    try {
      if (localList.shareCode) {
        await saveSharedList(localList.shareCode, localList.title, localList.items);
        showToast(`Synced ${localList.shareCode}`);
      } else {
        const code = await createSharedList(localList.title, localList.items);
        const next = { ...localList, shareCode: code };
        setLocalList(next);
        saveLocalList(next);
        showToast(`Share code: ${code}`);
      }
    } catch {
      showToast("Sync failed");
    }
  };

  const joinList = async () => {
    const code = joinCode.trim().toUpperCase();
    if (!code) return;
    try {
      const data = await loadSharedList(code);
      const next = { title: data.title, items: data.raw.items, shareCode: data.shareCode };
      setLocalList(next);
      saveLocalList(next);
      setJoinCode("");
      setTab("list");
    } catch {
      showToast("Bad code");
    }
  };

  const listDeals = useMemo(() => {
    const map = new Map(deals.map((d) => [d.id, d]));
    return localList.items.map((i) => ({ item: i, deal: map.get(i.dealId) })).filter((x) => x.deal);
  }, [localList, deals]);

  const listProgress = useMemo(() => {
    const done = localList.items.filter((i) => listDone[i.dealId]).length;
    return { total: localList.items.length, done };
  }, [localList, listDone]);

  return (
    <div className="app-shell">
      <header className="hero compact-hero">
        <h1 className="brand-lockup">
          <span className="brand-word">Savvy</span>
          <span className="brand-accent">HQ</span>
        </h1>
        <p className="hero-lede">
          Year-round shopping intelligence — Black Friday & Cyber Monday are peak events, not the whole product.
        </p>
        <div className="safety-shield">
          <span className="shield-icon" aria-hidden>
            ◆
          </span>
          Real savings vs 30-day averages · not retailer % stickers · authorized feeds only
        </div>
      </header>

      <SeasonBanner
        season={seasonInfo}
        override={ui.seasonOverride}
        onOverride={(s) => {
          const prefs = { ...ui, seasonOverride: s };
          setUi(prefs);
          saveUiPreferences(prefs);
        }}
      />

      <nav className="tabs scroll-tabs" aria-label="Main">
        {TABS.map(({ id, label }) => (
          <button
            key={id}
            type="button"
            className={`tab${tab === id ? " active" : ""}`}
            onClick={() => setTab(id)}
          >
            {label}
          </button>
        ))}
      </nav>

      {tab === "assistant" && (
        <PersonalAssistant
          {...cardProps}
          onNotifyPermission={() => void requestPermission().then(() => showToast("Notifications on"))}
        />
      )}
      {tab === "hq" && <HqDashboard {...cardProps} onOpenSearch={() => setTab("search")} />}
      {tab === "intelligence" && <IntelligenceHub />}
      {tab === "discover" && <DiscoverPage deals={deals} {...cardProps} />}
      {tab === "autopilot" && <AutopilotPage />}
      {tab === "search" && (
        <UniversalSearchPage
          {...cardProps}
          conditionFilter={ui.conditionFilter}
          couponsOnly={ui.couponsOnly}
        />
      )}
      {tab === "decor" && <DecorAgentPage stealThreshold={alerts.minStealScore} onAddList={addToList} listIds={listIds} />}
      {tab === "pricing" && <PricingAgentPage />}

      {tab === "list" && (
        <section className="panel">
          <h2>Our list</h2>
          {listProgress.total > 0 && (
            <p>
              {listProgress.done}/{listProgress.total} done
            </p>
          )}
          <input
            className="stack-input"
            value={localList.title}
            onChange={(e) => {
              const next = { ...localList, title: e.target.value };
              setLocalList(next);
              saveLocalList(next);
            }}
          />
          <button type="button" className="btn-primary" onClick={() => void publishList()}>
            {localList.shareCode ? "Sync" : "Share code"}
          </button>
          <div className="share-box">
            <input placeholder="Partner code" value={joinCode} onChange={(e) => setJoinCode(e.target.value)} />
            <button type="button" className="btn-ghost" onClick={() => void joinList()}>
              Join
            </button>
          </div>
          {listDeals.map(({ item, deal }) => (
            <label key={item.dealId} className="list-check">
              <input
                type="checkbox"
                checked={!!listDone[item.dealId]}
                onChange={() => {
                  const next = { ...listDone, [item.dealId]: !listDone[item.dealId] };
                  setListDone(next);
                  saveListDone(next);
                }}
              />
              {deal!.title} — ${deal!.price.toFixed(2)}
            </label>
          ))}
        </section>
      )}

      {tab === "alerts" && (
        <section className="panel">
          <h2>Alerts</h2>
          <label className="row-check">
            <input
              type="checkbox"
              checked={ui.applyStudentDiscount}
              onChange={(e) => {
                const prefs = { ...ui, applyStudentDiscount: e.target.checked };
                setUi(prefs);
                saveUiPreferences(prefs);
                showToast(e.target.checked ? "Student pricing on cards" : "Student pricing off");
              }}
            />
            I qualify for student discounts (show lower “you pay” when eligible)
          </label>
          <label>
            Min steal score
            <input
              type="number"
              value={alerts.minStealScore}
              onChange={(e) => {
                const next = { ...alerts, minStealScore: Number(e.target.value) };
                setAlerts(next);
                saveAlertSettings(next);
              }}
            />
          </label>
          <button type="button" className="btn-primary" onClick={() => void requestPermission()}>
            Allow notifications
          </button>
          <p className="panel-hint">Target-price watches are set in Assistant (e.g. MPC → $299).</p>
        </section>
      )}

      {toast && <div className="toast">{toast}</div>}
    </div>
  );
}
