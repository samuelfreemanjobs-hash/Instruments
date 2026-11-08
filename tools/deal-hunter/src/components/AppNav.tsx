/** PRD Part 3 — primary navigation (mobile-first) */

export type AppTab =
  | "assistant"
  | "hq"
  | "discover"
  | "search"
  | "intelligence"
  | "autopilot"
  | "coupons"
  | "compare"
  | "list"
  | "budget"
  | "calendar"
  | "alerts"
  | "settings"
  | "pricing";

const PRIMARY: { id: AppTab; label: string }[] = [
  { id: "hq", label: "Dashboard" },
  { id: "discover", label: "Discover" },
  { id: "search", label: "Search" },
  { id: "assistant", label: "Assistant" },
  { id: "alerts", label: "Alerts" },
];

const MORE: { id: AppTab; label: string }[] = [
  { id: "intelligence", label: "Intel" },
  { id: "autopilot", label: "Autopilot" },
  { id: "coupons", label: "Coupons" },
  { id: "compare", label: "Compare" },
  { id: "list", label: "Wishlist" },
  { id: "budget", label: "Budget" },
  { id: "calendar", label: "Calendar" },
  { id: "pricing", label: "Pricing AI" },
  { id: "settings", label: "Settings" },
];

interface Props {
  tab: AppTab;
  onTab: (t: AppTab) => void;
}

export function AppNav({ tab, onTab }: Props) {
  const inMore = MORE.some((m) => m.id === tab);

  return (
    <nav className="app-nav" aria-label="Primary">
      <div className="nav-primary">
        {PRIMARY.map(({ id, label }) => (
          <button
            key={id}
            type="button"
            className={`nav-btn${tab === id ? " active" : ""}`}
            onClick={() => onTab(id)}
          >
            {label}
          </button>
        ))}
      </div>
      <details className="nav-more" open={inMore}>
        <summary>More</summary>
        <div className="nav-more-grid">
          {MORE.map(({ id, label }) => (
            <button
              key={id}
              type="button"
              className={`nav-btn${tab === id ? " active" : ""}`}
              onClick={() => onTab(id)}
            >
              {label}
            </button>
          ))}
        </div>
      </details>
    </nav>
  );
}
