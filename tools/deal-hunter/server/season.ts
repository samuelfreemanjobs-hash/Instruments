export type ShoppingSeason =
  | "black_friday"
  | "cyber_monday"
  | "holiday"
  | "grocery";

export interface SeasonInfo {
  season: ShoppingSeason;
  label: string;
  focus: string;
  auto: boolean;
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
    return seasonInfo("black_friday", true);
  }
  if (t >= blackFriday.getTime() && t < cyberMonday.getTime()) {
    return seasonInfo("black_friday", true);
  }
  if (t >= cyberMonday.getTime() && t < addDays(cyberMonday, 3).getTime()) {
    return seasonInfo("cyber_monday", true);
  }
  if (month === 10 && day >= 1 && t < addDays(thanksgiving, -7).getTime()) {
    return seasonInfo("black_friday", true);
  }
  if (month === 10 && t >= addDays(cyberMonday, 3).getTime() && day <= 30) {
    return seasonInfo("holiday", true);
  }
  if (month === 11 && day <= 24) {
    return seasonInfo("holiday", true);
  }
  return seasonInfo("grocery", true);
}

function seasonInfo(season: ShoppingSeason, auto: boolean): SeasonInfo {
  const copy: Record<ShoppingSeason, { label: string; focus: string }> = {
    black_friday: {
      label: "Black Friday",
      focus: "One screen, top steals first. Add to your shared list before they expire.",
    },
    cyber_monday: {
      label: "Cyber Monday",
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
  return { season, auto, ...copy[season] };
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
