/** Retailer allowlist for outbound product links (mirrors deal API trust rules). */
const TRUSTED_HOSTS = new Set([
  "amazon.com",
  "www.amazon.com",
  "bestbuy.com",
  "www.bestbuy.com",
  "target.com",
  "www.target.com",
  "walmart.com",
  "www.walmart.com",
  "costco.com",
  "www.costco.com",
  "homedepot.com",
  "www.homedepot.com",
  "lowes.com",
  "www.lowes.com",
  "kohls.com",
  "www.kohls.com",
  "newegg.com",
  "www.newegg.com",
  "ebay.com",
  "www.ebay.com",
  "reebok.com",
  "www.reebok.com",
  "dyson.com",
  "www.dyson.com",
  "bose.com",
  "www.bose.com",
  "patagonia.com",
  "www.patagonia.com",
  "rei.com",
  "www.rei.com",
  "gamestop.com",
  "www.gamestop.com",
  "irobot.com",
  "www.irobot.com",
  "sharkclean.com",
  "www.sharkclean.com",
  "logitech.com",
  "www.logitech.com",
  "casper.com",
  "www.casper.com",
  "williams-sonoma.com",
  "www.williams-sonoma.com",
  "acmetools.com",
  "www.acmetools.com",
  "adidas.com",
  "www.adidas.com",
  "sephora.com",
  "www.sephora.com",
  "ulta.com",
  "www.ulta.com",
  "chewy.com",
  "www.chewy.com",
  "guitarcenter.com",
  "www.guitarcenter.com",
  "sweetwater.com",
  "www.sweetwater.com",
]);

const SCAM_HINTS = [/bit\.ly/i, /t\.co\//i, /telegram/i, /whatsapp\.com/i, /crypto/i, /gift\s*card\s*only/i];

const BLOCKED_PROTOCOLS = new Set(["javascript:", "data:", "vbscript:", "file:"]);

export function assessProductUrl(url: string): { safeUrl: boolean; hostname: string } {
  const trimmed = url.trim();
  if (!trimmed) return { safeUrl: false, hostname: "" };

  let parsed: URL;
  try {
    parsed = new URL(trimmed);
  } catch {
    return { safeUrl: false, hostname: "" };
  }

  if (parsed.protocol !== "http:" && parsed.protocol !== "https:") {
    return { safeUrl: false, hostname: "" };
  }
  if (BLOCKED_PROTOCOLS.has(parsed.protocol)) {
    return { safeUrl: false, hostname: parsed.hostname };
  }

  const hostname = parsed.hostname.toLowerCase();
  const scamPattern = SCAM_HINTS.some((re) => re.test(trimmed));
  const safeUrl = TRUSTED_HOSTS.has(hostname) && !scamPattern;
  return { safeUrl, hostname };
}
