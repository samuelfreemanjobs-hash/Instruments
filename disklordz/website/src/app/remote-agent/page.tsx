import type { Metadata, Viewport } from "next";

import { RemoteAgentClient } from "./remote-agent-client";

export const metadata: Metadata = {
  title: "Remote Grok Agent",
  description: "Mobile webhook controller for Grok Bot",
  manifest: "/remote-agent-manifest.json",
  appleWebApp: {
    capable: true,
    statusBarStyle: "black-translucent",
    title: "Grok Remote",
  },
};

export const viewport: Viewport = {
  themeColor: "#09090b",
  width: "device-width",
  initialScale: 1,
  viewportFit: "cover",
};

export default function RemoteAgentPage() {
  return <RemoteAgentClient />;
}
