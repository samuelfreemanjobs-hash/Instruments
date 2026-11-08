export type ShoppingSeason =
  | "everyday"
  | "black_friday"
  | "cyber_monday"
  | "holiday"
  | "grocery";

export type PeakEvent = "black_friday" | "cyber_monday" | null;

export interface SeasonInfo {
  season: ShoppingSeason;
  label: string;
  focus: string;
  auto: boolean;
  peakEvent: PeakEvent;
  yearRound: boolean;
}

/** US retail calendar — override in the app if you want manual control */
export function detectSeason(now = new Date()): SeasonInfo {
  const y = now.getFullYear();
  const month = now.getMonth();
  const day = now.getDate();

  const thanksgiving = nthWeekdayOfMonth(y, 10, 4, 4);
  const blackFriday = addDays(thanksgiving, 1);
  const cyberMonday = addDays(blackFriday, 3);

  const t = new Date(y, month, day).getTime();

  if (t >= addDays(thanksgiving, -7).getTime() && t < blackFriday.getTime()) {
    return seasonInfo("black_friday", true, "black_friday");
  }
  if (t >= blackFriday.getTime() && t < cyberMonday.getTime()) {
    return seasonInfo("black_friday", true, "black_friday");
  }
  if (t >= cyberMonday.getTime() && t < addDays(cyberMonday, 3).getTime()) {
    return seasonInfo("cyber_monday", true, "cyber_monday");
  }
  if (month === 10 && day >= 1 && t < addDays(thanksgiving, -7).getTime()) {
    return seasonInfo("black_friday", true, "black_friday");
  }
  if (month === 10 && t >= addDays(cyberMonday, 3).getTime() && day <= 30) {
    return seasonInfo("holiday", true);
  }
  if (month === 11 && day <= 24) {
    return seasonInfo("holiday", true, null);
  }
  if (month === 0 || month === 1) {
    return seasonInfo("grocery", true, null);
  }
  return seasonInfo("everyday", true, null);
}

function seasonInfo(season: ShoppingSeason, auto: boolean, peakEvent: PeakEvent = null): SeasonInfo {
  const copy: Record<ShoppingSeason, { label: string; focus: string }> = {
    everyday: {
      label: "Year-round mode",
      focus:
        "Always-on deal intelligence — we rank real savings vs 30-day averages, not headline % off. Black Friday & Cyber Monday amplify steals when those windows open.",
    },
    black_friday: {
      label: "Black Friday — peak event",
      focus: "Our busiest week. Top steals first; verify final cart price with coupons.",
    },
    cyber_monday: {
      label: "Cyber Monday — peak event",
      focus: "Online codes and tech. Copy verified coupons at checkout.",
    },
    holiday: {
      label: "Holiday season",
      focus: "Gifts and shipping deadlines — trusted stores only, free shipping filter on.",
    },
    grocery: {
      label: "Groceries & home",
      focus: "Stack coupons on pantry, cleaning, and household basics.",
    },
  };
  const peak =
    peakEvent ??
    (season === "black_friday" ? "black_friday" : season === "cyber_monday" ? "cyber_monday" : null);
  return { season, auto, peakEvent: peak, yearRound: true, ...copy[season] };
}

function nthWeekdayOfMonth(year: number, month: number, weekday: number, n: number): Date {
  const d = new Date(year, month, 1);
  let count = 0;
  while (d.getMonth() === month) {
    if (d.getDay() === weekday) {
      count++;
      if (count === n) return new Date(d);
    }
    d.setDate(d.getDate() + 1);
  }
  return new Date(year, month, 28);
}

function addDays(d: Date, n: number): Date {
  const x = new Date(d);
  x.setDate(x.getDate() + n);
  return x;
}
