import fs from "fs";
import path from "path";

export type IntegrationRepo = {
  id: number;
  repo: string;
  url: string;
  status: "integrated" | "partial" | "external";
  wiring: string;
};

export type IntegrationManifest = {
  version: number;
  description: string;
  repos: IntegrationRepo[];
};

const manifestPath = path.join(process.cwd(), "..", "integrations", "manifest.json");

export function loadIntegrationManifest(): IntegrationManifest {
  const raw = fs.readFileSync(manifestPath, "utf8");
  return JSON.parse(raw) as IntegrationManifest;
}
