import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";

const here = path.dirname(fileURLToPath(import.meta.url));
const integrationsRoot = path.join(here, "..", "..", "..");
const manifestPath = path.join(integrationsRoot, "manifest.json");

function loadManifest(): { repos: unknown[] } {
  return JSON.parse(fs.readFileSync(manifestPath, "utf8"));
}

const server = new McpServer({
  name: "disklordz",
  version: "0.1.0",
});

server.tool(
  "integration_status",
  "Summarize Disklordz open-source integration manifest.",
  {},
  async () => ({
    content: [
      {
        type: "text",
        text: JSON.stringify(loadManifest(), null, 2),
      },
    ],
  }),
);

server.tool(
  "rag_corpus_chunk",
  "Print chunk count from disklordz/rag/data/chunks.jsonl if present.",
  {},
  async () => {
    const chunksPath = path.join(integrationsRoot, "..", "rag", "data", "chunks.jsonl");
    if (!fs.existsSync(chunksPath)) {
      return {
        content: [
          {
            type: "text",
            text: "No chunks.jsonl — run python3 disklordz/rag/scripts/chunk_corpus.py",
          },
        ],
      };
    }
    const lines = fs.readFileSync(chunksPath, "utf8").trim().split("\n").length;
    return {
      content: [{ type: "text", text: `chunks: ${lines}` }],
    };
  },
);

async function main() {
  const transport = new StdioServerTransport();
  await server.connect(transport);
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
