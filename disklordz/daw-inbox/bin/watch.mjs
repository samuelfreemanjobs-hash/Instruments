#!/usr/bin/env node
/**
 * WO-SAAS-016: Watch a download folder for disklordz-*.zip and extract to DAW inbox.
 */
import { createReadStream, existsSync, mkdirSync, renameSync, readFileSync } from "fs";
import { homedir } from "os";
import { basename, dirname, join } from "path";
import chokidar from "chokidar";
import JSZip from "jszip";

const ZIP_RE = /^disklordz-.*\.zip$/i;

function loadConfig() {
  const configPath = process.env.DISKLOORDZ_INBOX_CONFIG
    ?? join(homedir(), ".config", "disklordz", "inbox.json");

  let file = {};
  if (existsSync(configPath)) {
    file = JSON.parse(readFileSync(configPath, "utf8"));
  }

  const downloadDir =
    process.env.DISKLOORDZ_INBOX_DOWNLOAD_DIR
    ?? file.downloadDir
    ?? join(homedir(), "Downloads");

  const inboxDir =
    process.env.DISKLOORDZ_INBOX_TARGET_DIR
    ?? file.inboxDir
    ?? join(homedir(), "Music", "Disklordz", "Inbox");

  const processedDir =
    process.env.DISKLOORDZ_INBOX_PROCESSED_DIR
    ?? file.processedDir
    ?? join(inboxDir, "_processed_zips");

  return { downloadDir, inboxDir, processedDir, configPath };
}

async function extractZip(zipPath, destRoot) {
  const buf = await new Promise((resolve, reject) => {
    const chunks = [];
    createReadStream(zipPath)
      .on("data", (c) => chunks.push(c))
      .on("end", () => resolve(Buffer.concat(chunks)))
      .on("error", reject);
  });

  const zip = await JSZip.loadAsync(buf);
  const folderName = basename(zipPath, ".zip");
  const outDir = join(destRoot, folderName);
  mkdirSync(outDir, { recursive: true });

  for (const [path, entry] of Object.entries(zip.files)) {
    if (entry.dir) continue;
    const data = await entry.async("nodebuffer");
    const target = join(outDir, path);
    mkdirSync(dirname(target), { recursive: true });
    const { writeFileSync } = await import("fs");
    writeFileSync(target, data);
  }

  return outDir;
}

async function handleZip(zipPath, cfg) {
  if (!ZIP_RE.test(basename(zipPath))) return;
  console.log(`[disklordz-inbox] extracting ${zipPath}`);
  const outDir = await extractZip(zipPath, cfg.inboxDir);
  console.log(`[disklordz-inbox] → ${outDir}`);
  mkdirSync(cfg.processedDir, { recursive: true });
  const dest = join(cfg.processedDir, basename(zipPath));
  try {
    renameSync(zipPath, dest);
    console.log(`[disklordz-inbox] archived zip → ${dest}`);
  } catch (err) {
    console.warn(`[disklordz-inbox] could not move zip: ${err.message}`);
  }
}

async function scanExisting(downloadDir, cfg) {
  const { readdirSync } = await import("fs");
  for (const name of readdirSync(downloadDir)) {
    if (ZIP_RE.test(name)) {
      await handleZip(join(downloadDir, name), cfg);
    }
  }
}

async function main() {
  const once = process.argv.includes("--once");
  const cfg = loadConfig();
  mkdirSync(cfg.inboxDir, { recursive: true });

  console.log("[disklordz-inbox] config");
  console.log(`  download:  ${cfg.downloadDir}`);
  console.log(`  inbox:     ${cfg.inboxDir}`);
  console.log(`  processed: ${cfg.processedDir}`);

  await scanExisting(cfg.downloadDir, cfg);
  if (once) {
    console.log("[disklordz-inbox] --once complete");
    return;
  }

  const watcher = chokidar.watch(cfg.downloadDir, {
    ignoreInitial: true,
    awaitWriteFinish: { stabilityThreshold: 800, pollInterval: 100 },
  });

  watcher.on("add", (path) => {
    handleZip(path, cfg).catch((e) => console.error(e));
  });

  console.log("[disklordz-inbox] watching… (Ctrl+C to stop)");
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
