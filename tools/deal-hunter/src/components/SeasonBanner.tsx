import type { SeasonInfo, ShoppingSeason } from "../types";
import { SEASON_LABELS } from "../types";

interface Props {
  season: SeasonInfo | null;
  override: ShoppingSeason | null;
  onOverride?: (s: ShoppingSeason | null) => void;
}

const ALL_SEASONS: ShoppingSeason[] = ["everyday", "black_friday", "cyber_monday", "holiday", "grocery"];

export function SeasonBanner({ season, override, onOverride }: Props) {
  if (!season) return null;
  const active = override ?? season.season;

  return (
    <section className={`season-banner${season.peakEvent ? " peak" : ""}`} aria-live="polite">
      <div>
        <p className="season-kicker">{season.peakEvent ? "Peak event" : "Year-round intelligence"}</p>
        <h2 className="season-title">{SEASON_LABELS[active]}</h2>
        <p className="season-focus">{season.focus}</p>
      </div>
      <div className="season-switch">
        <label htmlFor="season-pick">Shopping focus</label>
        <select
          id="season-pick"
          value={override ?? "auto"}
          onChange={(e) => {
            const v = e.target.value;
            onOverride?.(v === "auto" ? null : (v as ShoppingSeason));
          }}
        >
          <option value="auto">Auto ({season.label})</option>
          {ALL_SEASONS.map((s) => (
            <option key={s} value={s}>
              {SEASON_LABELS[s]}
            </option>
          ))}
        </select>
      </div>
    </section>
  );
}
