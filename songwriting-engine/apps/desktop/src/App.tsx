import { useState } from "react";
import { invoke } from "@tauri-apps/api/core";
import {
  AudioLines,
  Check,
  ChevronDown,
  CircleHelp,
  Download,
  FileMusic,
  FolderOpen,
  LoaderCircle,
  Music2,
  Plus,
  Sparkles,
  Waves,
} from "lucide-react";

type TrackSummary = { name: string; notes: number };
type SectionSummary = { kind: string; start_bar: number; bars: number };
type GenerateSummary = {
  valid: boolean;
  project: string;
  midi: string[];
  report: string;
  bars: number;
  title: string;
  tempo_bpm: number;
  key: string;
  scale: string;
  seed: number;
  style: string;
  sections: SectionSummary[];
  tracks: TrackSummary[];
};

const styles = [
  { id: "west_coast_smoker", label: "West Coast Smoker" },
  { id: "quiet_storm", label: "Quiet Storm" },
  { id: "gospel_rnb", label: "Gospel R&B" },
  { id: "dusty_jazz_rap", label: "Dusty Jazz-Rap" },
];

const trackColors: Record<string, string> = {
  "Poly Pad": "#e0a64b",
  "Electric Piano": "#76c6ad",
  "Synth Bass": "#e47658",
  Lead: "#8db6d8",
};

function makeSections(totalBars: number) {
  if (totalBars === 72) return [
    { type: "intro", bars: 8 }, { type: "verse", bars: 16 }, { type: "hook", bars: 8 },
    { type: "verse", bars: 16 }, { type: "hook", bars: 8 }, { type: "bridge", bars: 8 }, { type: "outro", bars: 8 },
  ];
  const template: [string, number][] = totalBars >= 32
    ? [["intro", 4], ["verse", 8], ["hook", 4], ["verse", 8], ["hook", 4], ["outro", 4]]
    : [["intro", 2], ["verse", 6], ["hook", 4], ["verse", 2], ["outro", 2]];
  const sections: { type: string; bars: number }[] = [];
  let remaining = totalBars;
  let index = 0;
  while (remaining > 0) {
    const [type, preferredBars] = template[index % template.length];
    const bars = Math.min(preferredBars, remaining);
    sections.push({ type, bars });
    remaining -= bars;
    index += 1;
  }
  return sections;
}

function App() {
  const [title, setTitle] = useState("Smoker Sketch 001");
  const [tempo, setTempo] = useState(95);
  const [arrangementBars, setArrangementBars] = useState(72);
  const [key, setKey] = useState("D");
  const [scale, setScale] = useState("natural_minor");
  const [style, setStyle] = useState("west_coast_smoker");
  const [progression, setProgression] = useState("Dm9, Gm9, Bbmaj7, A7");
  const [brief, setBrief] = useState("");
  const [seed, setSeed] = useState(48127);
  const [swing, setSwing] = useState(0.12);
  const [format, setFormat] = useState<"1" | "0" | "both">("both");
  const [result, setResult] = useState<GenerateSummary | null>(null);
  const [error, setError] = useState("");
  const [busy, setBusy] = useState(false);

  const chordSymbols = progression.split(",").map((item) => item.trim()).filter(Boolean);
  const sections = makeSections(arrangementBars);
  const displayedSections = result?.sections ?? sections.map((section, index) => ({
    kind: section.type,
    start_bar: sections.slice(0, index).reduce((sum, item) => sum + item.bars, 0),
    bars: section.bars,
  }));

  async function generate() {
    setBusy(true);
    setError("");
    setResult(null);
    const config = {
      project: { title, schema_version: 1, seed },
      music: { tempo_bpm: tempo, time_signature: [4, 4], key, scale, swing },
      style: { profile: style, harmonic_density: 0.7, melodic_density: 0.4, rhythmic_density: 0.5 },
      harmony: { progression: chordSymbols, bars_per_chord: 1, voice_leading: true },
      arrangement: { sections },
      tracks: { pad: true, electric_piano: true, bass: true, lead: true },
      export: { format: 1, midi_ppq: 960, include_tempo: true, include_markers: true },
    };
    try {
      const formats = format === "both" ? [0, 1] : [Number(format)];
      const generated = brief.trim()
        ? await invoke<GenerateSummary>("generate_song_from_prompt", { prompt: brief, seed, formats })
        : await invoke<GenerateSummary>("generate_song", { config, formats });
      setResult(generated);
    } catch (reason) {
      setError(String(reason));
    } finally {
      setBusy(false);
    }
  }

  return (
    <main className="app-shell">
      <header className="topbar">
        <div className="brand-lockup">
          <div className="brand-mark"><AudioLines size={19} strokeWidth={1.8} /></div>
          <span>ANTIGRAVITY <b>SONGWRITER</b></span>
        </div>
        <div className="session-meta"><span className="live-dot" /> ENGINE READY <span className="meta-divider">/</span> MIDI FIRST</div>
        <button className="icon-button help-button" title="About this engine" aria-label="About this engine"><CircleHelp size={17} /></button>
      </header>

      <div className="workspace">
        <aside className="settings-panel">
          <div className="panel-heading">
            <div className="eyebrow">COMPOSITION / 01</div>
            <h1>New session</h1>
          </div>

          <label className="field-label" htmlFor="title">SESSION NAME</label>
          <input id="title" value={title} onChange={(event) => setTitle(event.target.value)} />

          <label className="field-label brief-label" htmlFor="brief">CREATIVE BRIEF <span>OPTIONAL / DETERMINISTIC</span></label>
          <textarea id="brief" value={brief} onChange={(event) => setBrief(event.target.value)} placeholder="95 BPM, D minor, smoky West Coast. Chords: Dm9, Gm9, Bbmaj7, A7. 32 bars." />

          <div className="field-row">
            <div className="field-block">
              <label className="field-label" htmlFor="tempo">TEMPO</label>
              <div className="input-suffix"><input id="tempo" type="number" min="20" max="300" value={tempo} onChange={(event) => setTempo(Number(event.target.value))} /><span>BPM</span></div>
            </div>
            <div className="field-block">
              <label className="field-label" htmlFor="bars">SONG LENGTH</label>
              <div className="input-suffix"><input id="bars" type="number" min="1" max="512" value={arrangementBars} onChange={(event) => setArrangementBars(Math.max(1, Math.min(512, Number(event.target.value) || 1)))} /><span>BARS</span></div>
            </div>
          </div>
          <div className="field-block seed-field">
            <label className="field-label" htmlFor="seed">GENERATION SEED</label>
            <input id="seed" type="number" value={seed} onChange={(event) => setSeed(Number(event.target.value))} />
          </div>

          <div className="field-row">
            <div className="field-block">
              <label className="field-label" htmlFor="key">KEY</label>
              <div className="select-wrap"><select id="key" value={key} onChange={(event) => setKey(event.target.value)}>{["C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"].map((note) => <option key={note}>{note}</option>)}</select><ChevronDown size={14} /></div>
            </div>
            <div className="field-block">
              <label className="field-label" htmlFor="scale">SCALE</label>
              <div className="select-wrap"><select id="scale" value={scale} onChange={(event) => setScale(event.target.value)}>{[["natural_minor", "Natural minor"], ["major", "Major"], ["dorian", "Dorian"], ["harmonic_minor", "Harmonic minor"], ["minor_pentatonic", "Minor pentatonic"], ["blues", "Blues"]].map(([id, label]) => <option value={id} key={id}>{label}</option>)}</select><ChevronDown size={14} /></div>
            </div>
          </div>

          <label className="field-label" htmlFor="style">STYLE PROFILE</label>
          <div className="select-wrap style-select"><select id="style" value={style} onChange={(event) => setStyle(event.target.value)}>{styles.map((item) => <option value={item.id} key={item.id}>{item.label}</option>)}</select><ChevronDown size={14} /></div>

          <label className="field-label progression-label" htmlFor="progression">CHORD LOOP <span>COMMA SEPARATED</span></label>
          <input id="progression" className="chord-input" value={progression} onChange={(event) => setProgression(event.target.value)} />
          <div className="chord-chips">{chordSymbols.map((chord, index) => <span className="chord-chip" key={`${chord}-${index}`}><small>{String(index + 1).padStart(2, "0")}</small>{chord}</span>)}<button className="chip-add" title="Add a chord" aria-label="Add a chord" onClick={() => setProgression(`${progression}${progression.trim() ? ", " : ""}Cmaj7`)}><Plus size={14} /></button></div>

          <div className="slider-row"><label className="field-label" htmlFor="swing">SWING</label><span className="slider-value">{Math.round(swing * 100)}%</span></div>
          <input id="swing" className="range-input" type="range" min="0" max="0.5" step="0.01" value={swing} onChange={(event) => setSwing(Number(event.target.value))} />

          <div className="format-block">
            <div className="field-label">MIDI EXPORT</div>
            <div className="segmented" role="group" aria-label="MIDI export format">
              {([["1", "TYPE 1"], ["0", "TYPE 0"], ["both", "BOTH"]] as const).map(([value, label]) => <button key={value} className={format === value ? "selected" : ""} onClick={() => setFormat(value)}>{label}</button>)}
            </div>
          </div>

          <button className="generate-button" onClick={generate} disabled={busy || (!brief.trim() && (chordSymbols.length === 0 || !title.trim()))}>
            {busy ? <LoaderCircle size={17} className="spin" /> : <Sparkles size={17} />}
            {busy ? "COMPOSING" : "GENERATE ARRANGEMENT"}
          </button>
          {error && <div className="error-message">{error}</div>}
          <div className="settings-foot"><span>DETERMINISTIC ENGINE</span><span>v0.1.0</span></div>
        </aside>

        <section className="arrangement-panel">
          <div className="arrangement-head">
            <div>
              <div className="eyebrow">ARRANGEMENT / OVERVIEW</div>
              <h2>{result ? result.title : "Your next idea starts here."}</h2>
            </div>
            {result && <div className="complete-badge"><Check size={14} /> VALIDATED</div>}
          </div>

          {result ? (
            <>
              <div className="summary-strip">
                <div><span>TEMPO</span><strong>{result.tempo_bpm}<small> BPM</small></strong></div>
                <div><span>KEY / SCALE</span><strong>{result.key} <small>{result.scale.replaceAll("_", " ")}</small></strong></div>
                <div><span>ARRANGEMENT</span><strong>{result.bars}<small> BARS</small></strong></div>
                <div><span>SEED</span><strong>{result.seed}</strong></div>
              </div>
              <div className="section-heading"><span>SONG FORM</span><span>{displayedSections.length} SECTIONS <i>·</i> {result.bars} BARS</span></div>
              <div className="song-form" aria-label="Song arrangement sections">
                {displayedSections.map((section, index) => <div className={`section-block section-${section.kind}`} style={{ flexGrow: section.bars }} key={`${section.kind}-${index}`}><span>{section.kind.toUpperCase()}</span><small>{section.bars} BARS</small></div>)}
              </div>
              <div className="section-heading tracks-heading"><span>GENERATED PARTS</span><span>{result.tracks.reduce((total, track) => total + track.notes, 0)} MIDI EVENTS</span></div>
              <div className="track-list">
                {result.tracks.map((track, index) => <div className="track-row" key={track.name}>
                  <div className="track-index">0{index + 1}</div>
                  <div className="track-name"><i style={{ background: trackColors[track.name] ?? "#93a79d" }} /><div><strong>{track.name}</strong><small>{["PAD / HARMONY", "KEYS / COMPING", "LOW END", "MELODIC VOICE"][index] ?? "MIDI PART"}</small></div></div>
                  <div className="note-activity" aria-label={`${track.notes} notes`}><div className="activity-fill" style={{ width: `${Math.max(5, Math.min(100, track.notes / 3.24))}%`, background: trackColors[track.name] ?? "#93a79d" }} /></div>
                  <div className="note-count">{track.notes}<small> NOTES</small></div>
                  <button className="icon-button track-action" title="MIDI part generated"><Waves size={15} /></button>
                </div>)}
              </div>
              <div className="export-row">
                <div className="export-status"><Check size={15} /><span>Project JSON and MIDI files passed structural validation.</span></div>
                <button className="export-button" onClick={() => invoke("reveal_artifact", { path: result.project })} title={result.project}><FolderOpen size={16} /> OUTPUT PATH <Download size={14} /></button>
              </div>
              <div className="artifact-list"><FileMusic size={15} /><span>{result.midi.map((path) => path.split(/[\\/]/).pop()).join("  ·  ")}</span><span className="report-name">{result.report.split(/[\\/]/).pop()}</span></div>
            </>
          ) : (
            <div className="empty-state">
              <div className="empty-icon"><Music2 size={25} strokeWidth={1.4} /></div>
              <div className="empty-title">A loop with somewhere to go.</div>
              <p>Set a harmonic center, choose a feel, and generate a complete editable MIDI arrangement.</p>
              <div className="empty-spec"><span>4 COORDINATED PARTS</span><i /> <span>SEEDED GENERATION</span><i /> <span>MIDI 0 / 1</span></div>
              <div className="ghost-form">
                <div className="ghost-form-head"><span>DEFAULT SESSION</span><span>READY TO GENERATE</span></div>
                <div className="ghost-chords">{chordSymbols.map((chord, index) => <div className="ghost-chord" key={`${chord}-${index}`}><small>CHORD {index + 1}</small><strong>{chord}</strong></div>)}</div>
                <div className="ghost-rail"><span>INTRO</span><span>VERSE</span><span>HOOK</span><span>VERSE</span><span>HOOK</span><span>BRIDGE</span><span>OUTRO</span></div>
              </div>
            </div>
          )}
          <footer className="arrangement-footer"><span><span className="footer-indicator" /> LOCAL GENERATION</span><span>NO AI API REQUIRED <i>·</i> MIDI VALIDATION ON EXPORT</span></footer>
        </section>
      </div>
    </main>
  );
}

export default App;
