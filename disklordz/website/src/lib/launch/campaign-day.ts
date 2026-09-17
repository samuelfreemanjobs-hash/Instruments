/** Campaign day index (1 = Day 1) from LAUNCH_CAMPAIGN_START ISO env. */

export function getLaunchCampaignDay(now = new Date()): number {
  const startRaw = process.env.LAUNCH_CAMPAIGN_START;
  if (!startRaw) return 1;
  const start = new Date(startRaw);
  if (Number.isNaN(start.getTime())) return 1;
  const ms = now.getTime() - start.getTime();
  if (ms < 0) return 0;
  return Math.floor(ms / (24 * 60 * 60 * 1000)) + 1;
}

export function isTripwireLive(now = new Date()): boolean {
  if (process.env.LAUNCH_TRIPWIRE_ENABLED === "true") return true;
  return getLaunchCampaignDay(now) >= 2;
}
