import type { NextConfig } from "next";
import path from "node:path";
import { fileURLToPath } from "node:url";

import { DEAL_API_PROXY_SOURCES } from "./deal-api-proxy-allowlist";

const apiOrigin = process.env.DEAL_API_ORIGIN?.trim() ?? "";
const webRoot = path.dirname(fileURLToPath(import.meta.url));

const nextConfig: NextConfig = {
  reactStrictMode: true,
  outputFileTracingRoot: webRoot,
  async rewrites() {
    const local =
      !apiOrigin || /^https?:\/\/(127\.0\.0\.1|localhost)(:\d+)?\/?$/i.test(apiOrigin);
    if (local) return [];
    return DEAL_API_PROXY_SOURCES.map((source) => ({
      source,
      destination: `${apiOrigin}${source}`,
    }));
  },
};

export default nextConfig;
