"use client";

import { useState } from "react";
import { saveNotificationPrefs, type NotificationPrefs } from "./actions";
import { Button } from "@/components/ui/button";
import { Card, CardContent } from "@/components/ui/card";

const LABELS: { key: keyof NotificationPrefs; label: string }[] = [
  { key: "deal_alert_enabled", label: "Deal steals & drops" },
  { key: "price_alert_enabled", label: "Price target watches" },
  { key: "coupon_alert_enabled", label: "Verified coupon events" },
  { key: "push_enabled", label: "Browser / web push" },
  { key: "email_enabled", label: "Email digest (n8n)" },
];

export function SettingsClient({ initial }: { initial: NotificationPrefs }) {
  const [prefs, setPrefs] = useState(initial);
  const [msg, setMsg] = useState<string | null>(null);

  const toggle = (key: keyof NotificationPrefs) => {
    setPrefs((p) => ({ ...p, [key]: !p[key] }));
  };

  return (
    <Card>
      <CardContent className="space-y-3 pt-4">
        <h3 className="text-sm font-medium">Notifications</h3>
        <ul className="space-y-2 text-sm">
          {LABELS.map(({ key, label }) => (
            <li key={key} className="flex items-center justify-between gap-2">
              <span>{label}</span>
              <button
                type="button"
                role="switch"
                aria-checked={prefs[key]}
                className={`h-6 w-11 rounded-full px-0.5 transition ${prefs[key] ? "bg-bfm-accent" : "bg-bfm-border"}`}
                onClick={() => toggle(key)}
              >
                <span
                  className={`block h-5 w-5 rounded-full bg-white transition ${prefs[key] ? "translate-x-5" : "translate-x-0"}`}
                />
              </button>
            </li>
          ))}
        </ul>
        <Button type="button" onClick={() => void saveNotificationPrefs(prefs).then((r) => setMsg(r.ok ? "Saved" : r.error ?? "Error"))}>
          Save preferences
        </Button>
        {msg && <p className="text-xs text-bfm-muted">{msg}</p>}
      </CardContent>
    </Card>
  );
}
