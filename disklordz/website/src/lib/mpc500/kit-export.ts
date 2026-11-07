/**
 * Turn an 808 Lab kit folder (manifest + OneShots/ + Loops/) into an Akai MPC500
 * card folder: short sample names, 12-pad sheet, 16-bit / 44.1 kHz WAV check, RAM budget.
 * See docs/disklordz/spark-le/MPC500-CARD-EXPORT.md.
 */
import { copyFileSync, existsSync, mkdirSync, readFileSync, writeFileSync } from "fs";
import { join } from "path";

/** MPC500 shows and stores at most 16 characters of a sample name. */
export const MPC500_NAME_MAX = 16;
export const MPC500_PAD_COUNT = 12;
export const MPC500_DEFAULT_RAM_MB = 16;

const ROLE_CODES: Record<string, string> = {
  "808_sub": "SUB",
  "808_knock": "KNK",
  snare_body: "SNR",
  snare_snap_clap: "CLP",
  hat_closed: "CHH",
  hat_open: "OHH",
  rim_perc: "RIM",
  cowbell_fx: "FX",
  "808_alt": "ALT",
  perc_2: "PRC",
  vox_chop: "VOX",
  crash_riser: "CRS",
};

export type LabPad = { role: string; file: string; target_lufs?: number };
export type LabOverflow = LabPad & { mpc_pad?: number };

export type LabManifest = {
  lane?: string;
  preset_id?: string;
  bpm?: number;
  swing_note?: string;
  spark_le: { pads: Record<string, LabPad> };
  loops?: { id: string; file: string; bars: number }[];
  overflow?: LabOverflow[];
};

export type WavInfo = { channels: number; sampleRate: number; bitsPerSample: number; dataBytes: number };

export type Mpc500Assignment = {
  pad: string;
  role: string;
  sampleName: string;
  source: string;
  targetLufs?: number;
};

export type Mpc500Loop = { sampleName: string; source: string; bars: number };

export type Mpc500Kit = {
  code: string;
  pads: Mpc500Assignment[];
  loops: Mpc500Loop[];
  totalBytes: number;
  ramBudgetBytes: number;
  warnings: string[];
};

function clean(s: string): string {
  return s.toUpperCase().replace(/[^A-Z0-9]/g, "");
}

/** e.g. SMOOTH-JAZZ-RAP @ 86 → SJR86 (max 8 chars so names stay ≤ 16). */
export function kitCode(lane: string, bpm: number | undefined): string {
  const initials = lane
    .split(/[^A-Za-z0-9]+/)
    .filter(Boolean)
    .map((w) => w[0])
    .join("");
  const code = clean(initials).slice(0, 4) + (bpm ? String(Math.round(bpm)) : "");
  return (code || "KIT").slice(0, 8);
}

export function roleCode(role: string): string {
  return ROLE_CODES[role] ?? (clean(role).slice(0, 3) || "SMP");
}

export function padLabel(pad: number): string {
  return `A${String(pad).padStart(2, "0")}`;
}

export function parseWavHeader(buf: Buffer): WavInfo {
  if (buf.length < 12 || buf.toString("ascii", 0, 4) !== "RIFF" || buf.toString("ascii", 8, 12) !== "WAVE") {
    throw new Error("not a RIFF/WAVE file");
  }
  let fmt: Omit<WavInfo, "dataBytes"> & { format: number } | null = null;
  let dataBytes = -1;
  let off = 12;
  while (off + 8 <= buf.length) {
    const id = buf.toString("ascii", off, off + 4);
    const size = buf.readUInt32LE(off + 4);
    const body = off + 8;
    if (id === "fmt ") {
      fmt = {
        format: buf.readUInt16LE(body),
        channels: buf.readUInt16LE(body + 2),
        sampleRate: buf.readUInt32LE(body + 4),
        bitsPerSample: buf.readUInt16LE(body + 14),
      };
    } else if (id === "data") {
      dataBytes = size;
    }
    off = body + size + (size % 2);
  }
  if (!fmt) throw new Error("missing fmt chunk");
  if (dataBytes < 0) throw new Error("missing data chunk");
  if (fmt.format !== 1 && fmt.format !== 0xfffe) throw new Error(`unsupported WAV format ${fmt.format} (need PCM)`);
  return { channels: fmt.channels, sampleRate: fmt.sampleRate, bitsPerSample: fmt.bitsPerSample, dataBytes };
}

/** MPC500 loads 16-bit 44.1 kHz mono/stereo PCM; anything else must be converted first. */
export function checkMpc500Wav(info: WavInfo, label: string): void {
  if (info.bitsPerSample !== 16) throw new Error(`${label}: ${info.bitsPerSample}-bit, MPC500 needs 16-bit`);
  if (info.sampleRate !== 44100) throw new Error(`${label}: ${info.sampleRate} Hz, MPC500 needs 44100 Hz`);
  if (info.channels !== 1 && info.channels !== 2) throw new Error(`${label}: ${info.channels} channels`);
}

export function planMpc500Kit(
  kitDir: string,
  manifest: LabManifest,
  opts: { code?: string; ramMb?: number } = {},
): Mpc500Kit {
  const code = clean(opts.code ?? kitCode(manifest.lane ?? manifest.preset_id ?? "KIT", manifest.bpm)).slice(0, 8);
  const ramBudgetBytes = (opts.ramMb ?? MPC500_DEFAULT_RAM_MB) * 1024 * 1024;
  const warnings: string[] = [];
  const used = new Set<string>();
  let totalBytes = 0;

  const claim = (name: string): string => {
    const n = name.slice(0, MPC500_NAME_MAX);
    if (used.has(n)) throw new Error(`duplicate MPC500 sample name ${n}`);
    used.add(n);
    return n;
  };

  const measure = (rel: string): void => {
    const full = join(kitDir, rel);
    if (!existsSync(full)) throw new Error(`missing file ${rel}`);
    const info = parseWavHeader(readFileSync(full));
    checkMpc500Wav(info, rel);
    totalBytes += info.dataBytes;
  };

  const entries: { pad: number; p: LabPad }[] = Object.entries(manifest.spark_le.pads).map(([k, p]) => ({
    pad: Number(k),
    p,
  }));
  for (const o of manifest.overflow ?? []) {
    if (o.mpc_pad) entries.push({ pad: o.mpc_pad, p: o });
  }

  const taken = new Set<number>();
  const pads: Mpc500Assignment[] = [];
  for (const { pad, p } of entries.sort((a, b) => a.pad - b.pad)) {
    if (!Number.isInteger(pad) || pad < 1 || pad > MPC500_PAD_COUNT) {
      throw new Error(`pad ${pad} outside MPC500 bank A (1-${MPC500_PAD_COUNT})`);
    }
    if (taken.has(pad)) throw new Error(`pad ${pad} assigned twice`);
    taken.add(pad);
    measure(p.file);
    pads.push({
      pad: padLabel(pad),
      role: p.role,
      sampleName: claim(`${code}_${String(pad).padStart(2, "0")}${roleCode(p.role)}`),
      source: p.file,
      targetLufs: p.target_lufs,
    });
  }

  const loops: Mpc500Loop[] = (manifest.loops ?? []).map((l, i) => {
    measure(l.file);
    const word = clean(l.id.split(/[_-]/)[0] ?? "").slice(0, 4) || "LOOP";
    return { sampleName: claim(`${code}_L${i + 1}${word}`), source: l.file, bars: l.bars };
  });

  if (totalBytes > ramBudgetBytes) {
    throw new Error(
      `kit needs ${(totalBytes / 1048576).toFixed(1)} MB, over the ${opts.ramMb ?? MPC500_DEFAULT_RAM_MB} MB RAM budget`,
    );
  }
  if (totalBytes > ramBudgetBytes * 0.8) {
    warnings.push(`kit uses ${Math.round((totalBytes / ramBudgetBytes) * 100)}% of MPC500 RAM budget`);
  }
  return { code, pads, loops, totalBytes, ramBudgetBytes, warnings };
}

export function padSheet(kit: Mpc500Kit, manifest: LabManifest): string {
  const lines = [
    `DISKLORDZ MPC500 KIT ${kit.code}`,
    `Lane: ${manifest.lane ?? "-"}  Preset: ${manifest.preset_id ?? "-"}  BPM: ${manifest.bpm ?? "-"}`,
  ];
  if (manifest.swing_note) lines.push(`Swing: ${manifest.swing_note}`);
  lines.push("", "PAD  ROLE              SAMPLE");
  for (const p of kit.pads) lines.push(`${p.pad}  ${p.role.padEnd(16)}  ${p.sampleName}`);
  const empty = Array.from({ length: MPC500_PAD_COUNT }, (_, i) => padLabel(i + 1)).filter(
    (l) => !kit.pads.some((p) => p.pad === l),
  );
  if (empty.length) lines.push(`${empty.join(" ")}  (empty: add overflow entries with mpc_pad 9-12)`);
  if (kit.loops.length) {
    lines.push("", "LOOPS (load to a free pad or bank B)");
    for (const l of kit.loops) lines.push(`  ${l.sampleName}  ${l.bars} bars  (${l.source})`);
  }
  lines.push(
    "",
    `Memory: ${(kit.totalBytes / 1048576).toFixed(2)} MB of ${(kit.ramBudgetBytes / 1048576).toFixed(0)} MB budget`,
  );
  return lines.join("\r\n") + "\r\n";
}

export function writeMpc500Kit(kitDir: string, outDir: string, kit: Mpc500Kit, manifest: LabManifest): void {
  mkdirSync(outDir, { recursive: true });
  for (const s of [...kit.pads, ...kit.loops]) {
    copyFileSync(join(kitDir, s.source), join(outDir, `${s.sampleName}.WAV`));
  }
  writeFileSync(join(outDir, "PADS.TXT"), padSheet(kit, manifest));
  writeFileSync(
    join(outDir, "mpc500-kit.json"),
    JSON.stringify({ format: "DISKLORDZ_MPC500_KIT", version: 1, lane: manifest.lane, bpm: manifest.bpm, ...kit }, null, 2),
  );
}
