/** Official retailer host allowlist — blocks lookalike / unknown checkout links */
export const TRUSTED_HOSTS = new Set([
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

const SCAM_HINTS = [
  /bit\.ly/i,
  /t\.co\//i,
  /telegram/i,
  /whatsapp\.com/i,
  /crypto/i,
  /gift\s*card\s*only/i,
];

export interface TrustReport {
  verifiedRetailer: boolean;
  safeUrl: boolean;
  hostname: string;
  shopperTips: string[];
}

export function assessDealUrl(url: string): TrustReport {
  let hostname = "";
  try {
    hostname = new URL(url).hostname.toLowerCase();
  } catch {
    return {
      verifiedRetailer: false,
      safeUrl: false,
      hostname: "",
      shopperTips: [
        "This link could not be verified. Shop only on sites you type in yourself.",
      ],
    };
  }

  const verifiedRetailer = TRUSTED_HOSTS.has(hostname);
  const scamPattern = SCAM_HINTS.some((re) => re.test(url));
  const safeUrl = verifiedRetailer && !scamPattern;

  const shopperTips: string[] = [
    "Pay with a credit card for chargeback protection.",
    "Skip sellers that demand gift cards, wire, or crypto.",
  ];
  if (!verifiedRetailer) {
    shopperTips.unshift("Retailer is not on our verified list — double-check the domain spelling.");
  }
  if (scamPattern) {
    shopperTips.unshift("This URL looks risky. We will not open it from Deal Hunter.");
  }

  return { verifiedRetailer, safeUrl, hostname, shopperTips };
}
