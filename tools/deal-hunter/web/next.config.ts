import type { NextConfig } from "next";
import path from "node:path";
import { fileURLToPath } from "node:url";

import { DEAL_API_PROXY_SOURCES } from "./deal-api-proxy-allowlist";

const apiOrigin = process.env.DEAL_API_ORIGIN ?? "http://127.0.0.1:3847";
const webRoot = path.dirname(fileURLToPath(import.meta.url));

const nextConfig: NextConfig = {
  reactStrictMode: true,
  outputFileTracingRoot: webRoot,
  async rewrites() {
    return DEAL_API_PROXY_SOURCES.map((source) => ({
      source,
      destination: `${apiOrigin}${source}`,
    }));
  },
};

export default nextConfig;
