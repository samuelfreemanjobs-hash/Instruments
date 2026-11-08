import type { AlertSettings, WatchEntry } from "../types";
import type { ConditionFilter, ShoppingSeason } from "../types";

const WATCH_KEY = "deal-hunter-watch";
const ALERT_KEY = "deal-hunter-alerts";
const LOCAL_LIST_KEY = "deal-hunter-local-list";
const UI_KEY = "deal-hunter-ui";
const LIST_DONE_KEY = "deal-hunter-list-done";
const TARGET_WATCH_KEY = "deal-hunter-target-watches";
const ASSISTANT_KEY = "deal-hunter-assistant-log";

export const DEFAULT_ALERTS: AlertSettings = {
  minStealScore: 85,
  notifyPriceDrops: true,
  notifySteals: true,
};

export interface UiPreferences {
  easyMode: boolean;
  conditionFilter: ConditionFilter;
  couponsOnly: boolean;
  seasonOverride: ShoppingSeason | null;
  showStealsOnly: boolean;
  applyStudentDiscount: boolean;
}

export const DEFAULT_UI: UiPreferences = {
  easyMode: true,
  conditionFilter: "include_used",
  couponsOnly: false,
  seasonOverride: null,
  showStealsOnly: false,
  applyStudentDiscount: false,
};

export interface TargetPriceWatch {
  id: string;
  productQuery: string;
  targetPrice: number;
  createdAt: string;
  notifiedAt?: string;
}

export interface AssistantLogEntry {
  role: "user" | "assistant";
  text: string;
  at: string;
}

export function loadWatchlist(): WatchEntry[] {
  try {
    const raw = localStorage.getItem(WATCH_KEY);
    return raw ? (JSON.parse(raw) as WatchEntry[]) : [];
  } catch {
    return [];
  }
}

export function saveWatchlist(items: WatchEntry[]) {
  localStorage.setItem(WATCH_KEY, JSON.stringify(items));
}

export function loadTargetWatches(): TargetPriceWatch[] {
  try {
    const raw = localStorage.getItem(TARGET_WATCH_KEY);
    return raw ? (JSON.parse(raw) as TargetPriceWatch[]) : [];
  } catch {
    return [];
  }
}

export function saveTargetWatches(watches: TargetPriceWatch[]) {
  localStorage.setItem(TARGET_WATCH_KEY, JSON.stringify(watches));
}

export function loadAssistantLog(): AssistantLogEntry[] {
  try {
    const raw = localStorage.getItem(ASSISTANT_KEY);
    return raw ? (JSON.parse(raw) as AssistantLogEntry[]) : [];
  } catch {
    return [];
  }
}

export function saveAssistantLog(entries: AssistantLogEntry[]) {
  localStorage.setItem(ASSISTANT_KEY, JSON.stringify(entries.slice(-40)));
}

export function loadAlertSettings(): AlertSettings {
  try {
    const raw = localStorage.getItem(ALERT_KEY);
    return raw ? { ...DEFAULT_ALERTS, ...(JSON.parse(raw) as AlertSettings) } : DEFAULT_ALERTS;
  } catch {
    return DEFAULT_ALERTS;
  }
}

export function saveAlertSettings(settings: AlertSettings) {
  localStorage.setItem(ALERT_KEY, JSON.stringify(settings));
}

export interface LocalListState {
  title: string;
  items: { dealId: string; note?: string; addedAt: string }[];
  shareCode?: string;
}

export function loadLocalList(): LocalListState {
  try {
    const raw = localStorage.getItem(LOCAL_LIST_KEY);
    return raw
      ? (JSON.parse(raw) as LocalListState)
      : { title: "Our Black Friday list", items: [] };
  } catch {
    return { title: "Our Black Friday list", items: [] };
  }
}

export function saveLocalList(state: LocalListState) {
  localStorage.setItem(LOCAL_LIST_KEY, JSON.stringify(state));
}

export function loadUiPreferences(): UiPreferences {
  try {
    const raw = localStorage.getItem(UI_KEY);
    return raw ? { ...DEFAULT_UI, ...(JSON.parse(raw) as UiPreferences) } : DEFAULT_UI;
  } catch {
    return DEFAULT_UI;
  }
}

export function saveUiPreferences(prefs: UiPreferences) {
  localStorage.setItem(UI_KEY, JSON.stringify(prefs));
}

export function loadListDone(): Record<string, boolean> {
  try {
    const raw = localStorage.getItem(LIST_DONE_KEY);
    return raw ? (JSON.parse(raw) as Record<string, boolean>) : {};
  } catch {
    return {};
  }
}

export function saveListDone(done: Record<string, boolean>) {
  localStorage.setItem(LIST_DONE_KEY, JSON.stringify(done));
}
